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
│   ├── Animation.*
│   ├── Collider2D.*
│   ├── Entity.*
│   ├── Entity2D.*
│   ├── EntityManager.*
│   ├── Game.*
│   └── ...
├── Game/
│   ├── main.cpp
│   ├── Abilities/
│   ├── Animations/
│   ├── Levels/
│   ├── Portal.*
│   └── assets/
├── tools/
│   └── spritesheet_editor.py
└── ...
```

### Rôle de chaque partie

- `Core` : moteur principal, abstractions communes, système d'entités et logique de base.
- `Game` : démonstration d'utilisation du moteur, scripts, entités spécifiques au prototype, interactions jouables et assets de test.

Les fichiers du dossier `Game` peuvent être retirés ou remplacés si vous souhaitez construire un autre jeu avec la même base technique.

## Linux

### Dependances

#### Installer CMake
1. ```sudo apt update```
2. ```sudo apt install cmake```

#### Installez GCC, `pkg-config` et les bibliotheques de developpement SFML :
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


### Assets et lancement

La démo utilise des chemins relatifs tels que `assets/sprites/...` et
`Animations/player_automatic.json`. CMake copie les dossiers `assets` et
`Animations` à côté de l'exécutable. Lance le jeu depuis ce dossier afin que ces
chemins relatifs soient résolus correctement :

```bash
cd build/linux/bin
./GameEngine
```

Pour substituer les dossiers d'assets ou d'animations avec CMake, définis
`GAME_ASSETS_DIR` et `GAME_ANIMATIONS_DIR` lors de la configuration :

```bash
cmake -S . -B build/linux \
    -DGAME_ASSETS_DIR="$PWD/TheGame/assets" \
    -DGAME_ANIMATIONS_DIR="$PWD/TheGame/Animations" \
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

### Animation

`Animation` gère la lecture d'une suite d'images ou d'une animation découpée
dans une spritesheet. `Entity` possède une animation courante dans son membre
`animation`; `Entity::update(deltaTime)` fait avancer cette animation. Une
`Entity2D` applique ensuite la texture et le rectangle de la frame courante à
son sprite. Pour les frames recadrées, le canevas logique est redimensionné à la
taille de l'entité et les décalages gardent les frames alignées.

#### Démarrer une animation directement

Une animation peut être configurée et démarrée à partir d'une liste de fichiers
image :

```cpp
entity.animation.start({
    "assets/animations/jump_01.png",
    "assets/animations/jump_02.png",
    "assets/animations/jump_03.png"
}, 0.12f, false); // durée d'une frame en secondes, sans boucle
```

Pour une spritesheet régulière, `columns` et `rows` indiquent le nombre de
colonnes et de lignes de frames dans toute l'image :

```cpp
entity.animation.startSpriteSheet(
    "assets/sprites/effect.png",
    4, 2,       // 4 colonnes par 2 lignes
    0.12f,      // durée d'une frame en secondes
    true        // boucle
);
```

`start(...)` configure puis démarre l'animation. `configure(...)` et
`configureSpriteSheet(...)` permettent de préparer l'animation sans la lancer;
`animation.start()` démarre alors à la première frame. Les contrôles disponibles
sont `stop()` (arrête et revient à la première frame), `pause()` et `resume()`.
`isConfigured()`, `isPlaying()` et `isFinished()` permettent de consulter son
état. Une animation non bouclée conserve sa dernière frame après sa fin; elle
reste affichée jusqu'au démarrage d'une autre animation. `startAnimation` ne
redémarre pas une animation de même nom si elle est déjà en cours ou terminée.

#### Enregistrer des animations dans une entité

Pour définir plusieurs animations d'une même spritesheet, l'entité peut charger
un fichier JSON avec `registerAnimation(path)`, puis en démarrer une par son nom
avec `startAnimation(name)`. Ces fonctions renvoient `false` si le chargement ou
la recherche du nom échoue.

Un `Player` accepte le chemin du JSON comme dernier paramètre de son constructeur.
À l'initialisation, il enregistre les définitions puis démarre `Idle_Front` :

```cpp
Player* player = EntityManager::addEntity<Player>(
    "player",
    "assets/sprites/player_spritesheet.png",
    64.0, 64.0,
    25.0f, groundY,
    true,                       // collisions
    "Animations/player.json"   // définitions d'animations
);

player->startAnimation("Jump_Left");
```

Le JSON contient le chemin de la spritesheet et les définitions d'animations. Le
chemin `spritePath` est résolu depuis le dossier parent du dossier contenant le
JSON; avec `Animations/player.json`, le chemin ci-dessous pointe donc vers
`assets/sprites/player_spritesheet.png` :

```json
{
  "spritePath": "assets/sprites/player_spritesheet.png",
  "columns": 13,
  "rows": 54,
  "animations": [
    {
      "name": "Idle_Front",
      "row": 24,
      "columns": 2,
      "reverse": false,
      "loop": true,
      "frameDurationMs": 400
    }
  ]
}
```

Dans ce format en grille, `columns` et `rows` à la racine décrivent le nombre de
cases de la spritesheet. Dans chaque animation, `row` est l'indice de la ligne
(à partir de zéro) et `columns` le nombre de frames consécutives à lire depuis la
première colonne. `reverse: true` inverse leur ordre. `loop` indique si la
lecture recommence après la dernière frame et `frameDurationMs` donne la durée
d'une frame en millisecondes. Leurs valeurs par défaut sont respectivement
`true` et `100`.

Ce format en grille reste pratique pour les spritesheets régulières. Il n'est
pas obligatoire : une animation peut définir ses rectangles de frame directement
en pixels. La liste peut suivre n'importe quel ordre dans l'image, et ses frames
peuvent venir de lignes ou de colonnes différentes :

```json
{
  "spritePath": "assets/sprites/characters.png",
  "animations": [
    {
      "name": "Attack",
      "loop": false,
      "frameDurationMs": 80,
      "frames": [
        { "x": 12, "y": 8, "width": 40, "height": 54,
          "offsetX": 12, "offsetY": 10,
          "canvasWidth": 64, "canvasHeight": 64 },
        { "x": 80, "y": 4, "width": 52, "height": 60,
          "offsetX": 6, "offsetY": 4,
          "canvasWidth": 64, "canvasHeight": 64 }
      ]
    }
  ]
}
```

Chaque objet de `frames` décrit une image de l'animation :

- `x`, `y` : coordonnées du coin supérieur gauche du rectangle dans la
  spritesheet, en pixels, depuis son coin supérieur gauche ;
- `width`, `height` : largeur et hauteur du rectangle recadré, en pixels ;
- `canvasWidth`, `canvasHeight` : dimensions du canevas logique de cette frame,
  avant redimensionnement à la taille de l'entité. Utilise les mêmes valeurs
  pour toutes les frames si tu veux conserver leur échelle relative. Si chaque
  canevas a la taille du rectangle recadré, celui-ci remplit l'entité ;
- `offsetX`, `offsetY` : position du coin supérieur gauche du rectangle recadré
  sur le canevas logique, en pixels. Le décalage peut différer d'une frame à
  l'autre pour garder le personnage aligné.

Les objets de la liste `frames` sont lus dans leur ordre d'apparition dans le
JSON; cet ordre définit donc la séquence de lecture de l'animation.

Les champs de canevas et de décalage sont facultatifs. Sans eux, le moteur
utilise le rectangle recadré comme canevas entier, avec un décalage nul. Les
valeurs doivent être cohérentes : les dimensions du canevas sont positives, les
décalages sont positifs ou nuls, et le rectangle placé avec son décalage doit
tenir dans le canevas. Le moteur adapte séparément l'échelle horizontale et
verticale à la taille de l'entité. Un canevas de même taille que le rectangle
recadré l'étire donc jusqu'à remplir l'entité; si leurs proportions diffèrent,
l'image peut être déformée. `loop` et `frameDurationMs` fonctionnent comme dans
le format en grille. Une animation avec `loop: false` s'arrête sur sa dernière
frame et la garde affichée jusqu'à ce qu'une autre animation démarre.

Lorsque les animations utilisent `frames`, `columns` et `rows` à la racine ne
sont pas nécessaires. Le système accepte ainsi les frames placées à n'importe
quel endroit de la texture, dans n'importe quel ordre, sans imposer une
orientation ou un découpage régulier.

Chaque animation enregistrée partage la texture de la spritesheet. Le choix de
l'animation ne recharge donc pas le fichier image. Pour remplacer l'animation
courante, il suffit d'appeler `startAnimation` avec un autre nom.

Une frame déjà enregistrée peut aussi servir de sprite par défaut, par exemple
pour afficher le baril intact depuis la première frame de son animation de
destruction :

```cpp
if (!barrel->registerAnimation("Animations/barrel.json"))
    std::cerr << "Impossible de charger les animations du baril\n";

if (!barrel->setSprite(barrel->getAnimationFrame("Destroy", 0)))
    std::cerr << "Frame neutre du baril introuvable\n";
```

`getAnimationFrame(name, index)` renvoie une frame si le nom et l'indice
existent. `setSprite` accepte ce résultat directement et renvoie `false` si la
frame est absente. Le sprite par défaut conserve la texture, le rectangle, le
canevas logique et les décalages de la frame. Il s'affiche lorsqu'aucune
animation n'a démarré, ou quand l'animation est arrêtée explicitement. Une
animation démarrée prend sa place; si elle se termine naturellement sans boucle,
sa dernière frame reste affichée jusqu'au démarrage d'une autre animation.

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
    float x = 0.0f, float y = 0.0f, float z = 0.0f);

Entity(string entityName,
    float x = 0.0f, float y = 0.0f, float z = 0.0f);

Entity2D(string entityName, string spriteName,
      double width = ..., double height = ...,
      float x = 0.0f, float y = 0.0f,
      bool useCollision = false);

Entity2D(string entityName, string spriteName,
      double width, double height,
      float x, float y, float z,
      bool useCollision = false);

Player(string entityName, string spriteName,
    double width = 32.0, double height = 32.0,
    float x = 0.0f, float y = 0.0f,
    bool useCollision = true,
    const std::string& animationPath = "");

Player(string entityName, string spriteName,
    double width, double height,
    float x, float y, float z);

Player(string entityName, string spriteName,
    double width, double height,
    float x, float y, float z,
    bool useCollision, const std::string& animationPath);
```

Pour une entité 2D, les deux derniers paramètres permettent donc de définir sa
position initiale et d'activer ou non sa participation aux collisions.
Les surcharges avec `z` acceptent la profondeur après `x` et `y`, avant
`useCollision`; les anciennes signatures restent valides. Toutes les entités
commencent avec `z = 0` si aucune profondeur n'est donnée. Au rendu,
`EntityManager` dessine les petites valeurs d'abord et les grandes ensuite : une
valeur Z plus élevée place donc l'entité visuellement devant. Les entités de
même profondeur conservent leur ordre de création.

#### Rechercher et supprimer une entité

Chaque entité possède un identifiant unique dans `entityId` :

```cpp
int playerId = player->entityId;
Player* samePlayer = EntityManager::getEntity<Player>(playerId);
Entity* anyEntity = EntityManager::getEntity(playerId);
```

Le manager fournit également `destroyEntity(entity)`,
`destroyEntity(entityId)` et `destroyAllExcept(entitiesToKeep)`.

#### Entités persistantes entre les niveaux

Pour conserver une entité comme avec Unity `DontDestroyOnLoad`, il suffit de
l'ajouter à la liste du moteur :

```cpp
EntityManager::dontDestroyOnLoad(player);
EntityManager::dontDestroyOnLoad(barrel);
```

Lors d'un changement de niveau, ces entités ne sont pas détruites, restent dans
`EntityManager`, continuent d'être mises à jour et sont automatiquement rendues
dans le niveau suivant. Il ne faut pas les recréer ni leur réattribuer leurs
capacités.

Par défaut, leur position actuelle est conservée :

```cpp
EntityManager::dontDestroyOnLoad(player);
```

Une nouvelle position peut être demandée pour le prochain niveau :

```cpp
EntityManager::dontDestroyOnLoad(player, sf::Vector2f(100.0f, 200.0f));
```

Pour les replacer en `(0, 0)` :

```cpp
EntityManager::dontDestroyOnLoad(player, sf::Vector2f(0.0f, 0.0f));
```

La liste complète est accessible avec
`EntityManager::getDontDestroyOnLoadEntities()`. Une entité peut en être retirée
avec `EntityManager::removeDontDestroyOnLoad(entity)`.

#### Rendu automatique

Le moteur appelle `EntityManager::renderAllEntities(...)`. Une entité héritant
de `Entity2D` dessine automatiquement son sprite. Il n'est donc pas nécessaire
d'écrire ceci dans chaque niveau :

```cpp
entity->render(target);
```

### Entity2D

`Entity2D` représente une entité 2D visible. Sa taille définit celle du sprite;
la boîte de collision par défaut suit le sprite ou la frame d'animation affichée.

#### Position

La position initiale peut être fournie au constructeur. Elle peut ensuite être
modifiée avec `setPosition` :

```cpp
entity->setPosition(220.0f, groundY);
```

Cette fonction met à jour à la fois la position logique de l'entité et la
position de son sprite, sans modifier sa profondeur Z. Pour définir les trois
coordonnées en même temps, utilise `setPosition(x, y, z)`. Pour ne changer que
la profondeur, utilise `setZIndex(z)` :

```cpp
entity->setPosition(220.0f, groundY, 2.0f);
entity->setZIndex(3.0f);
```

La position X/Y est accessible via `entity->position`; la profondeur via
`entity->getZIndex()`.

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
locales à chaque `Entity2D`, décalées par rapport à sa position dans le monde.

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

La résolution est faite automatiquement par `EntityManager` lorsqu'une seule
des deux entités en contact s'est déplacée pendant la frame. Le joueur reçoit
également l'information `grounded`, ce qui lui permet de sauter depuis le sol ou
depuis le dessus d'un obstacle sans configuration supplémentaire dans le niveau.

Deux entités qui se chevauchent à des profondeurs Z différentes déclenchent
quand même leurs événements `OnCollision`, mais ne sont pas physiquement
résolues l'une contre l'autre. Elles peuvent ainsi se traverser tout en
détectant leur chevauchement. La résolution physique s'applique aux entités qui
ont le même Z.

#### Boîte de collision

Le paramètre `useCollision` active la participation de l'entité à la détection
des collisions. Il ne définit pas la taille de sa boîte. Sans boîte personnalisée,
`Entity2D` utilise le rectangle du sprite. Pendant une animation découpée, il
utilise le rectangle de texture courant replacé sur le canevas logique avec
`offsetX`, `offsetY`, `canvasWidth` et `canvasHeight`; la boîte suit ainsi le
frame recadré. Une animation non bouclée conserve sa dernière boîte de frame
jusqu'au démarrage de l'animation suivante.

Pour remplacer ce comportement par une boîte fixe, appelle `setCollisionBox`.
Les valeurs sont exprimées en pixels locaux depuis le coin supérieur gauche de
l'entité et priment sur le sprite et les frames d'animation :

```cpp
player->setCollisionBox(16.0f, 16.0f, 32.0f, 48.0f);
```

Sans cet appel, une entité créée avec `useCollision = true` utilise donc le
sprite ou la frame recadrée comme boîte de collision. Les entités avec
`useCollision = false` ne sont pas incluses dans les paires testées par
`EntityManager`.

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

### Exemple : Portail et changement de niveau

`Portal` accepte le type C++ du niveau cible dans son constructeur. La syntaxe
est volontairement courte, proche d'un `typeof` :

```cpp
Portal(
    "portal",
    "assets/sprites/blue_portal.png",
    48.0, 48.0,
    300.0f, groundY,
    true,
    typeid(Level2)
);
```

Le type du dernier paramètre est `LevelManager::LevelType`, un alias de
`std::type_index` :

```cpp
using LevelType = std::type_index;
```

Le niveau doit être enregistré une seule fois dans `LevelManager`, avec la
fabrique qui sait l'instancier :

```cpp
LevelManager::registerLevel(typeid(Level2),
    [&](const std::vector<Entity*>&)
    {
        currentState = std::make_unique<Level2>();
    });
```

Le portail demande ensuite simplement `typeid(Level2)`. Lorsqu'un joueur entre
en contact avec lui, il s'arme; le changement de niveau est demandé quand le
joueur appuie sur `E`. La transition est exécutée à la fin de la frame afin de ne
pas modifier la liste des entités pendant sa détection.

Exemple de liaison dans `main.cpp` :

```cpp
currentState = std::make_unique<Level1>(typeid(Level2));
```

Le portail ne s'arme qu'à l'entrée en collision d'un `Player`; il se désarme à
la sortie de collision. Il faut appuyer sur `E` pendant le contact pour changer
de niveau.

Lorsqu'un portail déclenche un changement de niveau, `LevelManager` conserve
automatiquement toutes les entités enregistrées avec `dontDestroyOnLoad`. Le
portail n'a donc pas besoin de recevoir la liste du joueur et du barrel :

```cpp
EntityManager::addEntity<Portal>(
    "portal", "assets/sprites/blue_portal.png",
    48.0, 48.0, 300.0f, groundY, true,
    typeid(Level2)
);
```

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

## Tools

### Éditeur de spritesheet

Le dépôt inclut un éditeur graphique Python pour créer et modifier les rectangles
de frame des animations. Il n'utilise que la bibliothèque standard Python
(Tkinter et `tk.PhotoImage`) :

```powershell
python tools/spritesheet_editor.py
```

Pour créer un JSON, ouvre une spritesheet, crée une animation, règle son nom, sa
durée par frame et son option de boucle, puis dessine chaque rectangle de frame
sur l'image. Renseigne aussi le canevas logique et les décalages de la frame,
ajoute-la à l'animation, puis répète dans l'ordre de lecture souhaité. Le bouton
**Exporter JSON…** enregistre le format explicite `animations[].frames` décrit
dans [Core / Animation](#animation).

Pour reprendre un travail existant, **Charger JSON…** ouvre d'abord le JSON, puis
demande de choisir la spritesheet correspondante. Le JSON ne contient pas de
chemin absolu vers l'image; le chemin choisi sert à l'éditeur. Le champ
`spritePath` du JSON reste le chemin relatif utilisé par le jeu et peut être
modifié dans la barre supérieure avant l'export.

Dans la liste des frames, sélectionne une frame pour afficher son rectangle et
ses valeurs de canevas/décalage. Tire le centre du rectangle pour le déplacer;
tire un bord ou un coin pour le redimensionner. Ces gestes mettent directement
à jour la frame sélectionnée. `Maj` + glisser commence une nouvelle sélection.
Les boutons **Copier sélection** et **Coller sélection**, ou `Ctrl+C` et `Ctrl+V`,
copient le rectangle avec ses dimensions et ses valeurs de canevas/décalage; le
rectangle collé peut ensuite être déplacé vers la frame suivante.

Le chargeur JSON de l'éditeur attend le format à rectangles explicites
`animations[].frames`. Les JSON utilisant uniquement l'ancien format en grille
(`row` et `columns` par animation) ne sont pas convertis automatiquement.
