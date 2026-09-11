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

Le fichier `GameEngine/GameEngine.vcxproj` utilise actuellement les chemins SFML suivants :

```text
E:\Programmation\Tools\SFML-2.6.0\include
E:\Programmation\Tools\SFML-2.6.0\lib
```

Si SFML est installee ailleurs, adaptez les proprietes **Additional Include Directories** et **Additional Library Directories** du projet, ainsi que les dependances de l'editeur de liens.

### Compilation avec Visual Studio

Ouvrez `GameEngine.sln`, selectionnez `Release` et `x64` (ou `Win32`), puis choisissez **Build > Build Solution**.

La compilation peut aussi etre lancee depuis un **Developer Command Prompt for VS** :

```bat
msbuild GameEngine.sln /p:Configuration=Release /p:Platform=x64
```

Les fichiers generes par Visual Studio sont places dans les dossiers de sortie de la solution, par exemple `x64\Release\`.