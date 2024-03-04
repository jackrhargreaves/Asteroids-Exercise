#pragma once
#include <SFML/Graphics.hpp> 
#include <stdexcept> 
#include <string>
#include <iostream>
#include "TextureManager.h"

class Mob : public sf::Sprite {

public:
	Mob();
	virtual ~Mob() {}
	virtual void update(sf::Time deltaTime);
	void render(sf::RenderWindow& window);

	void setMoving(bool isMoving);
	void setTurning(bool isTurning);
	void rotate(float angle);
	void setPosition(const sf::Vector2f& position);
	void setPosition(float x, float y);
	void setBounds(const sf::FloatRect& bounds);
	void setTextureManager(TextureManager& manager);
	bool hasBounds() const;
	bool hasCollision() const;
	void printTextureInfo() const;
	void setTexture(const std::string& path);
	void setTexture(const sf::Texture& texture);
	void setVelocity(sf::Vector2f velocity);
	sf::FloatRect getBounds() const;
	sf::Sprite clone() const;

	sf::Texture texture;
	int width;
	int height;
	bool solid = true;
	sf::Sprite wrappedSprite;
	sf::FloatRect* viewBounds;
	TextureManager* textureManager;

	enum class Type { Base, Asteroid, EnemyShip, Player };
    virtual Type type() const { return Type::Base; }


private:
	int abs(double value);
	void drawRedBorder(sf::RenderWindow& window, const sf::FloatRect* viewBounds);


protected:
	sf::Vector2f velocity = sf::Vector2f(0.0f, 0.0f);
	bool moving = false;
	bool turning = false;
	int health;
	bool boundsSet = false;

};



