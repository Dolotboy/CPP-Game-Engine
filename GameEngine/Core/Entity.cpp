#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <regex>
#include <sstream>
#include <SFML/Graphics/Texture.hpp>

namespace
{
    std::string jsonString(const std::string& json, const std::string& key)
    {
        const std::regex field("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"");
        std::smatch match;
        return std::regex_search(json, match, field) ? match[1].str() : std::string();
    }

    unsigned int jsonNumber(const std::string& json, const std::string& key,
        unsigned int fallback)
    {
        const std::regex field("\\\"" + key + "\\\"\\s*:\\s*(\\d+)");
        std::smatch match;
        return std::regex_search(json, match, field)
            ? static_cast<unsigned int>(std::stoul(match[1].str())) : fallback;
    }
}

Entity::Entity(bool is2D, string entityName, float x, float y)
{
	this->is2D = is2D;
	this->entityName = entityName;
	this->entityId = EntityManager::generateEntityId();

	this->position = sf::Vector2f(x, y);
	this->velocity = sf::Vector2f(0.f, 0.f);

	EntityManager::entities.push_back(this);
}

Entity::Entity(string entityName, float x, float y)
	: Entity(true, entityName, x, y)
{
}

void Entity::update(float deltaTime)
{
	this->position += this->velocity * deltaTime;
	this->animation.update(deltaTime);
}

void Entity::setPosition(float x, float y)
{
	this->position = sf::Vector2f(x, y);
}

void Entity::setVelocity(float x, float y)
{
	this->velocity = sf::Vector2f(x, y);
}

bool Entity::registerAnimation(const std::string& animationPath)
{
    std::ifstream file(animationPath);
    if (!file)
    {
        std::cerr << "Unable to open animation definition: " << animationPath << std::endl;
        return false;
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    const std::string json = contents.str();
    const std::string spritePath = jsonString(json, "spritePath");
    const unsigned int columns = jsonNumber(json, "columns", 0);
    const unsigned int rows = jsonNumber(json, "rows", 0);
    if (spritePath.empty() || columns == 0 || rows == 0)
        return false;

    const std::filesystem::path definitionPath(animationPath);
    const std::filesystem::path resolvedSpritePath =
        definitionPath.parent_path().parent_path() / spritePath;
    auto spriteSheet = std::make_shared<sf::Texture>();
    if (!spriteSheet->loadFromFile(resolvedSpritePath.string()))
    {
        std::cerr << "Unable to load animation sprite sheet: "
            << resolvedSpritePath.string() << std::endl;
        return false;
    }
    const std::regex objectPattern("\\{[^{}]*\\}");
    const std::size_t animationsStart = json.find_first_of('[', json.find("\"animations\""));
    const std::size_t animationsEnd = json.find(']', animationsStart);
    if (animationsStart == std::string::npos || animationsEnd == std::string::npos)
        return false;

    const std::string definitions = json.substr(animationsStart + 1,
        animationsEnd - animationsStart - 1);
    bool registeredAny = false;
    for (std::sregex_iterator it(definitions.begin(), definitions.end(), objectPattern), end;
        it != end; ++it)
    {
        const std::string object = it->str();
        const std::string name = jsonString(object, "name");
        const unsigned int row = jsonNumber(object, "row", rows);
        const unsigned int frameCount = jsonNumber(object, "columns", 0);
        const std::regex reverseField("\"reverse\"\\s*:\\s*true");
        const bool reverse = std::regex_search(object, reverseField);
        if (name.empty())
            continue;

        Animation configured;
        if (!configured.configureSpriteSheetRow(spriteSheet, columns,
            rows, row, frameCount, 0.1f, true, reverse))
            return false;
        registeredAnimations[name] = std::move(configured);
        registeredAny = true;
    }

    return registeredAny;
}

bool Entity::startAnimation(const std::string& animationName)
{
    const auto found = registeredAnimations.find(animationName);
    if (found == registeredAnimations.end())
        return false;

    animation = found->second;
    animation.start();
    return true;
}

void Entity::render(sf::RenderTarget& target)
{
}

void Entity::printInfo() {
	std::cout << "Entity ID: " << entityId << std::endl;
}
