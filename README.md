# CPP-Game-Engine

## Linux

### Dependances

Installez GCC, `pkg-config` et les bibliotheques de developpement SFML :

```bash
sudo apt update
sudo apt install build-essential pkg-config libsfml-dev
```

### Compilation

Depuis la racine du projet, compilez l'application dans `dist/` :

```bash
mkdir -p dist
g++ -std=c++17 -Wall -Wextra -pedantic GameEngine/Core/*.cpp GameEngine/Game/*.cpp \
	-o dist/game-engine \
	$(pkg-config --cflags --libs sfml-graphics sfml-window sfml-system)
```

L'executable se trouve ensuite dans `dist/game-engine`.

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