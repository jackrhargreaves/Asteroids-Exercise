#pragma once
#include "Mob.h"
#include "Projectile.h"
#include "Asteroid.h"

class Player : public Mob {
public:
    Player();
    virtual ~Player();

    void update(sf::Time deltaTime) override;

    void setAcceleration(const sf::Vector2f& newAcceleration);
    void setRotation(bool right);
    void fire(std::vector<Projectile>& projectiles, const sf::Texture& texture, std::vector<Mob*>& mobs);
    bool withinAimingCone(Mob& mob);
    Type type() const override { return Type::Player; }
    void debugAimingCone(sf::RenderTarget& target, float length);
    void setTexture(const sf::Texture& primaryTexture, const sf::Texture& secondaryTexture);

    double* fireRateInSeconds = new double(0.5);
    float rotationFactor = 8.0f;
    float primaryTextureDuration = 0.6f;
    float secondaryTextureDuration = 0.3f;
    const sf::Texture* primaryTexture;
    const sf::Texture* secondaryTexture;

private:
    float projectileSpeed = 1000.0f;
    sf::Vector2f acceleration = sf::Vector2f(0.0f, 0.0f);
    sf::Vector2f maxSpeed = sf::Vector2f(10000.0f, 10000.0f);
    float decelerationFactor = 0.95f;
    float accelerationFactor = 2.6f;
    sf::Time textureToggleTimer; 
    bool isPrimaryTextureActive = true; 
    float dragFactor = 1.7;

};