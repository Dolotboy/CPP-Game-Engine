# CPP-Game-Engine

## Structure du projet

Le projet est séparé en deux grandes parties :

- `GameEngine/Core` : contient le moteur et les composants réutilisables. C'est la base technique du moteur : entités, composants, logique de jeu, gestion des capacités, fenêtre, etc.
- `GameEngine/Game` : contient une démo de jeu concrète, qui sert d'exemple d'utilisation du moteur. Ici, on trouve les éléments liés au gameplay : `Player`, `Jump`, `Attack`, `main.cpp`, ainsi que les assets.

Cette séparation permet d'avoir un moteur générique qui reste réutilisable, tout en gardant un exemple de jeu fonctionnel dans le dossier `Game`. Ce dossier peut être remplacé, supprimé ou modifié selon le projet final : il s'agit d'une démo de démonstration, pas d'une couche obligatoire du moteur.

### Exemple de structure

```text
GameEngine/
├── Core/
│   ├── EntityManager.cpp
│   ├── EntityManager.h
│   ├── Game.cpp
│   ├── Game.h
│   ├── Ability.cpp
│   ├── Ability.h
│   └── ...
├── Game/
│   ├── main.cpp
│   ├── Player.cpp
│   ├── Player.h
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