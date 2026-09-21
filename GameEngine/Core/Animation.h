#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <vector>

class Animation
{
public:
    bool configure(const std::vector<std::string>& framePaths,
        float frameDuration = 0.1f, bool loop = true);
    bool configureSpriteSheet(const std::string& spriteSheetPath,
        unsigned int columns, unsigned int rows,
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
    const sf::Texture* getTexture() const;
    const sf::IntRect& getTextureRect() const;

private:
    std::vector<sf::Texture> textures;
    std::vector<sf::IntRect> textureRects;
    float frameDuration = 0.1f;
    float elapsedTime = 0.0f;
    std::size_t currentFrame = 0;
    bool loop = true;
    bool playing = false;
};