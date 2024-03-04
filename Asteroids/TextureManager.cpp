#include "TextureManager.h"


TextureManager::TextureManager() {


}

/*
 *  Loads a texture from the specified file path, scales it, and stores it in the texture manager.
 *  Throws a runtime_error if the loading or scaling fails.
 *
 *  Parameters:
 *    name:    The name to associate with the loaded texture.
 *    path:    The file path of the texture to load.
 *    scaleX:  The scale factor for the texture along the x-axis.
 *    scaleY:  The scale factor for the texture along the y-axis.
 */
void TextureManager::loadTexture(const std::string& name, const std::string& path, float scaleX, float scaleY) {
    sf::Image originalImage;
    if (!originalImage.loadFromFile(path)) {
        throw std::runtime_error("Failed to load image from " + path);
    }

    sf::Vector2u originalSize = originalImage.getSize();
    sf::Image scaledImage;
    scaledImage.create(static_cast<unsigned int>(originalSize.x * scaleX), static_cast<unsigned int>(originalSize.y * scaleY));

    for (unsigned int y = 0; y < scaledImage.getSize().y; ++y) {
        for (unsigned int x = 0; x < scaledImage.getSize().x; ++x) {

            unsigned int origX = static_cast<unsigned int>(x / scaleX);
            unsigned int origY = static_cast<unsigned int>(y / scaleY);

            scaledImage.setPixel(x, y, originalImage.getPixel(origX, origY));
        }
    }

    sf::Texture texture;
    if (!texture.loadFromImage(scaledImage)) {
        throw std::runtime_error("Failed to create texture from scaled image");
    }

    textures.emplace_back(name);
    textures.back().texture = std::move(texture);
}


/*
 *  Retrieves a texture from the texture manager by name.
 *  Throws a runtime_error if the texture with the specified name is not found.
 *
 *  Parameters:
 *    name: The name of the texture to retrieve.
 *
 *  Returns:
 *    A const reference to the texture associated with the specified name.
 */
const sf::Texture& TextureManager::getTexture(const std::string& name) const {
    auto it = std::find_if(textures.begin(), textures.end(), [&name](const TextureInfo& info) {
        return info.name == name;
        });

    if (it != textures.end()) {
        return it->texture;
    }
    else {
        throw std::runtime_error("Texture not found: " + name);
    }
}


/*
 *  Rotates a texture by the specified angle and updates it in the texture manager.
 *  Throws a runtime_error if the texture with the specified name is not found.
 *
 *  Parameters:
 *    name:  The name of the texture to rotate.
 *    angle: The angle by which to rotate the texture.
 */

void TextureManager::rotateTexture(const std::string& name, float angle) {
    auto it = std::find_if(textures.begin(), textures.end(), [&name](const TextureInfo& info) {
        return info.name == name;
        });

    if (it == textures.end()) {
        throw std::runtime_error("Texture not found: " + name);
    }

    sf::Sprite sprite(it->texture);

    sf::FloatRect bounds = sprite.getLocalBounds();
    sf::Transform rotation;
    rotation.rotate(angle, bounds.width / 2, bounds.height / 2);
    sf::FloatRect rotatedBounds = rotation.transformRect(bounds);

    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite.setRotation(angle);

    sf::RenderTexture renderTexture;
    renderTexture.create(static_cast<unsigned int>(rotatedBounds.width), static_cast<unsigned int>(rotatedBounds.height));
    renderTexture.clear(sf::Color::Transparent);

    sprite.setPosition(rotatedBounds.width / 2.0f, rotatedBounds.height / 2.0f);

 
    renderTexture.draw(sprite);
    renderTexture.display();

    it->texture = renderTexture.getTexture();
}