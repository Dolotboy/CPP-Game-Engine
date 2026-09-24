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
    frames.clear();
    frames.reserve(textures.size());
    for (const std::shared_ptr<sf::Texture>& texture : textures)
    {
        const sf::Vector2u size = texture->getSize();
        frames.push_back({ sf::IntRect(0, 0, static_cast<int>(size.x),
            static_cast<int>(size.y)), sf::Vector2f(0.0f, 0.0f),
            sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)) });
    }

    frameDuration = newFrameDuration;
    loop = newLoop;
    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = false;
    finished = false;
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
    frames.clear();
    frames.reserve(static_cast<std::size_t>(columns) * rows);
    for (unsigned int row = 0; row < rows; ++row)
    {
        for (unsigned int column = 0; column < columns; ++column)
        {
            frames.push_back({ sf::IntRect(
                static_cast<int>(column * frameWidth),
                static_cast<int>(row * frameHeight),
                static_cast<int>(frameWidth),
                static_cast<int>(frameHeight)), sf::Vector2f(0.0f, 0.0f),
                sf::Vector2f(static_cast<float>(frameWidth),
                    static_cast<float>(frameHeight)) });
        }
    }

    frameDuration = newFrameDuration;
    loop = newLoop;
    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = false;
    finished = false;
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

    std::vector<AnimationFrame> rowFrames;
    rowFrames.reserve(frameCount);
    for (unsigned int frame = 0; frame < frameCount; ++frame)
    {
        const unsigned int column = reverse ? frameCount - 1 - frame : frame;
        rowFrames.push_back({ sf::IntRect(static_cast<int>(column * frameWidth),
            static_cast<int>(row * frameHeight), static_cast<int>(frameWidth),
            static_cast<int>(frameHeight)), sf::Vector2f(0.0f, 0.0f),
            sf::Vector2f(static_cast<float>(frameWidth),
                static_cast<float>(frameHeight)) });
    }

    return configureSpriteSheetFrames(spriteSheet, rowFrames, newFrameDuration, newLoop);
}

bool Animation::configureSpriteSheetFrames(
    const std::shared_ptr<sf::Texture>& spriteSheet,
    const std::vector<AnimationFrame>& frameData,
    float newFrameDuration, bool newLoop)
{
    if (!spriteSheet || frameData.empty() || newFrameDuration <= 0.0f)
        return false;

    const sf::Vector2u textureSize = spriteSheet->getSize();
    for (const AnimationFrame& frame : frameData)
    {
        const sf::IntRect& rect = frame.textureRect;
        if (rect.left < 0 || rect.top < 0 || rect.width <= 0 || rect.height <= 0 ||
            rect.left + rect.width > static_cast<int>(textureSize.x) ||
            rect.top + rect.height > static_cast<int>(textureSize.y) ||
            frame.canvasSize.x <= 0.0f || frame.canvasSize.y <= 0.0f ||
            frame.offset.x < 0.0f || frame.offset.y < 0.0f ||
            frame.offset.x + static_cast<float>(rect.width) > frame.canvasSize.x ||
            frame.offset.y + static_cast<float>(rect.height) > frame.canvasSize.y)
            return false;
    }

    textures.clear();
    textures.push_back(spriteSheet);
    frames = frameData;
    frameDuration = newFrameDuration;
    loop = newLoop;
    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = false;
    finished = false;
    return true;
}

void Animation::start()
{
    if (!isConfigured())
        return;

    currentFrame = 0;
    elapsedTime = 0.0f;
    playing = true;
    finished = false;
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
    finished = false;
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

        if (currentFrame >= frames.size())
        {
            if (!loop)
            {
                currentFrame = frames.size() - 1;
                playing = false;
                finished = true;
                elapsedTime = 0.0f;
                return;
            }

            currentFrame = 0;
        }
    }
}

bool Animation::isConfigured() const
{
    return !textures.empty() && !frames.empty();
}

bool Animation::isPlaying() const
{
    return playing;
}

bool Animation::isFinished() const
{
    return finished;
}

const sf::Texture* Animation::getTexture() const
{
	if (!isConfigured())
		return nullptr;
	return textures.size() == frames.size()
		? textures[currentFrame].get() : textures.front().get();
}

const sf::IntRect& Animation::getTextureRect() const
{
    static const sf::IntRect emptyRect;
    return isConfigured() ? frames[currentFrame].textureRect : emptyRect;
}

const AnimationFrame& Animation::getCurrentFrame() const
{
    static const AnimationFrame emptyFrame;
    return isConfigured() ? frames[currentFrame] : emptyFrame;
}

std::optional<AnimationSpriteFrame> Animation::getFrameSprite(
    std::size_t frameIndex) const
{
    if (!isConfigured() || frameIndex >= frames.size())
        return std::nullopt;

    const std::shared_ptr<sf::Texture>& frameTexture =
        textures.size() == frames.size() ? textures[frameIndex] : textures.front();
    return AnimationSpriteFrame{ frameTexture, frames[frameIndex] };
}
