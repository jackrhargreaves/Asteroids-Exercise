#include "EnemyShip.h"

EnemyShip::EnemyShip(std::vector<Projectile>* projectiles, TextureManager* textureManager) {

    if (projectiles == nullptr || textureManager == nullptr) {
        throw std::invalid_argument("Projectiles or TextureManager pointer cannot be null");
    }

    this->projectiles = projectiles;
    this->textureManager = textureManager;

    projectileT = &textureManager->getTexture("Laser");
}

EnemyShip::~EnemyShip() {
    delete fireRateInSeconds; 
}

/*
 *  Updates enemy ship's position and firing behavior.
 *  Parameters:
 *    deltaTime: Time elapsed since the last update cycle.
 *    targetPosition: Position of the target.
 */
void EnemyShip::update(sf::Time deltaTime, sf::Vector2f targetPosition) {
    sf::Vector2f currentPosition = getPosition();

    sf::Vector2f direction = targetPosition - currentPosition;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        direction /= distance;
    }

    setPosition(currentPosition + velocity * deltaTime.asSeconds());


    timeSinceLastFire += deltaTime;

    const sf::Time fireRate = sf::seconds(*fireRateInSeconds);
    if (timeSinceLastFire >= fireRate) {
        fire(targetPosition);
        timeSinceLastFire = sf::Time::Zero; 
    }
}

/*
 *  Fires a projectile from the enemy ship towards the specified target position.
 *  Parameters:
 *    targetPosition: The position towards which the projectile should be fired, normally the player.
 */
void EnemyShip::fire(sf::Vector2f targetPosition) {
    sf::Vector2f position = getPosition();
    sf::Vector2f direction = targetPosition - position;
    

    // Normalise the direction vector
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance > 0) {
        direction /= distance;
    }


    float rotation = std::atan2(direction.y, direction.x) * (180.0f / 3.14159265f) + 90;

    Projectile newProjectile(position, direction, rotation, projectileSpeed, *projectileT); 

    if (viewBounds != nullptr) {
        newProjectile.setBounds(*viewBounds);
    }

    //Allows it to collide with the player
    newProjectile.targetPlayer = true;

    projectiles->push_back(newProjectile);
}