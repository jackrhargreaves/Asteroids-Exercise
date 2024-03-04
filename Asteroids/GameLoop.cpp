#include "GameLoop.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib> 
#include <ctime> 
#include <cmath>

GameLoop::GameLoop()
    : window(sf::VideoMode(width, height), "Asteroids Exercise", sf::Style::Titlebar | sf::Style::Close),
    isRunning(true),
    isCursorGrabbed(true),
    originalAspectRatio(static_cast<float>(width) / static_cast<float>(height)) {
    textureManager = new TextureManager();


    //Load Textures
    textureManager->loadTexture("AsteroidFull", "Assets/Asteroid2.png", 10.0f, 10.0f); 
    textureManager->loadTexture("AsteroidHalf", "Assets/Asteroid2.png", 5.0f, 5.0f);
    textureManager->loadTexture("AsteroidOneFourth", "Assets/Asteroid2.png", 2.5f, 2.5f);
    textureManager->loadTexture("Laser", "Assets/Laser.png", 2.5f, 2.5f);
    textureManager->loadTexture("Ship", "Assets/Ship4.png", 5.0f, 5.0f);
    textureManager->rotateTexture("Ship", -45.0f);
    textureManager->loadTexture("ShipWithBurner", "Assets/Ship2.png", 5.0f, 5.0f);
    textureManager->rotateTexture("ShipWithBurner", -45.0f);
    textureManager->loadTexture("Projectile", "Assets/MissileBig.png", 1.5f, 1.5f);
    textureManager->loadTexture("EnemyShip", "Assets/EnemyShip.png", 5.0f, 5.0f);


    asteroidFullT = &textureManager->getTexture("AsteroidFull");
    asteroidHalfT = &textureManager->getTexture("AsteroidHalf");
    asteroidOneFourthT = &textureManager->getTexture("AsteroidOneFourth");
    shipT = &textureManager->getTexture("Ship");
    ShipWithBurnerT = &textureManager->getTexture("ShipWithBurner");
    projectileT = &textureManager->getTexture("Projectile");
    enemyShipT = &textureManager->getTexture("EnemyShip");

    player.setTexture(*ShipWithBurnerT, *shipT);


    //Set up vars
    gracePeriodDuration = sf::seconds(3.0);
    gracePeriodTimer = sf::Time::Zero;
    inGracePeriod = false;
    projectiles = new std::vector<Projectile>;
    mobs = new std::vector<Mob*>;
    worldSize = sf::Vector2f(static_cast<float>(width), static_cast<float>(height));
    center = sf::Vector2f(view.getCenter());
    srand(static_cast<unsigned int>(time(nullptr)));


    //Set up View and Mouse handling 
    view.setSize(worldSize.x / zoomFactor, worldSize.y / zoomFactor);
    window.setView(view);
    viewBounds = new sf::FloatRect(view.getCenter() - (view.getSize() / 2.f), view.getSize());
    sf::Vector2f viewTopLeft = view.getCenter() - (view.getSize() / 2.f);
    window.setMouseCursorGrabbed(isCursorGrabbed);
    window.setMouseCursorVisible(!isCursorGrabbed);
    player.setBounds(*viewBounds);
  


     //Set up Text
    if (!font.loadFromFile("Assets/HyperspaceBold.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    setUp();

    //Set up text overlay
    scoreText.setFont(font);
    scoreText.setCharacterSize(64);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(viewTopLeft.x + 40, viewTopLeft.y + 20);
    scoreText.setLetterSpacing(1.6);

    livesText.setFont(font);
    livesText.setCharacterSize(64);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(viewTopLeft.x + 40, viewTopLeft.y + 100);
    livesText.setLetterSpacing(1.6);

    pauseText.setFont(font);
    pauseText.setCharacterSize(64);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setLetterSpacing(1.6);
    pauseText.setString("PAUSED");
    sf::FloatRect pauseTextBounds = pauseText.getLocalBounds();
    pauseText.setOrigin(pauseTextBounds.width / 2.0f, pauseTextBounds.height / 2.0f);
    pauseText.setPosition(center);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(64);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setLetterSpacing(1.6);
    gameOverText.setString("GAME OVER");
    sf::FloatRect gameOverTextBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gameOverTextBounds.width / 2.0f, gameOverTextBounds.height / 2.0f);
    gameOverText.setPosition(center);



}

GameLoop::~GameLoop() {

    
    delete viewBounds;
    delete projectiles;
    delete textureManager;
    //The texture pointers are deleted with textureManager

    for (Mob* mob : *mobs) {
        delete mob;
    }

    delete mobs;
}
/*
*  Main game loop that handles the core update and rendering cycle.
*  It maintains a fixed update rate for game logic(UPS - Updates Per Second)
*  and an uncapped frame rate for rendering.
*
*  The game loop uses a time accumulator to ensure consistent updates
*  for more stable gameplay logic.
*/
void GameLoop::run() {
    sf::Time lastTime = clock.getElapsedTime();
    sf::Time timer = clock.getElapsedTime();
    double delta = 0.0;
    const double ns = 1.0 / 60.0; // 60 updates per second
    int frames = 0;
    int updates = 0;

    while (isRunning && window.isOpen()) {
        sf::Time now = clock.getElapsedTime();
        delta += (now - lastTime).asSeconds() / ns;
        lastTime = now;

        while (delta >= 1) {

            static sf::Time lastUpdate = now;
            sf::Time deltaTime = now - lastUpdate;
            lastUpdate = now;

            processEvents(deltaTime);
            update(deltaTime);
            updates++;
            delta--;
        }

        render(); //Uncapped FPS
        frames++;

        if (clock.getElapsedTime().asSeconds() - timer.asSeconds() > 1.0) {
            timer += sf::seconds(1.0);
            std::string title = "Asteroids Exercise | " + std::to_string(updates) + " UPS, " + std::to_string(frames) + " FPS";
            window.setTitle(title);
            updates = 0;
            frames = 0;
        }
    }
}

/*
 *  Checks for pixel-perfect collision between two Mobs using their textures.
 *  This function first performs a bounding box collision check. If the bounding
 *  boxes intersect, it then checks each pixel within the intersecting area for
 *  transparency. A collision is detected if two opaque pixels (above a certain
 *  alpha threshold) overlap.
 *
 *  Parameters:
 *    sprite1: The first Mob involved in the collision check.
 *    sprite2: The second Mob involved in the collision check.
 *
 *  Returns:
 *    bool: True if a pixel-perfect collision is detected, false otherwise.
 *
 *  Note:
 *    This function can be performance-intensive, especially with larger sprites/textures
 *    or complex scenes, due to its per-pixel checks.
 */
bool GameLoop::pixelPerfectCollision(const Mob sprite1, const Mob sprite2) {
   
    const unsigned int ALPHA_THRESHOLD = 50;

    if (!sprite1.hasCollision() || !sprite2.hasCollision()) {
        return false; 
    }

    if (!sprite1.getTexture() || !sprite2.getTexture()) {
        return false; 
    }

    sf::FloatRect intersection;
    if (!sprite1.getBounds().intersects(sprite2.getBounds(), intersection)) {
        return false; 
    }

    sf::IntRect rect1 = sprite1.getTextureRect();
    sf::IntRect rect2 = sprite2.getTextureRect();

    const sf::Image image1 = sprite1.getTexture()->copyToImage();
    const sf::Image image2 = sprite2.getTexture()->copyToImage();

    sf::Transform transform1 = sprite1.getInverseTransform();
    sf::Transform transform2 = sprite2.getInverseTransform();

    for (int i = intersection.left; i < intersection.left + intersection.width; i++) {
        for (int j = intersection.top; j < intersection.top + intersection.height; j++) {
            sf::Vector2f point1 = transform1.transformPoint(i, j);
            sf::Vector2i texturePoint1(point1.x - rect1.left, point1.y - rect1.top);

            sf::Vector2f point2 = transform2.transformPoint(i, j);
            sf::Vector2i texturePoint2(point2.x - rect2.left, point2.y - rect2.top);

            if (rect1.contains(texturePoint1) && rect2.contains(texturePoint2)) {
                sf::Color color1 = image1.getPixel(texturePoint1.x, texturePoint1.y);
                sf::Color color2 = image2.getPixel(texturePoint2.x, texturePoint2.y);

            
                if (color1.a > ALPHA_THRESHOLD && color2.a > ALPHA_THRESHOLD) {
                    return true; // Collision detected
                }
            }
        }
    }

    return false; // No collision detected
}

/*
 *  Checks for a bounding box collision between two Mobs. This method is a
 *  simpler and less computationally intensive approach compared to pixel-perfect
 *  collision detection. It's based on comparing the axis-aligned bounding boxes
 *  (AABB) of each sprite to see if they intersect.
 *
 *  Parameters:
 *    sprite1: Reference to the first Mob involved in the collision check.
 *    sprite2: Reference to the second Mob involved in the collision check.
 *
 *  Returns:
 *    bool: True if the bounding boxes of the two Mobs intersect, indicating a collision.
 *          False if the Mobs do not intersect or if either Mob is set to not have collisions.
 */
bool GameLoop::boundingBoxCollision(const Mob& sprite1, const Mob& sprite2) {

    if (!sprite1.hasCollision() || !sprite2.hasCollision()) {
        return false;
    }

    sf::FloatRect bounds1 = sprite1.getBounds();
    sf::FloatRect bounds2 = sprite2.getBounds();

    return bounds1.intersects(bounds2);
}


/*
 *  Processes all events polled from the SFML window within the game loop. This function
 *  handles various types of events including window close, window resize, and user inputs
 *  like keyboard and mouse events. Based on the event type, appropriate actions are taken
 *  such as closing the game window, resizing the game view, or updating the player's state
 *  with actions like firing, moving, and rotating.
 *
 *  Parameters:
 *    deltaTime: Represents the time elapsed since the last update cycle in the game loop.
 *               It's used to ensure consistent gameplay mechanics, such as regulating the
 *               fire rate of the player's ship, independent of the frame rate.
 *
 *  Key Features:
 *    - Window Close: Triggers the game window to close upon receiving a close event.
 *    - Window Resize: Adjusts the view to maintain the original aspect ratio following a window resize event.
 *    - Escape Key: Utilised to toggle the state of cursor grabbing and to pause the game.
 *    - Mouse Button Press: Engages cursor grabbing and resumes gameplay if previously paused.
 *    - Space Key: Enables the player to fire projectiles, subject to the fire rate constraint dictated by deltaTime.
 *    - W/A/D Keys: Facilitate player movement and rotation.
 *
 *  Note:
 *    Player interactions are contingent upon the game being active (not paused or over) and the cursor being captured.
 *    This mechanism serves as the fundamental pause functionality and state management within the game.
 */
void GameLoop::processEvents(sf::Time deltaTime) {

    static double* fireRateInSeconds = player.fireRateInSeconds;

    static sf::Time timeSinceLastFire = sf::seconds(*fireRateInSeconds);
    const sf::Time fireRate = sf::seconds(*fireRateInSeconds);

    sf::Event event;
    while (window.pollEvent(event)) {

        switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;

        case sf::Event::Resized:
            break;

        }

        if (event.type == sf::Event::Resized) {

            // Get the new window size
            float newWidth = event.size.width;
            float newHeight = event.size.height;

            // Calculate the new aspect ratio
            float newAspectRatio = newWidth / newHeight;
            float originalAspectRatio = worldSize.x / worldSize.y;

            sf::Vector2f newSize;

            if (newAspectRatio > originalAspectRatio) {
            
                newSize.y = worldSize.y / zoomFactor;
                newSize.x = (worldSize.y / zoomFactor) * newAspectRatio;
            }
            else {
           
                newSize.x = worldSize.x / zoomFactor;
                newSize.y = (worldSize.x / zoomFactor) / newAspectRatio;
            }

            // Set the new size for the view
            view.setSize(newSize);

     
            view.setCenter(worldSize.x / 2, worldSize.y / 2);

      
            window.setView(view);

            *viewBounds = sf::FloatRect(view.getCenter() - view.getSize() / 2.f, view.getSize());

        }
 
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            isCursorGrabbed = false;
            paused = true;
            window.setMouseCursorGrabbed(isCursorGrabbed);
            window.setMouseCursorVisible(!isCursorGrabbed);
        }

        if (event.type == sf::Event::MouseButtonPressed && !isCursorGrabbed) {
            isCursorGrabbed = true;
            paused = false;
            window.setMouseCursorGrabbed(isCursorGrabbed);
            window.setMouseCursorVisible(!isCursorGrabbed);
        }


        if (isCursorGrabbed) {
            if(gameOver == false && paused == false) {
                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    if (timeSinceLastFire >= fireRate) {
                        player.fire(*projectiles, *projectileT, *mobs);
                        timeSinceLastFire = sf::Time::Zero;
                    }
                }

                sf::Vector2f acceleration(0.0f, 0.0f);
                bool isMoving = false;
                bool isTurning = false;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                    acceleration.y -= 300.0f; // Move up
                    isMoving = true;
                }


                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                    player.setRotation(false); //Rotate left
                    isTurning = true;
                }


                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                    player.setRotation(true); //Rotate right
                    isTurning = true;
                }

                player.setAcceleration(acceleration);
                player.setMoving(isMoving);
                player.setTurning(isTurning);
            
             } else if (gameOver) {

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

                    setUp();
                }
             }
        }

    } 

    timeSinceLastFire += deltaTime;
}

/*
 *  Updates the game state based on the time elapsed since the last frame (deltaTime).
 *  This includes processing player inputs, updating mob positions, checking for collisions,
 *  and managing game events like spawning asteroids and enemy ships. The function also
 *  handles the game's pause state and the grace period following a collision.
 *
 *  Parameters:
 *    deltaTime: Time elapsed since the last update, used for time-dependent calculations
 *               like movement, spawning intervals, and the grace period duration.
 *
 *  Key Actions:
 *    - Score Update: Updates the score display based on the current score.
 *    - Pause Check: Returns immediately if the game is paused or over, halting updates.
 *    - Grace Period Handling: Manages the invulnerability period after the player collides with a mob.
 *    - Player Update: Processes player movements and actions based on user inputs.
 *    - Mob Spawning: Periodically spawns asteroids and enemy ships based on timers.
 *    - Mob Updates: Updates all mobs, including enemy ships with special logic for targeting the player.
 *    - Collision Detection: Checks for collisions between the player, mobs, and projectiles,
 *                           applying game logic like splitting asteroids and removing lives.
 *    - Projectile Management: Updates projectile positions, removes off-screen projectiles,
 *                             and handles collisions with mobs or the player.
 *
 *  Note:
 *    The function ensures that all game logic is processed in a consistent manner, adhering
 *    to the fixed time step defined by deltaTime. This approach provides a stable and uniform
 *    gameplay experience across different hardware.
 */
void GameLoop::update(sf::Time deltaTime) {


    scoreText.setString("Score: " + std::to_string(score));

    if (isCursorGrabbed) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
    }

    //Stop Updates
    if (gameOver || paused) {
        return;
    }

    if (inGracePeriod) {
        gracePeriodTimer += deltaTime;
        if (gracePeriodTimer >= gracePeriodDuration) {
            inGracePeriod = false;
            gracePeriodTimer = sf::Time::Zero;
        }
    }

    player.update(deltaTime);

    asteroidSpawnTimer += deltaTime;
    enemyShipSpawnTimer += deltaTime;

    if (asteroidSpawnTimer.asSeconds() >= 7) {
        spawnMob(Mob::Type::Asteroid);
        asteroidSpawnTimer = sf::Time::Zero;
    }

    if (enemyShipSpawnTimer.asSeconds() >= 25) {
        spawnMob(Mob::Type::EnemyShip);
        enemyShipSpawnTimer = sf::Time::Zero;
    }

    sf::Vector2f playerPos = player.getPosition();

    for (auto& mob : *mobs) {

        if (mob->type() == Mob::Type::EnemyShip) {
            EnemyShip* enemyShip = dynamic_cast<EnemyShip*>(mob);
            if (enemyShip) {
                enemyShip->update(deltaTime, playerPos);
            }
        } else {
            mob->update(deltaTime);
        }

        if (!mob->hasBounds() && isFullyOnScreen(*mob)) {
            mob->setBounds(*viewBounds);
        }
    }

    std::vector<Asteroid> newAsteroids;

    for (auto mobIt = mobs->begin(); mobIt != mobs->end();) {
      
        if (!inGracePeriod) {

            bool collided = pixelPerfectCollision(player, **mobIt);

            if (collided) {

                removeLife();


                if ((*mobIt)->type() == Mob::Type::Asteroid) {
                    Asteroid* asteroid = dynamic_cast<Asteroid*>(*mobIt); 
                    if (asteroid) {
                        asteroid->split(newAsteroids);
                    }
                }

                delete* mobIt; 

                mobIt = mobs->erase(mobIt);

            }
            else {
                ++mobIt;
            }
        }  else {
            ++mobIt;
        }
    }


    for (auto projectileIt = projectiles->begin(); projectileIt != projectiles->end();) {
        projectileIt->update(deltaTime);

        auto projectilePos = projectileIt->getPosition();
        bool offScreen = projectilePos.x < viewBounds->left || projectilePos.x > viewBounds->left + viewBounds->width ||
            projectilePos.y < viewBounds->top || projectilePos.y > viewBounds->top + viewBounds->height;

        if (offScreen) {
            projectileIt = projectiles->erase(projectileIt);
        }
        else {
            bool collided = false;
            if (projectileIt->targetPlayer) {
                if (!inGracePeriod) {
        
                    if (pixelPerfectCollision(*projectileIt, player)) {
         
                        removeLife();

      
                        projectileIt = projectiles->erase(projectileIt);
                        collided = true;

               
                    }
                }
            }
            else {
         
                for (auto mobIt = mobs->begin(); mobIt != mobs->end() && !collided;) {
                    if (pixelPerfectCollision(*projectileIt, **mobIt)) {
                        score += 100; 
                        projectileIt = projectiles->erase(projectileIt);

              
                        if ((*mobIt)->type() == Mob::Type::Asteroid) {
                            Asteroid* asteroid = dynamic_cast<Asteroid*>(*mobIt);
                            if (asteroid) {
                                asteroid->split(newAsteroids);  
                            }
                        }

                        // Remove the Mob object
                        delete* mobIt;  
                        mobIt = mobs->erase(mobIt);

                        collided = true;
                    }
                    else {
                        ++mobIt;
                    }
                }
            }

            if (!collided) {
                ++projectileIt;
            }
        }
    }

    for (const auto& asteroid : newAsteroids) {
        Asteroid* newAsteroidPtr = new Asteroid(asteroid);
        mobs->push_back(newAsteroidPtr);
    }
}

/*
 *  Renders game elements including the player, projectiles, mobs, and UI texts to the window.
 *  Applies a flashing effect to the player during the grace period and handles special
 *  renderings for game over and paused states.
 *
 *  - Clears the window and sets the view for rendering.
 *  - Renders the player, with flashing during the grace period.
 *  - Iterates and renders all projectiles and mobs.
 *  - Displays score, lives (or game over message), and pause message as applicable.
 *  - Finalises rendering by displaying all drawn elements.
 */
void GameLoop::render() {
    window.clear(sf::Color::Black);
    window.setView(view);

   // asteroid.render(window);

    float flashInterval = 1.0f / 3.0f;

    
    if (paused || !inGracePeriod || ((int)(gracePeriodTimer.asSeconds() / flashInterval)) % 2 == 0) {
        player.render(window);
       //player.debugAimingCone(window, 200.0f);
    }

    for (auto& projectile : *projectiles) {
       projectile.render(window);
    }

    for (auto& mob : *mobs) {
        mob->render(window);
    }

    window.draw(scoreText);

    if(!gameOver) {
        window.draw(livesText);

        if (paused) {
            window.draw(pauseText);
        }
    }
    else {
        window.draw(gameOverText);
    }

 

 
    window.display();
}

/*
 *  Efficiently renders a group of Mobs using a vertex array. This method assumes all Mobs in the group
 *  share the same texture. It constructs a quad for each Mob and sets texture coordinates accordingly,
 *  allowing for batch rendering which can improve performance.
 *
 *  - Checks if the group is not empty to proceed with rendering.
 *  - Retrieves the shared texture from the first Mob in the group.
 *  - Initialises a vertex array as Quads to represent each Mob.
 *  - Iterates over the group, setting vertex positions and texture coordinates for each Mob.
 *  - Draws the entire group in a single draw call using the shared texture.
 *
 *  This method is particularly useful for rendering large numbers of similar objects.
 */
void GameLoop::renderGroup(std::vector<Mob>& group) {

    if (!group.empty()) {

        // All in the group share the same texture
        const sf::Texture* texture = group.at(0).getTexture();

        // Create a vertex array with quads, 4 vertices for each in the group
        sf::VertexArray vertices(sf::Quads, group.size() * 4);

        for (std::size_t i = 0; i < group.size(); ++i) {
            const auto& proj = group.at(i);

            const sf::FloatRect bounds = proj.getBounds();
            const sf::IntRect texRect = proj.getTextureRect();

            // Calculate the 4 corners of the quad for an item in the group
            vertices[i * 4 + 0].position = sf::Vector2f(bounds.left, bounds.top);
            vertices[i * 4 + 1].position = sf::Vector2f(bounds.left + bounds.width, bounds.top);
            vertices[i * 4 + 2].position = sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height);
            vertices[i * 4 + 3].position = sf::Vector2f(bounds.left, bounds.top + bounds.height);

            // Set the texture coordinates for each corner of the quad
            vertices[i * 4 + 0].texCoords = sf::Vector2f(texRect.left, texRect.top);
            vertices[i * 4 + 1].texCoords = sf::Vector2f(texRect.left + texRect.width, texRect.top);
            vertices[i * 4 + 2].texCoords = sf::Vector2f(texRect.left + texRect.width, texRect.top + texRect.height);
            vertices[i * 4 + 3].texCoords = sf::Vector2f(texRect.left, texRect.top + texRect.height);
        }


        window.draw(vertices, texture);
    }
}

/*
 *  Checks if a given Mob is within the current screen view.
 *
 *  Parameters:
 *    mob: The Mob to check for partial visibility.
 *
 *  Returns:
 *    bool: True if the Mob's bounding box intersects with the view bounds, indicating it's on screen.
 *          False otherwise.
 */
bool GameLoop::isOnScreen(const Mob& mob) {
    return viewBounds->intersects(mob.getBounds());
}

/*
 *  Determines if a Mob is completely within the current screen view, without any part
 *  of it extending beyond the view boundaries.
 *
 *  Parameters:
 *    mob: The Mob to check for complete visibility.
 *
 *  Returns:
 *    bool: True if the entire Mob is within the view bounds, ensuring no part is off-screen.
 *          False if any part of the Mob extends beyond the view boundaries.
 *
 *  This function is useful for determining whether a Mob should have it's viewBounds set,
 *  making sure it doesn't warp after spawning
 */
bool GameLoop::isFullyOnScreen(const Mob& mob) {
    sf::FloatRect mobBounds = mob.getBounds(); 

    sf::Vector2f mobCenter = mob.getPosition();  
    mobBounds.left = mobCenter.x - mobBounds.width / 2.0f;
    mobBounds.top = mobCenter.y - mobBounds.height / 2.0f;

    bool isLeftEdgeInside = mobBounds.left >= viewBounds->left;
    bool isRightEdgeInside = mobBounds.left + mobBounds.width <= viewBounds->left + viewBounds->width;
    bool isTopEdgeInside = mobBounds.top >= viewBounds->top;
    bool isBottomEdgeInside = mobBounds.top + mobBounds.height <= viewBounds->top + viewBounds->height;

    return isLeftEdgeInside && isRightEdgeInside && isTopEdgeInside && isBottomEdgeInside;
}


/*
 *  Spawns a new Mob of the specified type at a random location just outside the view boundaries,
 *  with a randomised velocity and direction. The spawn location is determined by randomly selecting
 *  one of the four edges of the screen (top, right, bottom, left).
 *
 *  Parameters:
 *    type: The type of Mob to spawn, defined by the Mob::Type enumeration.
 *
 *  Functionality:
 *    - Randomly selects a spawn side and calculates a corresponding start position just off-screen.
 *    - Sets a base speed and applies a random angle deviation to give the Mob a varied velocity.
 *    - Initialises the Mob based on its type, sets its texture, and adds it to the mobs vector.
 */
void GameLoop::spawnMob(Mob::Type type) {

    int side = rand() % 4; // 0 = top, 1 = right, 2 = bottom, 3 = left
    sf::Vector2f startPosition;
    sf::Vector2f velocity;

    float baseSpeed = 500.0f;

    // Angle deviation within ±80 degrees (in radians)
    float angleDeviation = static_cast<float>((rand() % 160 - 80) * PI / 180.0);

    Mob* newMob = nullptr;

    // Initialise the new mob based on its type
    switch (type) {
    case Mob::Type::Asteroid:
        newMob = new Asteroid();
        newMob->setTexture(*asteroidFullT);
        break;
    case Mob::Type::EnemyShip:
        newMob = new EnemyShip(projectiles, textureManager);
        newMob->setTexture(*enemyShipT);
        break;
    default:
        std::cerr << "Unsupported Mob Type" << std::endl;
        return; // Early return if the type is not supported
    }

    newMob->setTextureManager(*textureManager);

    sf::FloatRect mobBounds = newMob->getBounds();

    // Determine startPosition and velocity based on the chosen side
    switch (side) {
    case 0: // Top
        startPosition.x = viewBounds->left + static_cast<float>(rand() % static_cast<int>(viewBounds->width));
        startPosition.y = viewBounds->top - mobBounds.height / 2; 
        velocity = sf::Vector2f(rand() % 200 - 100, baseSpeed); // Random horizontal component, fixed downward component
        break;
    case 1: // Right
        startPosition.x = viewBounds->left + viewBounds->width + mobBounds.width / 2; 
        startPosition.y = viewBounds->top + static_cast<float>(rand() % static_cast<int>(viewBounds->height));
        velocity = sf::Vector2f(-baseSpeed, rand() % 200 - 100); // Fixed leftward component, random vertical component
        break;
    case 2: // Bottom
        startPosition.x = viewBounds->left + static_cast<float>(rand() % static_cast<int>(viewBounds->width));
        startPosition.y = viewBounds->top + viewBounds->height + mobBounds.height / 2; 
        velocity = sf::Vector2f(rand() % 200 - 100, -baseSpeed); // Random horizontal component, fixed upward component
        break;
    case 3: // Left
        startPosition.x = viewBounds->left - mobBounds.width / 2; 
        startPosition.y = viewBounds->top + static_cast<float>(rand() % static_cast<int>(viewBounds->height));
        velocity = sf::Vector2f(baseSpeed, rand() % 200 - 100); // Fixed rightward component, random vertical component
        break;
    }

    float velMagnitude = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float velAngle = std::atan2(velocity.y, velocity.x) + angleDeviation;
    velocity = sf::Vector2f(std::cos(velAngle) * velMagnitude, std::sin(velAngle) * velMagnitude);

    newMob->setPosition(startPosition);
    newMob->setVelocity(velocity);
    mobs->push_back(newMob);
}

/*
 *  Resets the specified Mob to its initial state by repositioning it to the centre of the game area,
 *  nullifying its velocity, and setting its rotation to zero. This function is typically invoked
 *  following events such as player loses a life or game restarts to ensure a consistent starting point.
 *
 *  Parameters:
 *    mob: Reference to the Mob object that requires reinitialisation.
 *
 *  Functionality:
 *    - Repositions the Mob to the central point of the play area.
 *    - Sets the Mob's velocity to a standstill (0.0f, 0.0f) to halt any movement.
 *    - Resets the Mob's orientation by setting its rotation angle to zero.
 */
void GameLoop::reset(Mob& mob) {
    mob.setPosition(center);
    mob.setVelocity(sf::Vector2f(0.0f, 0.0f));
    mob.setRotation(0.0f);
}


/*
 *  Prepares the game for a new session by resetting stats, clearing entities,
 *  and spawning initial mobs. Used at game start or after a game over.
 */
void GameLoop::setUp() {
    gameOver = false;
    paused = false;
    score = 0;
    lives = 3; 
    livesText.setString("Lives: " + std::to_string(lives));
    scoreText.setString("Score: 0");
    projectiles->clear(); 
    mobs->clear();
    reset(player);

    asteroidSpawnTimer = sf::Time::Zero;
    enemyShipSpawnTimer = sf::Time::Zero;

    for (int i = 0; i < 2; i++) {
        spawnMob(Mob::Type::Asteroid);
        spawnMob(Mob::Type::EnemyShip);
    }

   
}

/*
 *  Decreases player lives and updates display. Sets game over if no lives left,
 *  resets player and initiates grace period to prevent immediate collisions.
 */
void GameLoop::removeLife() {
    lives -= 1;
    livesText.setString("Lives: " + std::to_string(lives));


    if (lives <= 0) {
        gameOver = true;
        return;
    }

    reset(player);
    inGracePeriod = true;
    gracePeriodTimer = sf::Time::Zero;
}