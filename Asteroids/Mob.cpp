#include "Mob.h"
#include <iostream>
#include <memory> 
#include <iomanip>


Mob::Mob() {

    health = 100; 
   
}

void Mob::update(sf::Time deltaTime) {
   
}

/*
 *  Renders the mob on the game window, handling screen wrapping if enabled.
 *  If the mob is partially out of bounds, it renders a wrapped version/clone of the mob's sprite
 *  on the opposite side of the screen.
 *  If fully out of bounds, its warps the position to the clone
 *  Parameters:
 *    window: Reference to the SFML RenderWindow where the mob is rendered.
 */
void Mob::render(sf::RenderWindow& window) {
    window.draw(*this);  

    //Handle screem wrap
    if (viewBounds != nullptr) {
        sf::FloatRect spriteBounds = getBounds();
        sf::Vector2f spriteCenter = getPosition();  
        sf::Vector2f newPosition = spriteCenter;   

        // A small buffer beyond which the mob is considered fully out of bounds
        float buffer = 0.0f;  

        // Check and draw wrappedSprite for each boundary, considering the buffer
        if (spriteCenter.x - spriteBounds.width / 2 < viewBounds->left - buffer) {
            newPosition.x = spriteCenter.x + viewBounds->width;
            wrappedSprite.setPosition(newPosition);
            window.draw(wrappedSprite);
        }
        else if (spriteCenter.x + spriteBounds.width / 2 > viewBounds->left + viewBounds->width + buffer) {
            newPosition.x = spriteCenter.x - viewBounds->width;
            wrappedSprite.setPosition(newPosition);
            window.draw(wrappedSprite);
        }

        if (spriteCenter.y - spriteBounds.height / 2 < viewBounds->top - buffer) {
            newPosition.y = spriteCenter.y + viewBounds->height;
            wrappedSprite.setPosition(newPosition);
            window.draw(wrappedSprite);
        }
        else if (spriteCenter.y + spriteBounds.height / 2 > viewBounds->top + viewBounds->height + buffer) {
            newPosition.y = spriteCenter.y - viewBounds->height;
            wrappedSprite.setPosition(newPosition);
            window.draw(wrappedSprite);
        }

        // Update the Mob's position only if it's fully out of bounds, including the buffer
        if (spriteCenter.x < viewBounds->left - buffer || spriteCenter.x > viewBounds->left + viewBounds->width + buffer || spriteCenter.y < viewBounds->top - buffer || spriteCenter.y > viewBounds->top + viewBounds->height + buffer) {
            setPosition(newPosition);  
        }
    }
}


int Mob::abs(double value) {
    if (value < 0) return -1;
    return 1;
}

void Mob::setMoving(bool isMoving) {
    moving = isMoving;
}

void Mob::setTurning(bool isTurning) {
    turning = isTurning;
}

/*
 *  Sets the texture of the mob using the specified file path.
 *  Throws a runtime_error if the texture loading fails.
 *
 *  Parameters:
 *    path: The file path of the texture to load.
 */
void Mob::setTexture(const std::string& path) {
    if (!texture.loadFromFile(path)) {
        throw std::runtime_error("Failed to load Asteroid.png");
    }

    sf::Sprite::setTexture(texture);

    sf::FloatRect bounds = getLocalBounds();
    setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    wrappedSprite = this->clone();

    
}

/*
 *  Sets the texture of the mob using the provided texture object.
 *
 *  Parameters:
 *    texture: Reference to the texture object to set.
 */
void Mob::setTexture(const sf::Texture& texture) {
    sf::Sprite::setTexture(texture); 

    sf::FloatRect bounds = getLocalBounds();
    setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    wrappedSprite = this->clone();
}

/*
 *  Returns the global bounds of the mob's sprite.
 */
sf::FloatRect Mob::getBounds() const {
    return getGlobalBounds();
}

void Mob::rotate(float angle) {
    
    sf::Sprite::rotate(angle);

}

/*
 *  Sets the bounds of the mob's sprite to the specified rectangle, representing the world/screen bounds.
 *  If the view bounds pointer is null, it creates a new view bounds rectangle.
 *  Otherwise, it updates the existing view bounds rectangle with the provided one.
 *  This function is typically used to ensure that the mob stays within the visible area of the game.
 *
 *  Parameters:
 *    bounds: The rectangle defining the world bounds within which the mob's sprite should stay.
 */
void Mob::setBounds(const sf::FloatRect& bounds) {

    if (viewBounds == nullptr) {
        viewBounds = new sf::FloatRect(bounds);
    }
    else {
        *viewBounds = bounds;
    }

    boundsSet = true;
}

void Mob::setTextureManager(TextureManager& manager) {
    textureManager = &manager;
}

bool Mob::hasBounds() const {
    return boundsSet;
}


void Mob::setPosition(const sf::Vector2f& position) {
    sf::Sprite::setPosition(position);
}

void Mob::setPosition(float x, float y) {
    setPosition(sf::Vector2f(x, y)); // Delegate to the other setPosition
}


/*
 *  Creates and returns a copy of the mob's sprite, including its texture, scale, position, and rotation.
 *  This function is used to clone the mob's sprite for screen wrapping functionality.
 *
 *  Returns:
 *    A copy of the mob's sprite with the same texture, scale, position, and rotation.
 */
sf::Sprite Mob::clone() const {
    sf::Sprite newSprite;

    if (this->getTexture()) {
        newSprite.setTexture(*(this->getTexture()));
    }

    newSprite.setScale(this->getScale());

    newSprite.setPosition(this->getPosition());
    newSprite.setRotation(this->getRotation());

    sf::FloatRect bounds = newSprite.getLocalBounds();
    newSprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    return newSprite;
}

/*
 *  Draws a red border around the given view bounds in the specified render window.
 *  This function is used for debugging purposes to visualise the boundaries of the game view.
 *
 *  Parameters:
 *    window: The render window where the red border will be drawn.
 *    viewBounds: The boundaries of the game view to draw the red border around.
 */
void Mob::drawRedBorder(sf::RenderWindow& window, const sf::FloatRect* viewBounds) {

    float thickness = 2.f;

    if (viewBounds == nullptr) {
        // If the viewBounds pointer is null, do nothing
        return;
    }

    sf::Vertex lines[8]; // Array to hold the vertices for the four lines

    // Top line
    lines[0] = sf::Vertex(sf::Vector2f(viewBounds->left, viewBounds->top), sf::Color::Red);
    lines[1] = sf::Vertex(sf::Vector2f(viewBounds->left + viewBounds->width, viewBounds->top), sf::Color::Red);

    // Bottom line
    lines[2] = sf::Vertex(sf::Vector2f(viewBounds->left, viewBounds->top + viewBounds->height), sf::Color::Red);
    lines[3] = sf::Vertex(sf::Vector2f(viewBounds->left + viewBounds->width, viewBounds->top + viewBounds->height), sf::Color::Red);

    // Left line
    lines[4] = sf::Vertex(sf::Vector2f(viewBounds->left, viewBounds->top), sf::Color::Red);
    lines[5] = sf::Vertex(sf::Vector2f(viewBounds->left, viewBounds->top + viewBounds->height), sf::Color::Red);

    // Right line
    lines[6] = sf::Vertex(sf::Vector2f(viewBounds->left + viewBounds->width, viewBounds->top), sf::Color::Red);
    lines[7] = sf::Vertex(sf::Vector2f(viewBounds->left + viewBounds->width, viewBounds->top + viewBounds->height), sf::Color::Red);

    window.draw(lines, 8, sf::Lines);
}

void Mob::setVelocity(sf::Vector2f velocity) {
    //std::cout << "NEW velocity" << velocity.x << std::endl;
    Mob::velocity = velocity;
}

bool Mob::hasCollision() const{
    return solid;
}

/*
 *  Prints information about the texture applied to the sprite, including its width, height, and pixel data.
 *  This function is useful for debugging purposes to inspect the texture information.
 */
void Mob::printTextureInfo() const {
    const sf::Texture* texture = getTexture();

    if (texture != nullptr) {
        // Convert the texture to an image to access pixel data
        sf::Image image = texture->copyToImage();

        sf::Vector2u size = image.getSize();
        std::cout << "Texture Info:" << std::endl;
        std::cout << "Width: " << size.x << std::endl;
        std::cout << "Height: " << size.y << std::endl;

        // Iterate over each pixel in the image
        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                sf::Color color = image.getPixel(x, y);
                std::cout << "("
                    << std::setw(3) << static_cast<int>(color.r) << ", "
                    << std::setw(3) << static_cast<int>(color.g) << ", "
                    << std::setw(3) << static_cast<int>(color.b) << ", "
                    << std::setw(3) << static_cast<int>(color.a) << ") ";
            }
            std::cout << std::endl;
        }
    }
    else {
        std::cout << "The sprite does not have a texture." << std::endl;
    }
}

