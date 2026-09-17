# CPP-Game-Engine

## Structure du projet

Le projet est séparé en deux grandes parties :

- `GameEngine/Core` : contient le moteur et les composants réutilisables. C'est la base technique du moteur : entités, composants, `Player`, logique de jeu, gestion des capacités, fenêtre, etc.
- `GameEngine/Game` : contient une démo de jeu concrète, qui sert d'exemple d'utilisation du moteur. Ici, on trouve les éléments liés au gameplay : `Jump`, `Attack`, `main.cpp`, ainsi que les assets.

Cette séparation permet d'avoir un moteur générique qui reste réutilisable, tout en gardant un exemple de jeu fonctionnel dans le dossier `Game`. Ce dossier peut être remplacé, supprimé ou modifié selon le projet final : il s'agit d'une démo de démonstration, pas d'une couche obligatoire du moteur.

### Exemple de structure

```text
GameEngine/
├── Core/
│   ├── EntityManager.cpp
│   ├── EntityManager.h
│   ├── Game.cpp
│   ├── Game.h
│   ├── Player.cpp
│   ├── Player.h
│   ├── Ability.cpp
│   ├── Ability.h
│   └── ...
├── Game/
│   ├── main.cpp
│   ├── Jump.cpp
│   ├── Jump.h
│   ├── Attack.cpp
│   ├── Attack.h
│   └── assets/
└── ...
```

### Rôle de chaque partie

- `Core` : moteur principal, abstractions communes, système d'entités et logique de base.
- `Game` : démonstration d'utilisation du moteur, scripts, entités spécifiques au prototype, interactions jouables et assets de test.

Les fichiers du dossier `Game` peuvent être retirés ou remplacés si vous souhaitez construire un autre jeu avec la même base technique.

## Linux

### Dependances

Installez GCC, `pkg-config` et les bibliotheques de developpement SFML :

```bash
sudo apt update
sudo apt install build-essential pkg-config libsfml-dev
```

### Compilation

#### CMake
Depuis la racine du projet, compilez l'application dans `build/` :
```bash
cmake -S . -B build/linux -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux --parallel
```
L'executable se trouve ensuite dans `build/linux/bin/GameEngine`.

#### GCC
Depuis la racine du projet, compilez l'application dans `dist/` :

```bash
mkdir -p dist
g++ -std=c++17 -Wall -Wextra -pedantic GameEngine/Core/*.cpp GameEngine/Game/*.cpp \
	-o dist/game-engine \
	$(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system)
mkdir -p dist/assets
cp -R GameEngine/Game/assets/. dist/assets/
```

L'executable se trouve ensuite dans `dist/game-engine`.

### Chemin des assets

Le chemin du sprite est fourni directement au programme. Il peut pointer vers
n'importe quel dossier du projet ou vers un chemin absolu :

```bash
./dist/game-engine GameEngine/Game/assets/player.png
./dist/game-engine TheGame/assets/player.png
./dist/game-engine /chemin/vers/MyGame/images/player.png
```

Si aucun chemin n'est fourni, CMake et la procédure GCC copient le dossier
d'assets à côté de l'exécutable et la démo charge `assets/player.png`. Le
lancement fonctionne donc depuis n'importe quel dossier :

```bash
./build/linux/bin/GameEngine
```

Pour utiliser un autre dossier d'assets avec CMake, définissez
`GAME_ASSETS_DIR` lors de la configuration :

```bash
cmake -S . -B build/linux \
    -DGAME_ASSETS_DIR="$PWD/TheGame/assets" \
    -DCMAKE_BUILD_TYPE=Release
```

## Windows

### Dependances

Installez les composants suivants :

- Visual Studio 2019 ou une version plus recente, avec la charge de travail **Developpement Desktop en C++** ;
- SFML 2.6.0, en version correspondant a l'architecture choisie (`Win32` ou `x64`).

#### Installation SFML Manuelle
1. Téléchargez les fichiers compilé SFML
2. À la racine du projet créer un fichier `CMakeUserPresets.json` et y mettre le contenu suivant (En changeant CHEMIN_VERS_SFML)
```cmake
{
    "version": 6,
    "include": [
        "CMakePresets.json"
    ],
    "configurePresets": [
        {
            "name": "windows-msvc",
            "displayName": "Windows - MSVC + SFML",
            "inherits": "windows",
            "cacheVariables": {
                "SFML_DIR": "CHEMIN_VERS_SFML/lib/cmake/SFML"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "windows-release-user",
            "configurePreset": "windows-msvc",
            "configuration": "Release"
        },
        {
            "name": "windows-debug-user",
            "configurePreset": "windows-msvc",
            "configuration": "Debug"
        }
    ]
}
```

### Compilation avec Visual Studio

Ouvrez `Developper PowerShell for VS`, placez-vous dans le dossier root du projet

1. Effectuez la commande suivante : ```cmake --list-presets```

Vous devriez obtenir quelque chose comme:
```text
Available configure presets:

  "windows"
  "windows-msvc"
```
2. Effectuez la commande suivante : ```cmake --preset windows-msvc```

Vous devriez obtenir quelque chose comme:
```text
-- Selecting Windows SDK version ...
-- The CXX compiler identification is MSVC ...
-- Found SFML ...
-- Configuring done
-- Generating done
```
3. Effectuez la commande suivante : ```cmake --build --preset windows-release-user```

## Core

### Collision2D

Le système de collision 2D est implémenté dans `Core/Collider2D`. Il utilise des
boîtes englobantes rectangulaires (AABB) basées sur la position et la taille de
chaque `Entity2D`.

La méthode `Collider2D::resolve` compare une entité mobile avec une entité
statique. La position de l'entité mobile avant son déplacement est conservée
pour identifier le côté de la collision :

- une collision par le haut pose l'entité mobile sur l'obstacle et remet sa
    vitesse verticale à zéro ;
- une collision par le bas arrête également le déplacement vertical ;
- une collision latérale replace l'entité contre le bord de l'obstacle et remet
    sa vitesse horizontale à zéro.

La méthode retourne un `CollisionResult` indiquant si une collision a eu lieu
et si elle correspond à un contact avec le dessus d'un obstacle (`grounded`).
Le niveau fournit ses obstacles au joueur pendant sa mise à jour. Cela permet,
par exemple, de traiter un barril comme une plateforme sur laquelle le joueur
peut atterrir, ou comme un obstacle qu'il doit contourner ou franchir.

### Player

Le `Player` possède une liste d'`Ability`. Chaque capacité contient un identifiant,
un nom d'affichage, un contrôle et une fonction de rappel. Une capacité peut
être ajoutée ou supprimée avec `addAbility` et `removeAbility`.

Pendant chaque mise à jour du joueur :

- une capacité continue est activée tant que son contrôle est maintenu ;
- une capacité non continue est activée une seule fois au moment où son contrôle
    passe de relâché à pressé ;
- la fonction de rappel de la capacité exécute l'action associée, comme le
    déplacement, le saut ou l'attaque.

L'état `grounded` indique si le joueur est posé sur le sol de la fenêtre ou sur
le dessus d'un obstacle. Il est remis à zéro au début de chaque mise à jour,
puis réactivé lorsqu'une collision verticale descendante est détectée ou lorsque
le joueur atteint le sol. La capacité de saut vérifie cet état avant d'appliquer
une vitesse verticale négative : le joueur ne peut donc pas effectuer de saut
en l'air, mais peut sauter depuis le sol ou depuis le dessus du barril.