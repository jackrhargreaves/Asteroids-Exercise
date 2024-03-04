#include "Projectile.h"

Projectile::Projectile(sf::Vector2f position, sf::Vector2f direction, float rotation, float speed, const sf::Texture& texture)
	: direction(direction), speed(speed), spawnPosition(position){

	setRotation(rotation);
	setPosition(position);
	setTexture(texture);
}

/*
 *  Updates the projectile's position and direction based on the selected target, if any.
 *
 *  Parameters:
 *    deltaTime: Time elapsed since the last update cycle.
 */
void Projectile::update(sf::Time deltaTime) {
    if (selectedTarget != nullptr) {
        if (!withinAimingCone(*selectedTarget)) {
            // Target moved out of the aiming cone, stop tracking
            selectedTarget = nullptr;
        }
        else {


            // Target is still within the aiming cone, continue tracking
            sf::Vector2f currentPosition = this->getPosition();
            sf::Vector2f targetPosition = selectedTarget->getPosition();
            sf::Vector2f targetDirection = targetPosition - currentPosition;
        

            float targetMagnitude = std::sqrt(targetDirection.x * targetDirection.x + targetDirection.y * targetDirection.y);
            sf::Vector2f normalizedTargetDirection = targetDirection / targetMagnitude;

            direction = direction + (normalizedTargetDirection - direction) * interpolationFactor;

            float directionMagnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction = direction / directionMagnitude;
        }
    }

    sf::Vector2f movement = direction * speed * deltaTime.asSeconds();
    sf::Sprite::move(movement);
}

/*
 *  Sets the target of the projectile.
 *
 *  Parameters:
 *    mob: The target mob to set.
 */
void Projectile::setTarget(Mob& mob) {

	selectedTarget = &mob;
}

/*
 *  Returns the target of the projectile.
 */
Mob* Projectile::getTarget() {

	return selectedTarget;
}

/*
 *  Checks if the given mob is within the aiming cone of the projectile.
 *
 *  Parameters:
 *    mob: The mob to check.
 *
 *  Returns:
 *    True if the mob is within the aiming cone, false otherwise.
 */
bool Projectile::withinAimingCone(Mob& mob) {
  

    if (!viewBounds) return false; 

    sf::Vector2f position = this->getPosition();
    float rotationInRadians = (this->getRotation() - 90) * (3.14159265f / 180.0f); 
    sf::Vector2f direction = sf::Vector2f(std::cos(rotationInRadians), std::sin(rotationInRadians));

    sf::Vector2f mobPos = mob.getPosition();
    if (!viewBounds->contains(mobPos)) {
        return false;
    }

    sf::Vector2f mobDir = mobPos - position;
    float mobDistanceSquared = mobDir.x * mobDir.x + mobDir.y * mobDir.y;

 
    sf::Vector2f normDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
    sf::Vector2f normMobDir = mobDir / std::sqrt(mobDistanceSquared);


    float dot = normDirection.x * normMobDir.x + normDirection.y * normMobDir.y;
    float angle = std::acos(dot);

    float coneWidth = aimAssistAngle * (3.14159265f / 180.0f);


    return angle <= coneWidth / 2.0f;
}


