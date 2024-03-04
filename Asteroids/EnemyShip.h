#pragma once
#include "Mob.h"
#include "Projectile.h"

class EnemyShip : public Mob {
public:

    EnemyShip(std::vector<Projectile>* projectiles, TextureManager* textureManager);
    ~EnemyShip();
    void update(sf::Time deltaTime, sf::Vector2f targetPosition);
    Type type() const override { return Type::EnemyShip; }
    float projectileSpeed = 400.0f;


private:

    void fire(sf::Vector2f targetPosition);

    std::vector<Projectile>* projectiles;
    double* fireRateInSeconds = new double(2.0); 
    sf::Time timeSinceLastFire;
    const sf::Texture* projectileT;

};