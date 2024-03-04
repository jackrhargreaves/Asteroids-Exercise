#include "Asteroid.h"

Asteroid::Asteroid() {
	
}

void Asteroid::update(sf::Time deltaTime) {
  
	sf::Vector2f currentPosition = getPosition();
	setPosition(currentPosition + velocity * deltaTime.asSeconds());

}


/*
 *  Splits an asteroid from large->medium->small 
 *  Parameters:
 *    asteroids: The vector holding all new asteroids to be added this tick.
 */
void Asteroid::split(std::vector<Asteroid>& asteroids) {

    if (size > 0){
        for (int i = 0; i < 2; i++) {
            Asteroid newAsteroid;
            newAsteroid.setPosition(getPosition());
            newAsteroid.setTextureManager(*textureManager);


            if (textureManager) {
                if (size == 2) {
                    newAsteroid.setTexture(textureManager->getTexture("AsteroidHalf"));
                }
                else {
                    newAsteroid.setTexture(textureManager->getTexture("AsteroidOneFourth"));
                }
            }

            const float PI = 3.14159265358979323846f;

            float originalAngle = atan2(velocity.y, velocity.x);

            float angleDeviation = static_cast<float>((rand() % splitAngle - splitAngle / 2) * PI / 180.0);

            float newAngleRadians = originalAngle + angleDeviation;

            float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

            sf::Vector2f newVelocity;
            newVelocity.x = cos(newAngleRadians) * speed;
            newVelocity.y = sin(newAngleRadians) * speed;

            newAsteroid.setVelocity(newVelocity);
            newAsteroid.setBounds(*viewBounds);
            newAsteroid.setSize(getSize() - 1);

            asteroids.push_back(newAsteroid);
        }
    }
}

/*
 *  Gets the size of the asteroid: 2 (large), 1 (medium), 0 (small).
 */
int Asteroid::getSize() {
    return size;
}


/*
 *  Sets the size of the asteroid to the specified value (0 for small, 1 for medium, 2 for large).
 *  Parameters:
 *    newSize: The new size of the asteroid.
 */
void Asteroid::setSize(int newSize) {
    size = std::max(0, std::min(newSize, 2));
}