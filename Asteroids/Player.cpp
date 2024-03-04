#include "Player.h"
#include "Projectile.h"
#include <iostream>

Player::Player() {

	//setTexture("Assets/Ship.png");
}


/*
 *  Updates the player's position, velocity, and animation based on user input and game mechanics.
 *
 *  Parameters:
 *    deltaTime: Time elapsed since the last update cycle.
 */
void Player::update(sf::Time deltaTime) {
    if (!moving) {
        velocity *= decelerationFactor;  // Apply deceleration when not moving
    }
    else {
        float rotationInRadians = (getRotation() - 90) * (3.14159265f / 180.0f);
        sf::Vector2f forward(std::cos(rotationInRadians), std::sin(rotationInRadians));

        sf::Vector2f forwardAcceleration = forward * std::sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
        forwardAcceleration *= accelerationFactor;

        velocity += forwardAcceleration * deltaTime.asSeconds();
    }

    // Apply a drag
    if (turning) {
        velocity *= 1.0f - (dragFactor * deltaTime.asSeconds());
    }

    // Clamp the velocity to the maxSpeed in any direction
    float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentSpeed > maxSpeed.x) {
        velocity = (velocity / currentSpeed) * maxSpeed.x;
    }

    sf::Vector2f newPosition = getPosition() + velocity * deltaTime.asSeconds();
    setPosition(newPosition);


    textureToggleTimer += deltaTime;

    //Player Animation
    if (moving || turning) {
        float currentDuration = isPrimaryTextureActive ? primaryTextureDuration : secondaryTextureDuration;

        if (textureToggleTimer.asSeconds() >= currentDuration) {
            isPrimaryTextureActive = !isPrimaryTextureActive; 
            textureToggleTimer = sf::Time::Zero;  

            
            if (isPrimaryTextureActive) {
                Mob::setTexture(*primaryTexture); 

            } else {
                Mob::setTexture(*secondaryTexture);  
            }
        }
    }
    else {
        if (!isPrimaryTextureActive) { 
            Mob::setTexture(*secondaryTexture);
        }
        isPrimaryTextureActive = false; 
    }
}

/*
 *  Fires a projectile from the player's position towards the nearest target within the aiming cone.
 *
 *  Parameters:
 *    projectiles: Vector to store the fired projectiles.
 *    texture: Texture to apply to the projectile.
 *    mobs: Vector containing the list of mobs in the game.
 */
void Player::fire(std::vector<Projectile>& projectiles, const sf::Texture& texture, std::vector<Mob*>& mobs) {
    sf::Vector2f position = this->getPosition();
    sf::Vector2f direction = sf::Vector2f(std::cos((this->getRotation() - 90) * (3.14159265f / 180.0f)),
        std::sin((this->getRotation() - 90) * (3.14159265f / 180.0f)));

    Mob* selectedMob = nullptr;
    float closestDistanceSquared = std::numeric_limits<float>::max();

    Projectile newProjectile(position, direction, getRotation(), projectileSpeed, texture);

    if (viewBounds != nullptr) {
        newProjectile.setBounds(*viewBounds);
    }

    for (auto& mob : mobs) {
        if ((mob->type() == Mob::Type::Asteroid || mob->type() == Mob::Type::EnemyShip) && newProjectile.withinAimingCone(*mob)) {
            sf::Vector2f mobDir = mob->getPosition() - position;
            float mobDistanceSquared = mobDir.x * mobDir.x + mobDir.y * mobDir.y;


            if (mobDistanceSquared < closestDistanceSquared) {
                closestDistanceSquared = mobDistanceSquared;
                selectedMob = mob; 
            }
        }
    }

    if (selectedMob != nullptr) {
  
        newProjectile.setTarget(*selectedMob);
    }

    projectiles.push_back(newProjectile);
}

/*
 *  Draws debug lines representing the aiming cone of the player's ship.
 *
 *  Parameters:
 *    target: Render target to draw the debug lines.
 *    length: Length of the aiming cone lines.
 */
void Player::debugAimingCone(sf::RenderTarget& target, float length) {
    float halfAngleRadians = (60.0f / 2.0f) * (3.14159265f / 180.0f); 
    float rotationInRadians = (this->getRotation() - 90) * (3.14159265f / 180.0f); 


    sf::Vector2f directionLeft = sf::Vector2f(std::cos(rotationInRadians - halfAngleRadians), std::sin(rotationInRadians - halfAngleRadians));
    sf::Vector2f directionRight = sf::Vector2f(std::cos(rotationInRadians + halfAngleRadians), std::sin(rotationInRadians + halfAngleRadians));


    sf::Vector2f endLeft = this->getPosition() + directionLeft * length;
    sf::Vector2f endRight = this->getPosition() + directionRight * length;


    sf::Vertex lineLeft[] = { sf::Vertex(this->getPosition()), sf::Vertex(endLeft) };
    sf::Vertex lineRight[] = { sf::Vertex(this->getPosition()), sf::Vertex(endRight) };


    lineLeft[0].color = sf::Color::Red;
    lineLeft[1].color = sf::Color::Red;
    lineRight[0].color = sf::Color::Red;
    lineRight[1].color = sf::Color::Red;

    // Draw the lines to the render target
    target.draw(lineLeft, 2, sf::Lines);
    target.draw(lineRight, 2, sf::Lines);
}

void Player::setAcceleration(const sf::Vector2f& newAcceleration) {
    acceleration = newAcceleration;

}

void Player::setRotation(bool right) {

    float rotationStep = rotationFactor * (right ? 1.0f : -1.0f);

    float newRotation = getRotation() + rotationStep;
    sf::Sprite::setRotation(newRotation);
}

/*
 *  Sets the texture of the player using the specified primary and secondary textures,
 *  these are used in the player animation.
 *
 *  Parameters:
 *    primaryTexture: The primary texture for the player.
 *    secondaryTexture: The secondary texture for the player.
 */
void Player::setTexture(const sf::Texture& primaryTexture, const sf::Texture& secondaryTexture) {
    
    this->primaryTexture = &primaryTexture;
    this->secondaryTexture = &secondaryTexture;

    sf::Sprite::setTexture(primaryTexture);

    sf::FloatRect bounds = getLocalBounds();
    setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    wrappedSprite = this->clone();
}

Player::~Player() {
    delete fireRateInSeconds;
}


