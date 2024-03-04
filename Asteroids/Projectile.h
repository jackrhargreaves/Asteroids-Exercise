#pragma once
#include "Mob.h"

class Projectile : public Mob {
public:
    Projectile(sf::Vector2f position, sf::Vector2f direction, float rotation, float speed, const sf::Texture& texture);

    virtual void update(sf::Time deltaTime) override;
    void setTarget(Mob& mob);
    Mob* getTarget();
    bool withinAimingCone(Mob& mob);

    bool targetPlayer = false;

private:
    sf::Vector2f direction;
    sf::Vector2f spawnPosition;
    float speed;
    float aimAssistAngle = 60.0f;
    float interpolationFactor = 1.05f;
    Mob* selectedTarget;

};