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

### EntityManager

`EntityManager` est responsable du cycle de vie et du cycle de jeu des entités.
Lorsqu'une entité est ajoutée avec `addEntity`, elle est automatiquement :

- conservée par le moteur ;
- mise à jour à chaque frame ;
- rendue si elle hérite de `Entity2D` ;
- incluse dans les collisions si `useCollision` vaut `true`.

Un niveau n'a donc pas besoin de rendre chaque entité manuellement ni de fournir
une liste d'obstacles au joueur.

#### `addEntity`

La fonction est variadique : ses paramètres sont transmis directement au
constructeur du type demandé.

```cpp
template <typename EntityType, typename... Arguments>
static EntityType* addEntity(Arguments&&... arguments);
```

Le retour est un pointeur vers l'entité créée. Le moteur en reste propriétaire ;
il ne faut donc pas appeler `delete` dessus.

Exemple avec une `Entity2D` :

```cpp
Entity2D* barrel = EntityManager::addEntity<Entity2D>(
    "barrel",                 // entityName
    "assets/sprites/barrel.png", // spriteName
    64.0,                      // largeur
    64.0,                      // hauteur
    150.0f,                    // position X initiale
    groundY,                   // position Y initiale
    true                       // utilise les collisions
);
```

Exemple avec un `Player` :

```cpp
Player* player = EntityManager::addEntity<Player>(
    "player", playerSpritePath,
    64.0, 64.0,
    25.0f, groundY,
    true
);
```

#### Constructeurs

Les constructeurs de base sont les suivants :

```cpp
Entity(bool is2D, string entityName,
    float x = 0.0f, float y = 0.0f);

Entity(string entityName,
    float x = 0.0f, float y = 0.0f);

Entity2D(string entityName, string spriteName,
      double width = ..., double height = ...,
      float x = 0.0f, float y = 0.0f,
      bool useCollision = false);

Player(string entityName, string spriteName,
    double width = 32.0, double height = 32.0,
    float x = 0.0f, float y = 0.0f,
    bool useCollision = true);
```

Pour une entité 2D, les deux derniers paramètres permettent donc de définir sa
position initiale et d'activer ou non sa participation aux collisions.

#### Rechercher et supprimer une entité

Chaque entité possède un identifiant unique dans `entityId` :

```cpp
int playerId = player->entityId;
Player* samePlayer = EntityManager::getEntity<Player>(playerId);
Entity* anyEntity = EntityManager::getEntity(playerId);
```

Le manager fournit également `destroyEntity(entity)`,
`destroyEntity(entityId)` et `destroyAllExcept(entitiesToKeep)`.

#### Rendu automatique

Le moteur appelle `EntityManager::renderAllEntities(...)`. Une entité héritant
de `Entity2D` dessine automatiquement son sprite. Il n'est donc pas nécessaire
d'écrire ceci dans chaque niveau :

```cpp
entity->render(target);
```

### Entity2D

`Entity2D` représente une entité visible avec une taille utilisée pour ses
limites de collision.

#### Position

La position initiale peut être fournie au constructeur. Elle peut ensuite être
modifiée avec `setPosition` :

```cpp
entity->setPosition(220.0f, groundY);
```

Cette fonction met à jour à la fois la position logique de l'entité et la
position de son sprite. La position courante est accessible avec
`entity->position`.

La vitesse peut être modifiée avec :

```cpp
entity->setVelocity(200.0f, 0.0f);
```

#### Activer les collisions

Le paramètre `useCollision` du constructeur active ou désactive la participation
de l'entité au système de collision. Il peut aussi être modifié après création :

```cpp
entity->setUseCollision(true);
bool collides = entity->usesCollision();
```

Une entité avec `useCollision = false` reste rendue et mise à jour, mais elle est
ignorée par la détection et la résolution des collisions.

### Collision2D

Le système de collision 2D est géré par `EntityManager` et implémenté dans
`Core/Collider2D`. Il utilise des boîtes englobantes rectangulaires (AABB)
basées sur la position et la taille de chaque `Entity2D`.

À chaque frame, le moteur :

1. met à jour toutes les entités ;
2. détecte les intersections entre les entités 2D utilisant les collisions ;
3. résout la position et la vitesse de l'entité mobile ;
4. déclenche les callbacks de collision.

`Collider2D::resolve` compare une entité mobile avec une entité statique. La
position de l'entité mobile avant son déplacement est conservée pour identifier
le côté de la collision :

- une collision par le haut pose l'entité mobile sur l'obstacle et remet sa
    vitesse verticale à zéro ;
- une collision par le bas arrête également le déplacement vertical ;
- une collision latérale replace l'entité contre le bord de l'obstacle et remet
    sa vitesse horizontale à zéro.

La méthode retourne un `CollisionResult` contenant :

- `collided` : indique si une collision a été résolue ;
- `grounded` : indique si l'entité mobile est posée sur le dessus de l'obstacle.

La résolution est faite automatiquement par `EntityManager`. Le joueur reçoit
également l'information `grounded`, ce qui lui permet de sauter depuis le sol ou
depuis le dessus d'un obstacle sans configuration supplémentaire dans le niveau.

Toute entité avec `useCollision = true` participe actuellement à la détection
et à la résolution physique. Si elle doit seulement recevoir un événement sans
bloquer le joueur, il faudra prévoir un réglage séparé de type trigger ; ce mode
n'est pas encore exposé par l'API actuelle.

### Événements `OnCollision`

Une entité 2D peut réagir à une collision en surchargeant les méthodes virtuelles
suivantes :

```cpp
virtual void OnCollision(const CollisionInfo& collision);
virtual void OnCollisionEnter(const Entity2D& other);
virtual void OnCollisionStay(const Entity2D& other);
virtual void OnCollisionExit(const Entity2D& other);
```

La méthode générale `OnCollision` reçoit un objet `CollisionInfo` contenant :

```cpp
struct CollisionInfo
{
    enum class State { Enter, Stay, Exit };
    State state;
    const Entity2D& other;
};
```

Les états ont la signification suivante :

- `CollisionInfo::State::Enter` : premier frame de contact ;
- `CollisionInfo::State::Stay` : les deux entités restent en contact ;
- `CollisionInfo::State::Exit` : le contact vient de se terminer.

#### Surcharger `OnCollision`

```cpp
class Portal : public Entity2D
{
public:
    using Entity2D::Entity2D;

    void OnCollision(const CollisionInfo& collision) override
    {
        if (collision.state == CollisionInfo::State::Enter)
            std::cout << "Une entite est entree dans le portail" << std::endl;

        if (collision.state == CollisionInfo::State::Exit)
            std::cout << "Une entite est sortie du portail" << std::endl;
    }
};
```

Il n'est pas nécessaire de transmettre un callback au constructeur. Le callback
appartient à l'objet, comme dans un moteur de jeu classique, et la détection est
effectuée par le moteur.

#### Identifier l'entité en collision

`collision.other` est une référence vers l'autre entité. Pour vérifier son type,
il est possible d'utiliser `dynamic_cast` :

```cpp
void Portal::OnCollision(const CollisionInfo& collision)
{
    const Player* player = dynamic_cast<const Player*>(&collision.other);
    if (player == nullptr)
        return;

    if (collision.state == CollisionInfo::State::Enter)
        std::cout << "Le Player touche le portail" << std::endl;
}
```

La référence est constante : elle permet d'inspecter l'autre entité, mais pas de
la modifier directement dans le callback.

#### Installer un callback avec `setOnCollision`

Lorsqu'une surcharge de classe n'est pas nécessaire, un callback peut être
enregistré avec `setOnCollision` :

```cpp
barrel->setOnCollision([](const CollisionInfo& collision)
{
    if (collision.state == CollisionInfo::State::Enter)
        std::cout << "Le barrel vient d'entrer en collision" << std::endl;

    if (dynamic_cast<const Player*>(&collision.other) != nullptr)
        std::cout << "Le barrel touche le Player" << std::endl;
});
```

`setOnCollision` reçoit une fonction de type :

```cpp
std::function<void(const CollisionInfo&)>;
```

Les méthodes `OnCollision`, `OnCollisionEnter`, `OnCollisionStay` et
`OnCollisionExit` sont appelées par le moteur. `setOnCollision` ajoute un
callback fonctionnel pratique pour les cas simples.

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