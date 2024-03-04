#pragma once
#include "Mob.h"

class Asteroid : public Mob {
public:
    Asteroid();

    virtual void update(sf::Time deltaTime) override;
    void setSize(int newSize);
    int getSize();
    void split(std::vector<Asteroid>& asteroids);

    Type type() const override { return Type::Asteroid; }

private:
    int size = 2;
    int splitAngle = 40;
};