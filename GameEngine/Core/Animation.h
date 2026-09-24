#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <cstddef>
#include <string>
#include <memory>
#include <optional>
#include <vector>

struct AnimationFrame
{
    sf::IntRect textureRect;
    sf::Vector2f offset = sf::Vector2f(0.0f, 0.0f);
    sf::Vector2f canvasSize;
};

struct AnimationSpriteFrame
{
    std::shared_ptr<sf::Texture> texture;
    AnimationFrame frame;
};

class Animation
{
public:
    bool configure(const std::vector<std::string>& framePaths,
        float frameDuration = 0.1f, bool loop = true);
    bool configureSpriteSheet(const std::string& spriteSheetPath,
        unsigned int columns, unsigned int rows,
        float frameDuration = 0.1f, bool loop = true);
    bool configureSpriteSheetRow(const std::string& spriteSheetPath,
        unsigned int columns, unsigned int rows, unsigned int row,
        unsigned int frameCount, float frameDuration = 0.1f,
        bool loop = true, bool reverse = false);
    bool configureSpriteSheetRow(const std::shared_ptr<sf::Texture>& spriteSheet,
        unsigned int columns, unsigned int rows, unsigned int row,
        unsigned int frameCount, float frameDuration = 0.1f,
        bool loop = true, bool reverse = false);
    bool configureSpriteSheetFrames(const std::shared_ptr<sf::Texture>& spriteSheet,
        const std::vector<AnimationFrame>& frameData,
        float frameDuration = 0.1f, bool loop = true);

    void start();
    bool start(const std::vector<std::string>& framePaths,
        float frameDuration = 0.1f, bool loop = true);
    bool start(const std::string& spriteSheetPath,
        unsigned int columns, unsigned int rows,
        float frameDuration = 0.1f, bool loop = true);
    bool startSpriteSheet(const std::string& spriteSheetPath,
        unsigned int columns, unsigned int rows,
        float frameDuration = 0.1f, bool loop = true);
    void stop();
    void pause();
    void resume();
    void update(float deltaTime);

    bool isConfigured() const;
    bool isPlaying() const;
    bool isFinished() const;
    const sf::Texture* getTexture() const;
    const sf::IntRect& getTextureRect() const;
    const AnimationFrame& getCurrentFrame() const;
    std::optional<AnimationSpriteFrame> getFrameSprite(std::size_t frameIndex) const;

private:
    std::vector<std::shared_ptr<sf::Texture>> textures;
    std::vector<AnimationFrame> frames;
    float frameDuration = 0.1f;
    float elapsedTime = 0.0f;
    std::size_t currentFrame = 0;
    bool loop = true;
    bool playing = false;
    bool finished = false;
};
