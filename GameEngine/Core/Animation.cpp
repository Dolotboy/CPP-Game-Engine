#include "Animation.h"

#include <iostream>
#include <memory>
#include <utility>

bool Animation::configure(const std::vector<std::string>& framePaths,
    float newFrameDuration, bool newLoop)
{
    if (framePaths.empty() || newFrameDuration <= 0.0f)
        return false;

    std::vector<std::shared_ptr<sf::Texture>> loadedTextures;
    loadedTextures.reserve(framePaths.size());

    for (const std::string& framePath : framePaths)
    {
        auto texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromFile(framePath))
        {
            std::cerr << "Unable to load animation frame: " << framePath << std::endl;
            return false;
        }

        loadedTextures.push_back(std::move(texture));
    }

    textures = std::move(loadedTextures);
    textureRects.clear();
    textureRects.reserve(textures.size());
    for (const std::shared_ptr<sf::Texture>& texture : textures)
    {
        const sf::Vector2u size = texture->getSize();
        textureRects.emplace_back(0, 0,
            static_cast<int>(size.x), static_cast<int>(size.y));
    }

    frameDuration = newFrameDuration;
    loop = newLoop;
    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = false;
    return true;
}

bool Animation::configureSpriteSheet(const std::string& spriteSheetPath,
    unsigned int columns, unsigned int rows,
    float newFrameDuration, bool newLoop)
{
    if (spriteSheetPath.empty() || columns == 0 || rows == 0 || newFrameDuration <= 0.0f)
        return false;

    sf::Texture texture;
    if (!texture.loadFromFile(spriteSheetPath))
    {
        std::cerr << "Unable to load animation sprite sheet: " << spriteSheetPath << std::endl;
        return false;
    }

    const sf::Vector2u size = texture.getSize();
    const unsigned int frameWidth = size.x / columns;
    const unsigned int frameHeight = size.y / rows;
    if (frameWidth == 0 || frameHeight == 0)
        return false;

    textures.clear();
    textures.push_back(std::make_shared<sf::Texture>(std::move(texture)));
    textureRects.clear();
    textureRects.reserve(static_cast<std::size_t>(columns) * rows);
    for (unsigned int row = 0; row < rows; ++row)
    {
        for (unsigned int column = 0; column < columns; ++column)
        {
            textureRects.emplace_back(
                static_cast<int>(column * frameWidth),
                static_cast<int>(row * frameHeight),
                static_cast<int>(frameWidth),
                static_cast<int>(frameHeight));
        }
    }

    frameDuration = newFrameDuration;
    loop = newLoop;
    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = false;
    return true;
}

bool Animation::configureSpriteSheetRow(const std::string& spriteSheetPath,
    unsigned int columns, unsigned int rows, unsigned int row,
    unsigned int frameCount, float newFrameDuration, bool newLoop, bool reverse)
{
    if (spriteSheetPath.empty() || columns == 0 || rows == 0 || row >= rows ||
        frameCount == 0 || frameCount > columns || newFrameDuration <= 0.0f)
        return false;

    sf::Texture texture;
    if (!texture.loadFromFile(spriteSheetPath))
    {
        std::cerr << "Unable to load animation sprite sheet: " << spriteSheetPath << std::endl;
        return false;
    }

    return configureSpriteSheetRow(std::make_shared<sf::Texture>(std::move(texture)),
        columns, rows, row, frameCount, newFrameDuration, newLoop, reverse);
}

bool Animation::configureSpriteSheetRow(const std::shared_ptr<sf::Texture>& spriteSheet,
    unsigned int columns, unsigned int rows, unsigned int row,
    unsigned int frameCount, float newFrameDuration, bool newLoop, bool reverse)
{
    if (!spriteSheet || columns == 0 || rows == 0 || row >= rows ||
        frameCount == 0 || frameCount > columns || newFrameDuration <= 0.0f)
        return false;

    const sf::Vector2u size = spriteSheet->getSize();
    const unsigned int frameWidth = size.x / columns;
    const unsigned int frameHeight = size.y / rows;
    if (frameWidth == 0 || frameHeight == 0)
        return false;

    textures.clear();
    textures.push_back(spriteSheet);
    textureRects.clear();
    textureRects.reserve(frameCount);
    for (unsigned int frame = 0; frame < frameCount; ++frame)
    {
        const unsigned int column = reverse ? frameCount - 1 - frame : frame;
        textureRects.emplace_back(static_cast<int>(column * frameWidth),
            static_cast<int>(row * frameHeight), static_cast<int>(frameWidth),
            static_cast<int>(frameHeight));
    }

    frameDuration = newFrameDuration;
    loop = newLoop;
    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = false;
    return true;
}

void Animation::start()
{
    if (!isConfigured())
        return;

    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = true;
}

bool Animation::start(const std::vector<std::string>& framePaths,
    float newFrameDuration, bool newLoop)
{
    if (!configure(framePaths, newFrameDuration, newLoop))
        return false;

    start();
    return true;
}

bool Animation::start(const std::string& spriteSheetPath,
    unsigned int columns, unsigned int rows,
    float newFrameDuration, bool newLoop)
{
    return startSpriteSheet(spriteSheetPath, columns, rows,
        newFrameDuration, newLoop);
}

bool Animation::startSpriteSheet(const std::string& spriteSheetPath,
    unsigned int columns, unsigned int rows,
    float newFrameDuration, bool newLoop)
{
    if (!configureSpriteSheet(spriteSheetPath, columns, rows, newFrameDuration, newLoop))
        return false;

    start();
    return true;
}

void Animation::stop()
{
    playing = false;
    elapsedTime = 0.0f;
    currentFrame = 0;
}

void Animation::pause()
{
    playing = false;
}

void Animation::resume()
{
    if (isConfigured())
        playing = true;
}

void Animation::update(float deltaTime)
{
    if (!playing || !isConfigured() || deltaTime <= 0.0f)
        return;

    elapsedTime += deltaTime;
    while (elapsedTime >= frameDuration)
    {
        elapsedTime -= frameDuration;
        ++currentFrame;

        if (currentFrame >= textureRects.size())
        {
            if (!loop)
            {
                currentFrame = textureRects.size() - 1;
                playing = false;
                elapsedTime = 0.0f;
                return;
            }

            currentFrame = 0;
        }
    }
}

bool Animation::isConfigured() const
{
    return !textures.empty() && !textureRects.empty();
}

bool Animation::isPlaying() const
{
    return playing;
}

const sf::Texture* Animation::getTexture() const
{
    return isConfigured() ? textures.front().get() : nullptr;
}

const sf::IntRect& Animation::getTextureRect() const
{
    static const sf::IntRect emptyRect;
    return isConfigured() ? textureRects[currentFrame] : emptyRect;
}
