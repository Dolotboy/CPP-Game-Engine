#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <limits>
#include <regex>
#include <sstream>

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

    bool jsonBoolean(const std::string& json, const std::string& key, bool fallback)
    {
        const std::regex field("\\\"" + key + "\\\"\\s*:\\s*(true|false)");
        std::smatch match;
        if (!std::regex_search(json, match, field))
            return fallback;
        return match[1].str() == "true";
    }

    std::size_t matchingBracket(const std::string& json, std::size_t opening)
    {
        int depth = 0;
        bool inString = false;
        bool escaped = false;
        for (std::size_t i = opening; i < json.size(); ++i)
        {
            const char character = json[i];
            if (inString)
            {
                if (escaped)
                    escaped = false;
                else if (character == '\\')
                    escaped = true;
                else if (character == '"')
                    inString = false;
                continue;
            }

            if (character == '"')
                inString = true;
            else if (character == '[')
                ++depth;
            else if (character == ']' && --depth == 0)
                return i;
        }
        return std::string::npos;
    }

    std::vector<std::string> arrayObjects(const std::string& json,
        const std::string& key)
    {
        const std::size_t keyPosition = json.find("\"" + key + "\"");
        if (keyPosition == std::string::npos)
            return {};
        const std::size_t opening = json.find('[', keyPosition);
        if (opening == std::string::npos)
            return {};
        const std::size_t closing = matchingBracket(json, opening);
        if (closing == std::string::npos)
            return {};

        std::vector<std::string> objects;
        std::size_t objectStart = std::string::npos;
        int objectDepth = 0;
        bool inString = false;
        bool escaped = false;
        for (std::size_t i = opening + 1; i < closing; ++i)
        {
            const char character = json[i];
            if (inString)
            {
                if (escaped)
                    escaped = false;
                else if (character == '\\')
                    escaped = true;
                else if (character == '"')
                    inString = false;
                continue;
            }

            if (character == '"')
                inString = true;
            else if (character == '{')
            {
                if (objectDepth++ == 0)
                    objectStart = i;
            }
            else if (character == '}' && --objectDepth == 0 &&
                objectStart != std::string::npos)
            {
                objects.push_back(json.substr(objectStart, i - objectStart + 1));
                objectStart = std::string::npos;
            }
        }
        return objects;
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
    if (spritePath.empty())
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
    bool registeredAny = false;
    const std::vector<std::string> animationDefinitions = arrayObjects(json, "animations");
    for (const std::string& object : animationDefinitions)
    {
        const std::string name = jsonString(object, "name");
        const unsigned int row = jsonNumber(object, "row", rows);
        const unsigned int frameCount = jsonNumber(object, "columns", 0);
        const std::regex reverseField("\"reverse\"\\s*:\\s*true");
        const bool reverse = std::regex_search(object, reverseField);
        const bool loop = jsonBoolean(object, "loop", true);
        const unsigned int frameDurationMs = jsonNumber(object, "frameDurationMs", 100);
        if (name.empty())
            continue;

        Animation configured;
        const float frameDuration = static_cast<float>(frameDurationMs) / 1000.0f;
        const std::vector<std::string> frameObjects = arrayObjects(object, "frames");
        bool configuredSuccessfully = false;
        if (!frameObjects.empty())
        {
            std::vector<AnimationFrame> frameData;
            frameData.reserve(frameObjects.size());
            for (const std::string& frame : frameObjects)
            {
                const unsigned int missing = std::numeric_limits<unsigned int>::max();
                const unsigned int frameX = jsonNumber(frame, "x", missing);
                const unsigned int frameY = jsonNumber(frame, "y", missing);
                const int width = static_cast<int>(jsonNumber(frame, "width", 0));
                const int height = static_cast<int>(jsonNumber(frame, "height", 0));
                if (frameX == missing || frameY == missing ||
                    frameX > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
                    frameY > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
                    width <= 0 || height <= 0)
                {
                    frameData.clear();
                    break;
                }
                const unsigned int offsetX = jsonNumber(frame, "offsetX", 0);
                const unsigned int offsetY = jsonNumber(frame, "offsetY", 0);
                const unsigned int canvasWidth = jsonNumber(frame, "canvasWidth",
                    offsetX + static_cast<unsigned int>(width));
                const unsigned int canvasHeight = jsonNumber(frame, "canvasHeight",
                    offsetY + static_cast<unsigned int>(height));
                frameData.push_back({ sf::IntRect(static_cast<int>(frameX),
                    static_cast<int>(frameY), width, height),
                    sf::Vector2f(static_cast<float>(offsetX), static_cast<float>(offsetY)),
                    sf::Vector2f(static_cast<float>(canvasWidth),
                        static_cast<float>(canvasHeight)) });
            }
            if (!frameData.empty())
                configuredSuccessfully = configured.configureSpriteSheetFrames(
                    spriteSheet, frameData, frameDuration, loop);
        }
        else if (columns > 0 && rows > 0)
        {
            configuredSuccessfully = configured.configureSpriteSheetRow(spriteSheet,
                columns, rows, row, frameCount, frameDuration, loop, reverse);
        }

        if (!configuredSuccessfully)
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

    if (activeAnimationName == animationName &&
        (animation.isPlaying() || animation.isFinished()))
        return true;

    animation = found->second;
    activeAnimationName = animationName;
    animation.start();
    return true;
}

void Entity::render(sf::RenderTarget& target)
{
}

void Entity::printInfo() {
	std::cout << "Entity ID: " << entityId << std::endl;
}
