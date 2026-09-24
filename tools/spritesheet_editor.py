"""Small Tkinter editor for manually selecting sprite animation frames.

Run with: python tools/spritesheet_editor.py
Only the Python standard library is required.
"""

from __future__ import annotations

import json
import math
import os
import tkinter as tk
from dataclasses import dataclass, field
from tkinter import filedialog, messagebox, ttk


@dataclass
class AnimationDefinition:
    name: str
    loop: bool = True
    frame_duration_ms: int = 100
    frames: list[dict[str, int]] = field(default_factory=list)


class SpritesheetEditor:
    ZOOMS = (0.25, 0.5, 1.0, 2.0, 4.0)
    COLORS = ("#ff5c5c", "#56d4ff", "#f6d743", "#78e08f", "#c77dff")

    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("Spritesheet Animation Editor")
        self.root.geometry("1320x820")
        self.root.minsize(900, 600)

        self.image_path = ""
        self.source_image: tk.PhotoImage | None = None
        self.display_image: tk.PhotoImage | None = None
        self.image_size = (0, 0)
        self.zoom = 1.0
        self.animations: list[AnimationDefinition] = []
        self.drag_start: tuple[float, float] | None = None
        self.moving_selection = False
        self.resizing_selection: str | None = None
        self.selection_at_drag_start: tuple[int, int, int, int] | None = None
        self.editing_frame: tuple[int, int] | None = None
        self.clipboard_selection: tuple[tuple[int, int, int, int], dict[str, int]] | None = None
        self.selection: tuple[int, int, int, int] | None = None
        self.selection_item: int | None = None

        self.sprite_path_var = tk.StringVar(value="assets/sprites/player_spritesheet.png")
        self.animation_name_var = tk.StringVar()
        self.duration_var = tk.StringVar(value="100")
        self.loop_var = tk.BooleanVar(value=True)
        self.canvas_width_var = tk.StringVar(value="64")
        self.canvas_height_var = tk.StringVar(value="64")
        self.offset_x_var = tk.StringVar(value="0")
        self.offset_y_var = tk.StringVar(value="0")
        self.selection_info_var = tk.StringVar(value="Aucune frame sélectionnée")
        self.image_info_var = tk.StringVar(value="Aucune spritesheet chargée")
        self.zoom_var = tk.StringVar(value="100 %")

        self._build_ui()
        self.root.bind_all("<Control-c>", self.copy_selection)
        self.root.bind_all("<Control-C>", self.copy_selection)
        self.root.bind_all("<Control-v>", self.paste_selection)
        self.root.bind_all("<Control-V>", self.paste_selection)

    def _build_ui(self) -> None:
        toolbar = ttk.Frame(self.root, padding=(8, 8, 8, 4))
        toolbar.pack(fill=tk.X)
        ttk.Button(toolbar, text="Ouvrir une spritesheet…", command=self.open_image).pack(side=tk.LEFT)
        ttk.Button(toolbar, text="Charger JSON…", command=self.load_json).pack(side=tk.LEFT, padx=(6, 0))
        ttk.Label(toolbar, text="Chemin spritePath exporté :").pack(side=tk.LEFT, padx=(12, 4))
        ttk.Entry(toolbar, textvariable=self.sprite_path_var, width=42).pack(side=tk.LEFT, fill=tk.X, expand=True)
        ttk.Label(toolbar, text="Zoom :").pack(side=tk.LEFT, padx=(12, 4))
        zoom_box = ttk.Combobox(toolbar, textvariable=self.zoom_var, width=8,
                                values=("25 %", "50 %", "100 %", "200 %", "400 %"), state="readonly")
        zoom_box.pack(side=tk.LEFT)
        zoom_box.bind("<<ComboboxSelected>>", self._change_zoom)
        ttk.Button(toolbar, text="Exporter JSON…", command=self.export_json).pack(side=tk.LEFT, padx=(12, 0))

        body = ttk.Panedwindow(self.root, orient=tk.HORIZONTAL)
        body.pack(fill=tk.BOTH, expand=True, padx=8, pady=4)

        left = ttk.Frame(body, padding=6, width=250)
        center = ttk.Frame(body, padding=4)
        right = ttk.Frame(body, padding=6, width=270)
        body.add(left, weight=0)
        body.add(center, weight=1)
        body.add(right, weight=0)

        self._build_animation_panel(left)
        self._build_image_panel(center)
        self._build_frame_panel(right)
        ttk.Label(self.root, textvariable=self.image_info_var, anchor=tk.W, padding=(10, 4)).pack(fill=tk.X)

    def _build_animation_panel(self, parent: ttk.Frame) -> None:
        ttk.Label(parent, text="Animations", font=("Segoe UI", 11, "bold")).pack(anchor=tk.W)
        list_frame = ttk.Frame(parent)
        list_frame.pack(fill=tk.BOTH, expand=True, pady=(5, 8))
        self.animation_list = tk.Listbox(list_frame, height=10, exportselection=False)
        scrollbar = ttk.Scrollbar(list_frame, orient=tk.VERTICAL, command=self.animation_list.yview)
        self.animation_list.configure(yscrollcommand=scrollbar.set)
        self.animation_list.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.animation_list.bind("<<ListboxSelect>>", self._select_animation)

        ttk.Label(parent, text="Nom").pack(anchor=tk.W)
        ttk.Entry(parent, textvariable=self.animation_name_var).pack(fill=tk.X, pady=(2, 6))
        ttk.Label(parent, text="Durée d’une frame (ms)").pack(anchor=tk.W)
        ttk.Entry(parent, textvariable=self.duration_var).pack(fill=tk.X, pady=(2, 6))
        ttk.Checkbutton(parent, text="Boucler l’animation", variable=self.loop_var).pack(anchor=tk.W, pady=(0, 8))

        buttons = ttk.Frame(parent)
        buttons.pack(fill=tk.X)
        ttk.Button(buttons, text="Nouvelle", command=self.create_animation).pack(side=tk.LEFT, fill=tk.X, expand=True)
        ttk.Button(buttons, text="Appliquer", command=self.apply_animation_settings).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(5, 0))
        ttk.Button(parent, text="Supprimer l’animation", command=self.delete_animation).pack(fill=tk.X, pady=(5, 0))

        ttk.Separator(parent).pack(fill=tk.X, pady=12)
        ttk.Label(parent, text="Canevas logique de la frame", font=("Segoe UI", 10, "bold")).pack(anchor=tk.W)
        ttk.Label(parent, text="Taille commune permettant d’aligner les frames.").pack(anchor=tk.W, pady=(2, 6))
        canvas_row = ttk.Frame(parent)
        canvas_row.pack(fill=tk.X)
        ttk.Label(canvas_row, text="Largeur").pack(side=tk.LEFT)
        ttk.Entry(canvas_row, textvariable=self.canvas_width_var, width=7).pack(side=tk.LEFT, padx=(4, 10))
        ttk.Label(canvas_row, text="Hauteur").pack(side=tk.LEFT)
        ttk.Entry(canvas_row, textvariable=self.canvas_height_var, width=7).pack(side=tk.LEFT, padx=4)
        offset_row = ttk.Frame(parent)
        offset_row.pack(fill=tk.X, pady=(6, 0))
        ttk.Label(offset_row, text="Décalage X").pack(side=tk.LEFT)
        ttk.Entry(offset_row, textvariable=self.offset_x_var, width=7).pack(side=tk.LEFT, padx=(4, 10))
        ttk.Label(offset_row, text="Y").pack(side=tk.LEFT)
        ttk.Entry(offset_row, textvariable=self.offset_y_var, width=7).pack(side=tk.LEFT, padx=4)
        ttk.Label(parent, text="Les valeurs sont appliquées à la prochaine frame ajoutée.",
                  wraplength=230).pack(anchor=tk.W, pady=(6, 0))

    def _build_image_panel(self, parent: ttk.Frame) -> None:
        parent.columnconfigure(0, weight=1)
        parent.rowconfigure(0, weight=1)
        self.canvas = tk.Canvas(parent, background="#25282e", highlightthickness=0,
                                cursor="crosshair")
        horizontal = ttk.Scrollbar(parent, orient=tk.HORIZONTAL, command=self.canvas.xview)
        vertical = ttk.Scrollbar(parent, orient=tk.VERTICAL, command=self.canvas.yview)
        self.canvas.configure(xscrollcommand=horizontal.set, yscrollcommand=vertical.set)
        self.canvas.grid(row=0, column=0, sticky="nsew")
        vertical.grid(row=0, column=1, sticky="ns")
        horizontal.grid(row=1, column=0, sticky="ew")
        self.canvas.bind("<ButtonPress-1>", self._begin_selection)
        self.canvas.bind("<B1-Motion>", self._drag_selection)
        self.canvas.bind("<ButtonRelease-1>", self._finish_selection)
        self.canvas.bind("<Motion>", self._update_selection_cursor)
        self.canvas.bind("<MouseWheel>", self._mouse_wheel)

    def _build_frame_panel(self, parent: ttk.Frame) -> None:
        ttk.Label(parent, text="Frames de l’animation", font=("Segoe UI", 11, "bold")).pack(anchor=tk.W)
        ttk.Label(parent, text="Glisse pour créer. Tire le cadre pour le déplacer ou ses bords pour le redimensionner. Maj + glisser crée un nouveau cadre.",
                  wraplength=250).pack(anchor=tk.W, pady=(3, 8))
        self.frame_list = tk.Listbox(parent, height=18, exportselection=False)
        self.frame_list.pack(fill=tk.BOTH, expand=True)
        self.frame_list.bind("<<ListboxSelect>>", self._select_frame)
        ttk.Label(parent, textvariable=self.selection_info_var, wraplength=250).pack(anchor=tk.W, pady=(8, 5))
        clipboard_buttons = ttk.Frame(parent)
        clipboard_buttons.pack(fill=tk.X, pady=(0, 5))
        ttk.Button(clipboard_buttons, text="Copier sélection", command=self.copy_selection).pack(side=tk.LEFT, fill=tk.X, expand=True)
        ttk.Button(clipboard_buttons, text="Coller sélection", command=self.paste_selection).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(5, 0))
        ttk.Button(parent, text="Ajouter la sélection comme frame", command=self.add_selected_frame).pack(fill=tk.X)
        frame_buttons = ttk.Frame(parent)
        frame_buttons.pack(fill=tk.X, pady=(5, 0))
        ttk.Button(frame_buttons, text="↑", width=5, command=lambda: self.move_frame(-1)).pack(side=tk.LEFT)
        ttk.Button(frame_buttons, text="↓", width=5, command=lambda: self.move_frame(1)).pack(side=tk.LEFT, padx=4)
        ttk.Button(frame_buttons, text="Supprimer", command=self.delete_frame).pack(side=tk.LEFT, fill=tk.X, expand=True)

    def load_json(self) -> None:
        json_path = filedialog.askopenfilename(
            title="Choisir le fichier d’animations",
            filetypes=(("Fichiers JSON", "*.json"), ("Tous les fichiers", "*.*")),
        )
        if not json_path:
            return
        try:
            with open(json_path, "r", encoding="utf-8") as json_file:
                data = json.load(json_file)
            parsed_animations = []
            for entry in data["animations"]:
                frames = []
                for frame in entry["frames"]:
                    width, height = int(frame["width"]), int(frame["height"])
                    frames.append({
                        "x": int(frame["x"]), "y": int(frame["y"]),
                        "width": width, "height": height,
                        "offsetX": int(frame.get("offsetX", 0)),
                        "offsetY": int(frame.get("offsetY", 0)),
                        "canvasWidth": int(frame.get("canvasWidth", width)),
                        "canvasHeight": int(frame.get("canvasHeight", height)),
                    })
                parsed_animations.append(AnimationDefinition(
                    name=str(entry["name"]),
                    loop=bool(entry.get("loop", True)),
                    frame_duration_ms=int(entry.get("frameDurationMs", 100)),
                    frames=frames,
                ))
            if not parsed_animations:
                raise ValueError("Le JSON ne contient aucune animation.")
        except (OSError, json.JSONDecodeError, KeyError, TypeError, ValueError) as error:
            messagebox.showerror("JSON invalide", f"Impossible de charger les animations :\n{error}")
            return

        sprite_path = filedialog.askopenfilename(
            title="Choisir la spritesheet correspondante",
            filetypes=(("Images PNG", "*.png"), ("Images GIF", "*.gif"),
                       ("Toutes les images", "*.*")),
        )
        if not sprite_path:
            return
        try:
            image = tk.PhotoImage(file=sprite_path)
        except tk.TclError as error:
            messagebox.showerror("Image illisible", str(error))
            return
        image_size = (image.width(), image.height())
        for animation in parsed_animations:
            for frame in animation.frames:
                if (frame["x"] < 0 or frame["y"] < 0 or frame["width"] <= 0 or
                        frame["height"] <= 0 or frame["x"] + frame["width"] > image_size[0] or
                        frame["y"] + frame["height"] > image_size[1]):
                    messagebox.showerror(
                        "Frame hors image",
                        f"Une frame de « {animation.name} » dépasse les limites de la spritesheet choisie.",
                    )
                    return

        self.animations = parsed_animations
        self.image_path = os.path.abspath(sprite_path)
        self.source_image = image
        self.image_size = image_size
        self.selection = None
        self.editing_frame = None
        self.clipboard_selection = None
        self.sprite_path_var.set(str(data.get("spritePath", "")))
        basename = os.path.basename(sprite_path)
        self.image_info_var.set(f"{basename} — {image_size[0]} × {image_size[1]} px")
        self._refresh_animation_list(0)
        self._render_sheet()

    def open_image(self) -> None:
        path = filedialog.askopenfilename(
            title="Charger une spritesheet",
            filetypes=(("Images PNG", "*.png"), ("Images GIF", "*.gif"), ("Toutes les images", "*.*")),
        )
        if not path:
            return
        try:
            image = tk.PhotoImage(file=path)
        except tk.TclError as error:
            messagebox.showerror("Image illisible", str(error))
            return

        self.image_path = os.path.abspath(path)
        self.source_image = image
        self.image_size = (image.width(), image.height())
        self.selection = None
        self.editing_frame = None
        self.clipboard_selection = None
        basename = os.path.basename(path)
        self.sprite_path_var.set(f"assets/sprites/{basename}")
        self.image_info_var.set(f"{basename} — {self.image_size[0]} × {self.image_size[1]} px")
        self._render_sheet()

    def _change_zoom(self, _event: tk.Event | None = None) -> None:
        self.zoom = int(self.zoom_var.get().replace(" %", "")) / 100.0
        self._render_sheet()

    def _mouse_wheel(self, event: tk.Event) -> str:
        current = self.ZOOMS.index(self.zoom) if self.zoom in self.ZOOMS else 2
        current = max(0, min(len(self.ZOOMS) - 1, current + (1 if event.delta > 0 else -1)))
        self.zoom = self.ZOOMS[current]
        self.zoom_var.set(f"{int(self.zoom * 100)} %")
        self._render_sheet()
        return "break"

    def _scaled_image(self) -> tk.PhotoImage | None:
        if self.source_image is None:
            return None
        if self.zoom == 1.0:
            return self.source_image
        if self.zoom > 1.0:
            return self.source_image.zoom(int(self.zoom))
        return self.source_image.subsample(int(round(1.0 / self.zoom)))

    def _render_sheet(self) -> None:
        self.canvas.delete("all")
        if self.source_image is None:
            self.canvas.configure(scrollregion=(0, 0, 1, 1))
            return
        self.display_image = self._scaled_image()
        self.canvas.create_image(0, 0, image=self.display_image, anchor=tk.NW, tags=("sheet",))
        width = self.display_image.width()
        height = self.display_image.height()
        self.canvas.configure(scrollregion=(0, 0, width, height))

        for animation_index, animation in enumerate(self.animations):
            color = self.COLORS[animation_index % len(self.COLORS)]
            for index, frame in enumerate(animation.frames):
                x1, y1, x2, y2 = self._display_rect(frame)
                self.canvas.create_rectangle(x1, y1, x2, y2, outline=color,
                                             width=2, tags=("frame",))
                self.canvas.create_text(x1 + 3, y1 + 3, text=f"{animation.name}:{index}",
                                        fill=color, anchor=tk.NW, tags=("frame",))
        if self.selection is not None:
            x1, y1, x2, y2 = self._display_rect_from_bounds(self.selection)
            self.selection_item = self.canvas.create_rectangle(
                x1, y1, x2, y2, outline="#ffffff", width=2, dash=(5, 3), tags=("selection",))

    def _display_rect(self, frame: dict[str, int]) -> tuple[float, float, float, float]:
        return self._display_rect_from_bounds((frame["x"], frame["y"],
                                                frame["x"] + frame["width"],
                                                frame["y"] + frame["height"]))

    def _display_rect_from_bounds(self, bounds: tuple[int, int, int, int]) -> tuple[float, float, float, float]:
        return tuple(value * self.zoom for value in bounds)  # type: ignore[return-value]

    def _source_point(self, event: tk.Event) -> tuple[float, float]:
        return (self.canvas.canvasx(event.x) / self.zoom,
                self.canvas.canvasy(event.y) / self.zoom)

    def _begin_selection(self, event: tk.Event) -> None:
        if self.source_image is None:
            return
        self.drag_start = self._source_point(event)
        if self.selection is not None and not (event.state & 0x0001):
            left, top, right, bottom = self.selection
            point_x, point_y = self.drag_start
            edge = self._selection_edge_at(point_x, point_y)
            if edge is not None:
                self.resizing_selection = edge
                self.selection_at_drag_start = self.selection
                self.canvas.focus_set()
                return
            if left <= point_x < right and top <= point_y < bottom:
                self.moving_selection = True
                self.selection_at_drag_start = self.selection
                self.canvas.focus_set()
                return

        self.moving_selection = False
        self.resizing_selection = None
        self.selection_at_drag_start = None
        self.editing_frame = None
        x, y = self.drag_start
        self.selection_item = self.canvas.create_rectangle(
            x * self.zoom, y * self.zoom, x * self.zoom, y * self.zoom,
            outline="#ffffff", width=2, dash=(5, 3), tags=("selection",))

    def _drag_selection(self, event: tk.Event) -> None:
        if self.drag_start is None:
            return
        if self.moving_selection:
            self._move_selection_to(self._source_point(event))
            return
        if self.resizing_selection is not None:
            self._resize_selection_to(self._source_point(event))
            return
        if self.selection_item is None:
            return
        start_x, start_y = self.drag_start
        end_x, end_y = self._source_point(event)
        self.canvas.coords(self.selection_item, start_x * self.zoom, start_y * self.zoom,
                           end_x * self.zoom, end_y * self.zoom)

    def _move_selection_to(self, point: tuple[float, float]) -> None:
        if self.drag_start is None or self.selection_at_drag_start is None:
            return
        original = self.selection_at_drag_start
        width = original[2] - original[0]
        height = original[3] - original[1]
        delta_x = round(point[0] - self.drag_start[0])
        delta_y = round(point[1] - self.drag_start[1])
        left = max(0, min(self.image_size[0] - width, original[0] + delta_x))
        top = max(0, min(self.image_size[1] - height, original[1] + delta_y))
        self.selection = (left, top, left + width, top + height)
        self.selection_info_var.set(
            f"Rectangle image : x={left}, y={top}, largeur={width}, hauteur={height}")
        if self.selection_item is not None:
            x1, y1, x2, y2 = self._display_rect_from_bounds(self.selection)
            self.canvas.coords(self.selection_item, x1, y1, x2, y2)
        self._sync_edited_frame()

    def _selection_edge_at(self, x: float, y: float) -> str | None:
        if self.selection is None:
            return None
        left, top, right, bottom = self.selection
        tolerance = max(2.0, 7.0 / self.zoom)
        near_left, near_right = abs(x - left) <= tolerance, abs(x - right) <= tolerance
        near_top, near_bottom = abs(y - top) <= tolerance, abs(y - bottom) <= tolerance
        inside_x = left - tolerance <= x <= right + tolerance
        inside_y = top - tolerance <= y <= bottom + tolerance
        if near_left and near_top:
            return "nw"
        if near_right and near_top:
            return "ne"
        if near_left and near_bottom:
            return "sw"
        if near_right and near_bottom:
            return "se"
        if near_left and inside_y:
            return "w"
        if near_right and inside_y:
            return "e"
        if near_top and inside_x:
            return "n"
        if near_bottom and inside_x:
            return "s"
        return None

    def _update_selection_cursor(self, event: tk.Event) -> None:
        if self.selection is None:
            self.canvas.configure(cursor="crosshair")
            return
        point_x, point_y = self._source_point(event)
        edge = self._selection_edge_at(point_x, point_y)
        cursors = {
            "nw": "size_nw_se", "se": "size_nw_se",
            "ne": "size_ne_sw", "sw": "size_ne_sw",
            "w": "sb_h_double_arrow", "e": "sb_h_double_arrow",
            "n": "sb_v_double_arrow", "s": "sb_v_double_arrow",
        }
        if edge:
            cursor = cursors[edge]
        elif self.selection[0] <= point_x <= self.selection[2] and self.selection[1] <= point_y <= self.selection[3]:
            cursor = "fleur"
        else:
            cursor = "crosshair"
        self.canvas.configure(cursor=cursor)

    def _resize_selection_to(self, point: tuple[float, float]) -> None:
        if self.selection_at_drag_start is None or self.resizing_selection is None:
            return
        left, top, right, bottom = self.selection_at_drag_start
        x = max(0, min(self.image_size[0], round(point[0])))
        y = max(0, min(self.image_size[1], round(point[1])))
        mode = self.resizing_selection
        if "w" in mode:
            left = min(x, right - 1)
        if "e" in mode:
            right = max(x, left + 1)
        if "n" in mode:
            top = min(y, bottom - 1)
        if "s" in mode:
            bottom = max(y, top + 1)
        self.selection = (left, top, right, bottom)
        self.selection_info_var.set(
            f"Rectangle image : x={left}, y={top}, largeur={right-left}, hauteur={bottom-top}")
        if self.selection_item is not None:
            self.canvas.coords(self.selection_item, *self._display_rect_from_bounds(self.selection))
        self._sync_edited_frame()

    def _sync_edited_frame(self) -> None:
        if self.editing_frame is None or self.selection is None:
            return
        animation_index, frame_index = self.editing_frame
        if animation_index >= len(self.animations):
            return
        animation = self.animations[animation_index]
        if frame_index >= len(animation.frames):
            return
        left, top, right, bottom = self.selection
        animation.frames[frame_index].update({
            "x": left, "y": top, "width": right - left, "height": bottom - top,
        })
        self._refresh_frame_list(frame_index)

    def _finish_selection(self, event: tk.Event) -> None:
        if self.drag_start is None:
            return
        if self.moving_selection:
            self._move_selection_to(self._source_point(event))
            self.drag_start = None
            self.moving_selection = False
            self.selection_at_drag_start = None
            self._render_sheet()
            return
        if self.resizing_selection is not None:
            self._resize_selection_to(self._source_point(event))
            self.drag_start = None
            self.resizing_selection = None
            self.selection_at_drag_start = None
            self._render_sheet()
            return
        start_x, start_y = self.drag_start
        end_x, end_y = self._source_point(event)
        image_width, image_height = self.image_size
        left = max(0, min(image_width, math.floor(min(start_x, end_x))))
        top = max(0, min(image_height, math.floor(min(start_y, end_y))))
        right = max(0, min(image_width, math.ceil(max(start_x, end_x))))
        bottom = max(0, min(image_height, math.ceil(max(start_y, end_y))))
        self.drag_start = None
        if right <= left or bottom <= top:
            self.selection = None
            self.selection_info_var.set("Aucune frame sélectionnée")
            self._render_sheet()
            return
        self.selection = (left, top, right, bottom)
        self.editing_frame = None
        self.selection_info_var.set(
            f"Rectangle image : x={left}, y={top}, largeur={right-left}, hauteur={bottom-top}")
        self._render_sheet()

    def copy_selection(self, event: tk.Event | None = None) -> str | None:
        if event is not None and event.widget.winfo_class() in {
                "Entry", "TEntry", "Text", "TCombobox", "Spinbox", "TSpinbox"}:
            return None
        if self.selection is None:
            return "break" if event is not None else None
        try:
            canvas_width = int(self.canvas_width_var.get())
            canvas_height = int(self.canvas_height_var.get())
            offset_x = int(self.offset_x_var.get())
            offset_y = int(self.offset_y_var.get())
        except ValueError:
            return "break" if event is not None else None
        metadata = {
            "canvasWidth": canvas_width,
            "canvasHeight": canvas_height,
            "offsetX": offset_x,
            "offsetY": offset_y,
        }
        self.clipboard_selection = (self.selection, metadata)
        self.selection_info_var.set(
            f"Sélection copiée : {self.selection[2] - self.selection[0]}×"
            f"{self.selection[3] - self.selection[1]} px")
        return "break" if event is not None else None

    def paste_selection(self, event: tk.Event | None = None) -> str | None:
        if event is not None and event.widget.winfo_class() in {
                "Entry", "TEntry", "Text", "TCombobox", "Spinbox", "TSpinbox"}:
            return None
        if self.clipboard_selection is None or self.source_image is None:
            return "break" if event is not None else None
        copied_bounds, metadata = self.clipboard_selection
        width = copied_bounds[2] - copied_bounds[0]
        height = copied_bounds[3] - copied_bounds[1]
        left = max(0, min(self.image_size[0] - width, copied_bounds[0]))
        top = max(0, min(self.image_size[1] - height, copied_bounds[1]))
        self.selection = (left, top, left + width, top + height)
        self.canvas_width_var.set(str(metadata["canvasWidth"]))
        self.canvas_height_var.set(str(metadata["canvasHeight"]))
        self.offset_x_var.set(str(metadata["offsetX"]))
        self.offset_y_var.set(str(metadata["offsetY"]))
        self.selection_info_var.set(
            f"Sélection collée : x={left}, y={top}, largeur={width}, hauteur={height}. "
            "Glisse-la pour la déplacer.")
        self._render_sheet()
        return "break" if event is not None else None

    def create_animation(self) -> None:
        name = self.animation_name_var.get().strip()
        if not name:
            messagebox.showwarning("Nom requis", "Donne un nom à l’animation.")
            return
        if any(animation.name == name for animation in self.animations):
            messagebox.showwarning("Nom déjà utilisé", "Une animation porte déjà ce nom.")
            return
        try:
            duration = int(self.duration_var.get())
            if duration <= 0:
                raise ValueError
        except ValueError:
            messagebox.showwarning("Durée invalide", "La durée doit être un entier positif en millisecondes.")
            return
        self.animations.append(AnimationDefinition(name, self.loop_var.get(), duration))
        self._refresh_animation_list(len(self.animations) - 1)

    def apply_animation_settings(self) -> None:
        animation = self._selected_animation()
        if animation is None:
            messagebox.showinfo("Aucune animation", "Sélectionne une animation à modifier.")
            return
        new_name = self.animation_name_var.get().strip()
        if not new_name:
            messagebox.showwarning("Nom requis", "Le nom ne peut pas être vide.")
            return
        if any(item is not animation and item.name == new_name for item in self.animations):
            messagebox.showwarning("Nom déjà utilisé", "Une animation porte déjà ce nom.")
            return
        try:
            duration = int(self.duration_var.get())
            if duration <= 0:
                raise ValueError
        except ValueError:
            messagebox.showwarning("Durée invalide", "La durée doit être un entier positif en millisecondes.")
            return
        animation.name = new_name
        animation.loop = self.loop_var.get()
        animation.frame_duration_ms = duration
        self._refresh_animation_list(self.animations.index(animation))
        self._refresh_frame_list()
        self._render_sheet()

    def delete_animation(self) -> None:
        index = self._selected_animation_index()
        if index is None:
            return
        del self.animations[index]
        self._refresh_animation_list(min(index, len(self.animations) - 1))
        self._refresh_frame_list()
        self._render_sheet()

    def _selected_animation_index(self) -> int | None:
        selected = self.animation_list.curselection()
        return int(selected[0]) if selected else None

    def _selected_animation(self) -> AnimationDefinition | None:
        index = self._selected_animation_index()
        return self.animations[index] if index is not None else None

    def _select_animation(self, _event: tk.Event | None = None) -> None:
        animation = self._selected_animation()
        if animation is None:
            self._refresh_frame_list()
            return
        self.animation_name_var.set(animation.name)
        self.duration_var.set(str(animation.frame_duration_ms))
        self.loop_var.set(animation.loop)
        self._refresh_frame_list()
        self._render_sheet()

    def _refresh_animation_list(self, select_index: int | None = None) -> None:
        self.animation_list.delete(0, tk.END)
        for animation in self.animations:
            self.animation_list.insert(tk.END, f"{animation.name} ({len(animation.frames)})")
        if select_index is not None and self.animations:
            select_index = max(0, min(select_index, len(self.animations) - 1))
            self.animation_list.selection_set(select_index)
            self.animation_list.activate(select_index)
            self._select_animation()

    def _refresh_frame_list(self, select_index: int | None = None) -> None:
        self.frame_list.delete(0, tk.END)
        animation = self._selected_animation()
        if animation is None:
            return
        for index, frame in enumerate(animation.frames):
            self.frame_list.insert(tk.END,
                f"{index}: ({frame['x']}, {frame['y']}) {frame['width']}×{frame['height']}"
                f"  offset=({frame['offsetX']}, {frame['offsetY']})")
        if select_index is not None and animation.frames:
            select_index = max(0, min(select_index, len(animation.frames) - 1))
            self.frame_list.selection_set(select_index)
            self.frame_list.activate(select_index)

    def _select_frame(self, _event: tk.Event | None = None) -> None:
        animation = self._selected_animation()
        selected = self.frame_list.curselection()
        if animation is None or not selected:
            return
        frame_index = int(selected[0])
        frame = animation.frames[frame_index]
        self.editing_frame = (self.animations.index(animation), frame_index)
        self.canvas_width_var.set(str(frame["canvasWidth"]))
        self.canvas_height_var.set(str(frame["canvasHeight"]))
        self.offset_x_var.set(str(frame["offsetX"]))
        self.offset_y_var.set(str(frame["offsetY"]))
        self.selection = (frame["x"], frame["y"], frame["x"] + frame["width"],
                          frame["y"] + frame["height"])
        self.selection_info_var.set(
            f"Rectangle image : x={frame['x']}, y={frame['y']}, "
            f"largeur={frame['width']}, hauteur={frame['height']}")
        self._render_sheet()

    def add_selected_frame(self) -> None:
        animation = self._selected_animation()
        if animation is None:
            messagebox.showinfo("Aucune animation", "Crée ou sélectionne une animation avant d’ajouter une frame.")
            return
        if self.selection is None:
            messagebox.showinfo("Aucune sélection", "Dessine d’abord un rectangle sur la spritesheet.")
            return
        try:
            canvas_width = int(self.canvas_width_var.get())
            canvas_height = int(self.canvas_height_var.get())
            offset_x = int(self.offset_x_var.get())
            offset_y = int(self.offset_y_var.get())
            if min(canvas_width, canvas_height) <= 0 or min(offset_x, offset_y) < 0:
                raise ValueError
        except ValueError:
            messagebox.showwarning("Canevas invalide", "Les tailles doivent être positives et les décalages positifs ou nuls.")
            return

        left, top, right, bottom = self.selection
        width, height = right - left, bottom - top
        if offset_x + width > canvas_width or offset_y + height > canvas_height:
            messagebox.showwarning("Frame hors canevas", "Le rectangle avec son décalage doit tenir dans le canevas logique.")
            return
        animation.frames.append({
            "x": left, "y": top, "width": width, "height": height,
            "offsetX": offset_x, "offsetY": offset_y,
            "canvasWidth": canvas_width, "canvasHeight": canvas_height,
        })
        self._refresh_animation_list(self.animations.index(animation))
        self._refresh_frame_list(len(animation.frames) - 1)
        self._render_sheet()

    def delete_frame(self) -> None:
        animation = self._selected_animation()
        selected = self.frame_list.curselection()
        if animation is None or not selected:
            return
        index = int(selected[0])
        del animation.frames[index]
        self._refresh_animation_list(self.animations.index(animation))
        self._refresh_frame_list(min(index, len(animation.frames) - 1))
        self._render_sheet()

    def move_frame(self, direction: int) -> None:
        animation = self._selected_animation()
        selected = self.frame_list.curselection()
        if animation is None or not selected:
            return
        index = int(selected[0])
        destination = index + direction
        if not 0 <= destination < len(animation.frames):
            return
        animation.frames[index], animation.frames[destination] = (
            animation.frames[destination], animation.frames[index])
        self._refresh_animation_list(self.animations.index(animation))
        self._refresh_frame_list(destination)
        self._render_sheet()

    def export_json(self) -> None:
        if self.source_image is None or not self.animations:
            messagebox.showwarning("Rien à exporter", "Charge une spritesheet et crée au moins une animation.")
            return
        if any(not animation.frames for animation in self.animations):
            messagebox.showwarning("Frames manquantes", "Chaque animation doit contenir au moins une frame.")
            return
        sprite_path = self.sprite_path_var.get().strip().replace("\\", "/")
        if not sprite_path:
            messagebox.showwarning("spritePath requis", "Indique le chemin de la spritesheet utilisé par le jeu.")
            return

        output = filedialog.asksaveasfilename(
            title="Exporter les animations",
            defaultextension=".json",
            filetypes=(("Fichier JSON", "*.json"),),
            initialfile="animations.json",
        )
        if not output:
            return
        data = {
            "spritePath": sprite_path,
            "animations": [
                {
                    "name": animation.name,
                    "loop": animation.loop,
                    "frameDurationMs": animation.frame_duration_ms,
                    "frames": animation.frames,
                }
                for animation in self.animations
            ],
        }
        try:
            with open(output, "w", encoding="utf-8", newline="\n") as json_file:
                json.dump(data, json_file, indent=2, ensure_ascii=False)
                json_file.write("\n")
        except OSError as error:
            messagebox.showerror("Export impossible", str(error))
            return
        messagebox.showinfo("Export terminé", f"Animations exportées dans :\n{output}")


def main() -> None:
    root = tk.Tk()
    SpritesheetEditor(root)
    root.mainloop()


if __name__ == "__main__":
    main()
