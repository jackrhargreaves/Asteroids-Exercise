#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

class TextureManager {
public:
    TextureManager();
    void loadTexture(const std::string& name, const std::string& path, float scaleX, float scaleY);
    const sf::Texture& getTexture(const std::string& name) const;
    void rotateTexture(const std::string& name, float angle);

private:
    struct TextureInfo {
        sf::Texture texture;
        std::string name;

        TextureInfo(const std::string& name) : name(name) {}
    };

    std::vector<TextureInfo> textures;
};