#pragma once
#include <SFML/Graphics.hpp>
#include "Mob.h"
#include "Player.h"
#include "Asteroid.h"
#include "Projectile.h"
#include "TextureManager.h"
#include "EnemyShip.h"

class GameLoop {
public:
    GameLoop();
    virtual ~GameLoop();
    void run();
    bool pixelPerfectCollision(const Mob sprite1, const Mob sprite2);
    bool boundingBoxCollision(const Mob& sprite1, const Mob& sprite2);


private:
    int width = 800;
    int height = 800;
    float zoomFactor = 0.5f;
    const float PI = 3.14159265358979323846f;

    sf::RenderWindow window;
    sf::View view;
    sf::Vector2f worldSize;
    sf::Clock clock;
    sf::FloatRect* viewBounds;
    sf::Vector2f center;

    Player player;
    Asteroid asteroid;

    bool isRunning;
    bool isCursorGrabbed;
    bool gameOver = false;
    bool paused = false;
    bool inGracePeriod;

    const float originalAspectRatio;
    sf::Font font;
   
    int score;
    int lives;

    sf::Text scoreText;
    sf::Text livesText;
    sf::Text pauseText;
    sf::Text gameOverText;

    std::vector<Projectile>* projectiles;
    std::vector<Mob*>* mobs;
    TextureManager* textureManager;

    const sf::Texture* asteroidFullT;
    const sf::Texture* asteroidHalfT;
    const sf::Texture* asteroidOneFourthT;
    const sf::Texture* shipT;
    const sf::Texture* ShipWithBurnerT;
    const sf::Texture* projectileT;
    const sf::Texture* enemyShipT;

    sf::Time gracePeriodDuration;
    sf::Time gracePeriodTimer;
    sf::Time asteroidSpawnTimer;
    sf::Time enemyShipSpawnTimer;

    void spawnMob(Mob::Type type);
    void processEvents(sf::Time deltaTime);
    void update(sf::Time deltaTime);
    void render();
    void renderGroup(std::vector<Mob>& group);
    void spawnAsteroid();
    bool isOnScreen(const Mob& mob);
    bool isFullyOnScreen(const Mob& mob);
    void reset(Mob& mob);
    void setUp();
    void removeLife();
};