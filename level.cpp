// ============================================================================
// Level.cpp - Complete Level Implementation
// Includes Desert Level (Level 1) and Ice Level (Level 2)
// ============================================================================

#include "level.h"
#include "camera.h" // Added for camera shake
#include <cstdio>
#include <cstdlib>

#define PI 3.14159265359f

// ============================================================================
// BASE LEVEL CLASS
// ============================================================================

// Initialize static members
Model *Level::pillarModel = nullptr;
Model *Level::snowmanModel = nullptr;
Model *Level::christmasTreeModel = nullptr;
Model *Level::snakeModel = nullptr;
Model *Level::trapModel = nullptr;
Model *Level::chestModel = nullptr;

void Level::cleanupCommonAssets() {
  if (pillarModel) {
    delete pillarModel;
    pillarModel = nullptr;
  }
  if (snowmanModel) {
    delete snowmanModel;
    snowmanModel = nullptr;
  }
  if (christmasTreeModel) {
    delete christmasTreeModel;
    christmasTreeModel = nullptr;
  }
  if (snakeModel) {
    delete snakeModel;
    snakeModel = nullptr;
  }
  if (trapModel) {
    delete trapModel;
    trapModel = nullptr;
  }
  if (chestModel) {
    delete chestModel;
    chestModel = nullptr;
  }
}

Level::Level() {
  player = nullptr;
  portal = nullptr;
  levelComplete = false;
  isExiting = false;
  exitTimer = 0.0f;
}

Level::~Level() {
  if (portal)
    delete portal;
  for (auto c : collectibles)
    delete c;
  for (auto e : enemies)
    delete e;
  for (auto t : traps)
    delete t;
  for (auto o : obstacles)
    delete o;
  for (auto t : torches)
    delete t;

  // Instance models are deleted here
  if (treeModel) {
    delete treeModel;
    treeModel = nullptr;
  }
  if (rockModel) {
    delete rockModel;
    rockModel = nullptr;
  }
  if (groundModel) {
    delete groundModel;
    groundModel = nullptr;
  }
  if (cactusModel) {
    delete cactusModel;
    cactusModel = nullptr;
  }

  // STATIC models (pillarModel, etc.) are managed by cleanupCommonAssets()
  // Do NOT delete them here, or they will be double-freed on restart
}

void Level::loadCommonAssets() {
  // Initialize models if they don't exist
  if (!pillarModel) {
    pillarModel = new Model();
    pillarModel->load("assets/pillar.obj");
  }
  if (!snowmanModel) {
    snowmanModel = new Model();
    snowmanModel->load("assets/snowman.obj");
  }
  if (!christmasTreeModel) {
    christmasTreeModel = new Model();
    christmasTreeModel->load("assets/christmasTree.obj");
  }
  if (!snakeModel) {
    snakeModel = new Model();
    snakeModel->load("assets/snake.obj");
  }
  if (!trapModel) {
    trapModel = new Model();
    trapModel->load("assets/traps.obj");
  }
  if (!chestModel) {
    chestModel = new Model();
    chestModel->load("assets/chest.obj");
  }

  // Load non-static models
  treeModel = new Model();
  rockModel = new Model();
  groundModel = new Model();
  cactusModel = new Model();

  treeModel->load("assets/tree.obj");
  if (!rockModel->load("assets/rock.obj")) {
    printf("Failed to load rock model\n");
  }
  if (!groundModel->load("assets/ground.obj")) {
    printf("Failed to load ground model\n");
  }
  cactusModel->load("assets/cactus.obj");

  // Load textures
  wallTexture = loadBMP("assets/wall.bmp");
  groundTexture = loadBMP("assets/ground.bmp");
}

void Level::renderGround(float size, Texture &texture) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture.id);
  glEnable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 1.0f); // White to show texture colors

  // Always use simple quad (no model) for smooth ground
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-size, 0, -size);
  glTexCoord2f(10, 0);
  glVertex3f(size, 0, -size);
  glTexCoord2f(10, 10);
  glVertex3f(size, 0, size);
  glTexCoord2f(0, 10);
  glVertex3f(-size, 0, size);
  glEnd();
}

void Level::renderSkybox(float r, float g, float b) {
  glDisable(GL_LIGHTING);
  glColor3f(r, g, b);

  float size = 200.0f;
  glBegin(GL_QUADS);
  // Back
  glVertex3f(-size, 0, -size);
  glVertex3f(size, 0, -size);
  glVertex3f(size, size, -size);
  glVertex3f(-size, size, -size);

  // Left
  glVertex3f(-size, 0, size);
  glVertex3f(-size, 0, -size);
  glVertex3f(-size, size, -size);
  glVertex3f(-size, size, size);

  // Right
  glVertex3f(size, 0, -size);
  glVertex3f(size, 0, size);
  glVertex3f(size, size, size);
  glVertex3f(size, size, -size);

  // Front
  glVertex3f(size, 0, size);
  glVertex3f(-size, 0, size);
  glVertex3f(-size, size, size);
  glVertex3f(size, size, size);

  // Top
  glColor3f(r * 0.8f, g * 0.8f, b * 1.2f);
  glVertex3f(-size, size, -size);
  glVertex3f(size, size, -size);
  glVertex3f(size, size, size);
  glVertex3f(-size, size, size);
  glEnd();
  glEnable(GL_LIGHTING);
}

void Level::renderWalls(float size, float height, Texture &texture) {
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture.id);

  glColor3f(1.0f, 1.0f, 1.0f);

  float thickness = 1.0f;

  // North wall
  glPushMatrix();
  glTranslatef(0, height / 2, -size);
  glScalef(size * 2, height, thickness);
  glutSolidCube(1.0f);
  glPopMatrix();

  // South wall
  glPushMatrix();
  glTranslatef(0, height / 2, size);
  glScalef(size * 2, height, thickness);
  glutSolidCube(1.0f);
  glPopMatrix();

  // West wall
  glPushMatrix();
  glTranslatef(-size, height / 2, 0);
  glScalef(thickness, height, size * 2);
  glutSolidCube(1.0f);
  glPopMatrix();

  // East wall
  glPushMatrix();
  glTranslatef(size, height / 2, 0);
  glScalef(thickness, height, size * 2);
  glutSolidCube(1.0f);
  glPopMatrix();
}

// ============================================================================
// DESERT LEVEL IMPLEMENTATION
// ============================================================================

DesertLevel::DesertLevel() : Level() {
  totalOrbs = 5;
  timeOfDay = 0.0f;
  daySpeed = 0.05f;
  levelTimer = 120.0f; // 2 minutes
  maxTime = 120.0f;
}

DesertLevel::~DesertLevel() {
  for (auto c : chests)
    delete c;
}

void DesertLevel::init(Player *p) {
  player = p;
  levelComplete = false;
  levelTimer = maxTime; // Reset timer

  // Setup lighting
  sunLight.position = {0, 100, -80, 1}; // High up and visible
  sunLight.ambient = {0.5f, 0.5f, 0.5f, 1.0f};
  sunLight.diffuse = {1.0f, 0.9f, 0.8f, 1.0f}; // Warm sunlight
  sunLight.specular = {1.0f, 1.0f, 1.0f, 1.0f};

  // Set Normal Physics (High acceleration, High friction)
  // Faster movement as requested
  player->setPhysics(80.0f, 10.0f, 11.0f);

  // Spawn level elements
  spawnOrbs();
  spawnChests();
  spawnEnemies();
  spawnEnemies();
  spawnObstacles();

  // Spawn Torches (New Feature)
  torches.clear();
  // Portal Gate Torches (At Z = -80)
  torches.push_back(new Torch(-8, 2, -78));
  torches.push_back(new Torch(8, 2, -78));
  // Mid-way Torches
  torches.push_back(new Torch(15, 2, 0));
  torches.push_back(new Torch(-15, 2, 0));
  // Start Area Torches
  torches.push_back(new Torch(15, 2, 60));
  torches.push_back(new Torch(-15, 2, 60));

  // Create portal
  portal = new Portal(0, 1, -80);

  loadCommonAssets();

  // Load desert textures
  sandTexture = loadBMP("assets/sand_ground.bmp");
  desertWallTexture = loadBMP("assets/sandstone_wall.bmp");
}

void DesertLevel::spawnOrbs() {
  collectibles.clear();

  // Place 5 orbs in different locations
  collectibles.push_back(new Collectible(15, 2, 10));
  collectibles.push_back(new Collectible(-15, 2, -10));
  collectibles.push_back(new Collectible(20, 2, -20));
  collectibles.push_back(new Collectible(-10, 2, 15));
  collectibles.push_back(new Collectible(10, 2, -30));
}

void DesertLevel::spawnChests() {
  chests.clear();

  // Some chests have orbs, some don't
  chests.push_back(new Chest(25, 0.5f, 0, true, false));
  chests.push_back(new Chest(-20, 0.5f, 20, false, true)); // Coins!
  chests.push_back(new Chest(0, 0.5f, 25, true, false));
  chests.push_back(new Chest(30, 0.5f, -15, false, true)); // Coins!
}

void DesertLevel::spawnEnemies() {
  enemies.clear();

  // Scorpion enemies with patrol routes
  Enemy *scorpion1 = new Enemy(10, 0.5f, 0);
  scorpion1->patrolPoints.push_back({10, 0.5f, 0});
  scorpion1->patrolPoints.push_back({10, 0.5f, 20});
  scorpion1->patrolPoints.push_back({20, 0.5f, 20});
  scorpion1->patrolPoints.push_back({20, 0.5f, 0});
  enemies.push_back(scorpion1);

  Enemy *scorpion2 = new Enemy(-15, 0.5f, -10);
  scorpion2->patrolPoints.push_back({-15, 0.5f, -10});
  scorpion2->patrolPoints.push_back({-15, 0.5f, 10});
  scorpion2->patrolPoints.push_back({-25, 0.5f, 10});
  scorpion2->patrolPoints.push_back({-25, 0.5f, -10});
  enemies.push_back(scorpion2);

  // NEW ENEMIES (SNAKES)
  // 3. Guarding the new pillars (Far Left)
  Enemy *snake3 = new Enemy(35, 0.5f, 10);
  snake3->patrolPoints.push_back({35, 0.5f, 10});
  snake3->patrolPoints.push_back({45, 0.5f, 10});
  snake3->patrolPoints.push_back({40, 0.5f, 0});
  enemies.push_back(snake3);

  // 4. Guarding the new pillars (Far Right)
  Enemy *snake4 = new Enemy(-35, 0.5f, -15);
  snake4->patrolPoints.push_back({-35, 0.5f, -15});
  snake4->patrolPoints.push_back({-30, 0.5f, -5});
  snake4->patrolPoints.push_back({-40, 0.5f, -5});
  enemies.push_back(snake4);

  // 5. Roaming near the entrance (Left)
  Enemy *snake5 = new Enemy(5, 0.5f, -30);
  snake5->patrolPoints.push_back({5, 0.5f, -30});
  snake5->patrolPoints.push_back({15, 0.5f, -35});
  snake5->patrolPoints.push_back({5, 0.5f, -40});
  enemies.push_back(snake5);

  // 6. Roaming near the entrance (Right)
  Enemy *snake6 = new Enemy(-5, 0.5f, -30);
  snake6->patrolPoints.push_back({-5, 0.5f, -30});
  snake6->patrolPoints.push_back({-15, 0.5f, -35});
  snake6->patrolPoints.push_back({-5, 0.5f, -40});
  enemies.push_back(snake6);

  // 7. Exploring the back area
  Enemy *snake7 = new Enemy(0, 0.5f, 30);
  snake7->patrolPoints.push_back({0, 0.5f, 30});
  snake7->patrolPoints.push_back({10, 0.5f, 40});
  snake7->patrolPoints.push_back({-10, 0.5f, 40});
  enemies.push_back(snake7);
}

void DesertLevel::spawnObstacles() {
  obstacles.clear();

  // --- ANCIENT WALLS & COLONNADES ---
  // Increased map size from 45.0 to 90.0
  float wallSize = 90.0f;
  float wallThickness = 4.0f; // Thicker walls
  float wallHeight = 15.0f;   // Taller walls

  // Boundaries
  // North (z = -wallSize)
  obstacles.push_back(new Obstacle(0, 0, -wallSize, wallSize * 2, wallHeight,
                                   wallThickness, WALL));
  // South (z = +wallSize)
  obstacles.push_back(new Obstacle(0, 0, wallSize, wallSize * 2, wallHeight,
                                   wallThickness, WALL));
  // West (x = -wallSize)
  obstacles.push_back(new Obstacle(-wallSize, 0, 0, wallThickness, wallHeight,
                                   wallSize * 2, WALL));
  // East (x = +wallSize)
  obstacles.push_back(new Obstacle(wallSize, 0, 0, wallThickness, wallHeight,
                                   wallSize * 2, WALL));

  // Colonnades (Rows of pillars along the walls)
  // East/West sides
  for (float z = -wallSize + 10; z < wallSize - 5; z += 20) {
    obstacles.push_back(
        new Obstacle(wallSize - 8, 0, z, 3, 8, 3, PILLAR_ASSET));
    obstacles.push_back(
        new Obstacle(-wallSize + 8, 0, z, 3, 8, 3, PILLAR_ASSET));
  }
  // North/South sides
  for (float x = -wallSize + 10; x < wallSize - 5; x += 20) {
    if (abs(x) > 15) { // Leave gap for gate/spawn area
      obstacles.push_back(new Obstacle(x, 0, -wallSize + 8, 3, 8, 3,
                                       PILLAR_ASSET)); // North (near portal)
      obstacles.push_back(new Obstacle(x, 0, wallSize - 8, 3, 8, 3,
                                       PILLAR_ASSET)); // South (near spawn)
    }
  }

  // --- CENTRAL RUINS ---
  // Grand Entrance Pillars (Near Spawn)
  obstacles.push_back(new Obstacle(15, 0, 50, 4, 12, 4, PILLAR));
  obstacles.push_back(new Obstacle(-15, 0, 50, 4, 12, 4, PILLAR));

  // Path to Portal (Pillars leading the way)
  obstacles.push_back(new Obstacle(20, 0, 10, 3, 10, 3, PILLAR));
  obstacles.push_back(new Obstacle(-20, 0, 10, 3, 10, 3, PILLAR));
  obstacles.push_back(new Obstacle(20, 0, -30, 3, 10, 3, PILLAR));
  obstacles.push_back(new Obstacle(-20, 0, -30, 3, 10, 3, PILLAR));

  // --- PYRAMIDS ---
  // Large pyramid - shifted
  obstacles.push_back(new Obstacle(-45, 0, 20, 15, 12, 15, PYRAMID));
  // Huge pyramid - far right
  obstacles.push_back(new Obstacle(50, 0, -20, 18, 15, 18, PYRAMID));
  // Medium pyramid
  obstacles.push_back(new Obstacle(30, 0, 40, 12, 10, 12, PYRAMID));
  // New Large pyramid
  obstacles.push_back(new Obstacle(-50, 0, -50, 14, 11, 14, PYRAMID));

  // --- NATURE ---
  // Palm trees scattered
  obstacles.push_back(new Obstacle(40, 0, 60, 2, 10, 2, TREE));
  obstacles.push_back(new Obstacle(-40, 0, 60, 2, 10, 2, TREE));
  obstacles.push_back(new Obstacle(60, 0, 10, 2, 10, 2, TREE));
  obstacles.push_back(new Obstacle(-60, 0, -20, 2, 10, 2, TREE));

  // Cacti
  obstacles.push_back(new Obstacle(10, 0, 30, 1, 4, 1, CACTUS));
  obstacles.push_back(new Obstacle(-5, 0, 45, 1, 4, 1, CACTUS));
  obstacles.push_back(new Obstacle(70, 0, -70, 1, 4, 1, CACTUS));

  // Spike traps (repositioned)
  traps.push_back(new Trap(0, 0.1f, 0, SPIKE_TRAP));   // Center map
  traps.push_back(new Trap(0, 0.1f, -40, SPIKE_TRAP)); // Nearer portal
}

void DesertLevel::update(float deltaTime) {
  // Update timer
  levelTimer -= deltaTime;

  updateDayNightCycle(deltaTime);
  checkOrbCollection();
  updateEnemies(deltaTime);
  checkEnemyCollision();

  // Update Chest Animations
  for (auto chest : chests) {
    if (chest->opened && chest->lidAngle < 90.0f) {
      chest->lidAngle += 90.0f * deltaTime; // Smooth opening over ~1 second
      if (chest->lidAngle > 90.0f)
        chest->lidAngle = 90.0f;
    }
  }

  // Update Orb Spawning Animation
  for (auto orb : collectibles) {
    if (orb->isSpawning) {
      orb->spawnTimer += deltaTime;
      if (orb->spawnTimer < 1.0f) {
        // Floating up animation (0 to 1.0)
        // Start from y=0.5 (inside chest) to y=1.5 (hover height)
        float progress = orb->spawnTimer;
        // Ease out function for smooth rise
        progress = 1.0f - pow(1.0f - progress, 3.0f);
        orb->y = 0.5f + progress * 1.0f;
      } else {
        orb->isSpawning = false;
        orb->y = 1.5f; // Final height
      }
    }

    // Update Orb Collection Animation
    if (orb->isCollecting) {
      orb->collectTimer += deltaTime;
      if (orb->collectTimer > 0.5f) { // Animation duration
        orb->collected = true;
        orb->isCollecting = false;
        player->collectOrb();
      }
    }
  }

  // Check trap collisions
  for (auto trap : traps) {
    if (player->checkCollision(trap->x, trap->z, trap->radius)) {
      player->takeDamage(10);
      // Trigger Camera Shake on damage
      extern Camera *camera; // Access global camera
      if (camera)
        camera->triggerShake(0.5f, 0.2f);
    }
  }

  // Check obstacle collisions
  // Box collision for WALL, PILLAR, ROCK, PYRAMID (SOLID - cannot pass
  // through)
  for (auto obs : obstacles) {
    if (obs->type == WALL || obs->type == PILLAR || obs->type == ROCK ||
        obs->type == PYRAMID || obs->type == PILLAR_ASSET) {
      // Use box collision
      player->resolveCollisionWithBox(obs->x, obs->z, obs->width, obs->depth);
    } else if (obs->type == TREE || obs->type == CACTUS) {
      // Use cylinder/radius collision
      // Approximate radius from width/depth
      float radius = (obs->width + obs->depth) / 4.0f; // Average half-dimension
      if (radius < 0.5f)
        radius = 0.5f; // Minimum radius
      player->resolveCollision(obs->x, obs->z, radius);
    }
  }

  // Activate portal when all orbs collected
  if (player->getOrbsCollected() >= totalOrbs) {
    portal->active = true;
    portal->active = true;
    // portal->rotation += 50.0f * deltaTime; // REMOVED ROTATION
    // portal->scale = 1.0f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) / 200.0f);
    // // Keep scale pulse? User said "nerer rotat", maybe scale is ok? Let's
    // keep it static for "ancient" feel.
    portal->scale = 1.0f; // Static scale

    // Check if player enters portal
    if (player->checkCollision(portal->x, portal->z, portal->radius)) {
      if (!isExiting) {
        isExiting = true;
        playSound(SOUND_VICTORY);
        // Could play Teleport sound here too
      }
    }
  }

  if (isExiting) {
    exitTimer += deltaTime;
    // Fade out player? Or just screen fade handled in render.
    if (exitTimer > 2.0f) {
      levelComplete = true;
    }
  }

  // Map Boundaries (Simple box constraint)
  // Expanded to match visual walls (90.0f)
  float mapSize = 88.0f; // Slightly less than 90 to keep inside walls
  float px = player->getX();
  float pz = player->getZ();
  bool clamped = false;

  if (px > mapSize) {
    px = mapSize;
    clamped = true;
  }
  if (px < -mapSize) {
    px = -mapSize;
    clamped = true;
  }
  if (pz > mapSize) {
    pz = mapSize;
    clamped = true;
  }
  if (pz < -mapSize) {
    pz = -mapSize;
    clamped = true;
  }

  if (clamped) {
    player->setPosition(px, player->getY(), pz);
  }
}

void DesertLevel::updateDayNightCycle(float deltaTime) {
  timeOfDay += deltaTime * daySpeed;
  if (timeOfDay > 2 * PI)
    timeOfDay -= 2 * PI;

  // Calculate sun intensity and color based on time of day
  float sunHeight = sin(timeOfDay);

  // Day (0 to PI) / Night (PI to 2PI)
  if (sunHeight > 0) {
    // Daylight Colors
    if (sunHeight < 0.3f) { // Sunrise/Sunset (Orange/Red)
      sunLight.diffuse = {1.0f, 0.5f + sunHeight, 0.2f, 1.0f};
      sunLight.ambient = {0.3f, 0.2f, 0.2f, 1.0f};
    } else { // Noon (White/Bright)
      sunLight.diffuse = {1.0f, 1.0f, 0.9f, 1.0f};
      sunLight.ambient = {0.4f, 0.4f, 0.4f, 1.0f};
    }
  } else {
    // Night Colors (Blueish)
    sunLight.diffuse = {0.1f, 0.1f, 0.3f, 1.0f};
    sunLight.ambient = {0.05f, 0.05f, 0.1f, 1.0f};
  }

  // Update Light Position (Move across sky)
  sunLight.position = {0 + cos(timeOfDay) * 100.0f, 50.0f * abs(sin(timeOfDay)),
                       50.0f * sin(timeOfDay), 1.0f};
}

void DesertLevel::checkOrbCollection() {
  for (auto orb : collectibles) {
    if (!orb->collected && !orb->isCollecting &&
        player->checkCollision(orb->x, orb->z, orb->radius)) {
      orb->isCollecting = true;
      playSound(SOUND_COLLECT_ORB);
    }
  }
}

void DesertLevel::updateEnemies(float deltaTime) {
  for (auto enemy : enemies) {
    if (enemy->patrolPoints.empty())
      continue;

    Vec3 target = enemy->patrolPoints[enemy->patrolIndex];
    float dx = target.x - enemy->x;
    float dz = target.z - enemy->z;
    float dist = sqrt(dx * dx + dz * dz);

    if (dist < 0.5f) {
      enemy->patrolIndex =
          (enemy->patrolIndex + 1) % enemy->patrolPoints.size();
    } else {
      enemy->x += (dx / dist) * enemy->speed * deltaTime;
      enemy->z += (dz / dist) * enemy->speed * deltaTime;
      enemy->rotation = atan2(dx, dz) * 180.0f / PI;
    }
  }
}

void DesertLevel::checkEnemyCollision() {
  for (auto enemy : enemies) {
    if (player->checkCollision(enemy->x, enemy->z, enemy->radius)) {
      if (player->canTakeDamage()) {
        player->takeDamage(15);
        // Trigger Camera Shake on damage
        extern Camera *camera;
        if (camera)
          camera->triggerShake(0.5f, 0.3f);

        playSound(SOUND_ENEMY_GROWL);

        // Trigger enemy reaction
        enemy->isHit = true;
        enemy->hitTimer = 0.5f;   // React for 0.5 seconds
        enemy->recoilDist = 1.5f; // Move back

        // Push enemy back
        float dx = enemy->x - player->getX();
        float dz = enemy->z - player->getZ();
        float dist = sqrt(dx * dx + dz * dz);
        if (dist > 0) {
          enemy->x += (dx / dist) * 2.0f;
          enemy->z += (dz / dist) * 2.0f;
        }
      }
      player->resolveCollision(enemy->x, enemy->z, enemy->radius + 1.0f);
    }
  }
}

void DesertLevel::interact(float px, float py, float pz) {
  checkChestInteraction(px, py, pz);
}

void DesertLevel::checkChestInteraction(float px, float py, float pz) {
  for (auto chest : chests) {
    float dx = px - chest->x;
    float dz = pz - chest->z;
    float dist = sqrt(dx * dx + dz * dz);

    if (dist < 8.0f && !chest->opened) { // Increased from 5.0f to 8.0f
      chest->opened = true;
      playSound(SOUND_CHEST_OPEN); // Play chest opening sound
      if (chest->hasOrb) {
        Collectible *orb =
            new Collectible(chest->x, 0.5f, chest->z); // Start low inside chest
        orb->isSpawning = true; // Trigger floating animation
        collectibles.push_back(orb);
      } else if (chest->hasCoins) {
        // Opened a chest with coins!
      }
    }
  }
}

void DesertLevel::reset() {
  levelComplete = false;
  levelComplete = false;
  portal->active = false;
  // Set Normal Physics
  player->setPhysics(60.0f, 10.0f, 6.5f);

  for (auto orb : collectibles)
    orb->collected = false;
  for (auto chest : chests) {
    chest->opened = false;
    chest->lidAngle = 0;
  }
}

void DesertLevel::render() {
  // Setup lighting
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, sunLight.position.data());
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunLight.ambient.data());
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLight.diffuse.data());
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunLight.specular.data());

  // Ensure transparency is disabled by default for solid objects
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);

  // Render ground and skybox scaled for new map size (90.0)
  // Rendering slightly larger (100.0) to avoid edges
  renderGround(100.0f, sandTexture);
  renderSkybox(0.5f, 0.7f, 0.9f);
  // Walls at 90.0f
  renderWalls(90.0f, 15.0f, desertWallTexture);

  // Render orbs
  for (auto orb : collectibles) {
    if (!orb->collected)
      renderOrb(orb);
  }

  // Render chests
  for (auto chest : chests)
    renderChest(chest);

  // Render enemies
  for (auto enemy : enemies)
    renderScorpion(enemy);

  // Render obstacles
  for (auto obs : obstacles) {
    if (obs->type == PILLAR)
      renderPillar(obs->x, obs->y, obs->z);
    else if (obs->type == PILLAR_ASSET) {
      // Render asset pillar inline
      glPushMatrix();
      glTranslatef(obs->x, obs->y, obs->z);
      if (pillarModel && pillarModel->getWidth() > 0) {
        glColor3f(0.7f, 0.6f, 0.5f);
        glScalef(0.2f, 0.2f, 0.2f); // Reduced scale from 0.3 to 0.2
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
        pillarModel->render();
      } else {
        // Fallback
        glColor3f(0.5f, 0.5f, 0.5f);
        glScalef(1, 3, 1);
        glutSolidCube(1.0f);
      }
      glPopMatrix();
    } else if (obs->type == TREE)
      renderPalmTree(obs->x, obs->y, obs->z);
    else if (obs->type == ROCK)
      renderRock(obs->x, obs->y, obs->z);
    else if (obs->type == CACTUS)
      renderCactus(obs->x, obs->y, obs->z);
    else if (obs->type == PYRAMID)
      renderPyramid(obs->x, obs->y, obs->z, obs->width, obs->height);
  }

  // Render spike traps
  glColor3f(0.4f, 0.4f, 0.4f);
  for (auto trap : traps) {
    glPushMatrix();
    glTranslatef(trap->x, trap->y, trap->z);

    if (trapModel && trapModel->getWidth() > 0) {
      glScalef(0.2f, 0.2f, 0.2f); // Adjust scale as needed
      trapModel->render();
    } else {
      glScalef(trap->radius, 0.3f, trap->radius);
      glutSolidCube(2.0f);

      // Spikes
      glColor3f(0.3f, 0.3f, 0.3f);
      for (int i = 0; i < 8; i++) {
        float angle = i * 45.0f;
        glPushMatrix();
        glRotatef(angle, 0, 1, 0);
        glTranslatef(0.5f, 0.3f, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(0.1f, 0.5f, 8, 1);
        glPopMatrix();
      }
    }
    glPopMatrix();
  }

  // Render portal
  if (portal)
    renderPortal();

  // Render Torches
  for (auto torch : torches) {
    glPushMatrix();
    glTranslatef(torch->x, torch->y, torch->z);

    // Torch stick
    glColor3f(0.4f, 0.2f, 0.1f);
    glPushMatrix();
    glScalef(0.1f, 1.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Flame (Simple particle effect simulation)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    torch->flickerOffset += 0.1f;
    float flicker = 0.8f + 0.2f * sin(torch->flickerOffset);

    glColor4f(1.0f, 0.5f, 0.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0, 0.8f, 0);
    glScalef(flicker * 0.3f, flicker * 0.5f, flicker * 0.3f);
    glutSolidSphere(1.0f, 8, 8);
    glPopMatrix();

    glDisable(GL_BLEND);
    glPopMatrix();
  }

  // Render Sun
  glPushMatrix();
  glTranslatef(0.0f, 150.0f,
               -120.0f); // Higher up and further back for grand scale
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 0.8f);    // Bright yellow-white
  glutSolidSphere(15.0f, 20, 20); // Massive sun
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void DesertLevel::renderDesertEnvironment() {
  // Render professional sand ground
  renderGround(50, sandTexture);

  float skyR = 0.53f + 0.2f * sin(timeOfDay);
  float skyG = 0.81f + 0.1f * sin(timeOfDay);
  float skyB = 0.92f;
  renderSkybox(skyR, skyG, skyB);

  // Render sandstone walls
  renderWalls(45, 8, desertWallTexture);
}

void DesertLevel::renderPillar(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

  // Professional Manual OpenGL Pillar Implementation
  // Base
  glColor3f(0.8f, 0.7f, 0.6f); // Sandstone light
  glPushMatrix();
  glScalef(1.2f, 0.5f, 1.2f);
  glutSolidCube(2.0f);
  glPopMatrix();

  // Shaft (Cylinder)
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, desertWallTexture.id); // Reuse wall texture
  glColor3f(1.0f, 1.0f, 1.0f);

  GLUquadric *quad = gluNewQuadric();
  gluQuadricTexture(quad, GL_TRUE); // Enable texture coords

  glPushMatrix();
  glTranslatef(0, 0.5f, 0);            // Start on top of base
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Upright
  gluCylinder(quad, 0.8f, 0.8f, 5.0f, 16, 1);
  glPopMatrix();

  gluDeleteQuadric(quad);
  glDisable(GL_TEXTURE_2D);

  // Capital (Top) - Simple flared block
  glColor3f(0.85f, 0.75f, 0.65f);
  glPushMatrix();
  glTranslatef(0, 5.5f, 0); // Top of shaft
  glScalef(1.4f, 0.6f, 1.4f);
  glutSolidCube(2.0f);
  glPopMatrix();

  // Gold Trim on Capital
  glColor3f(1.0f, 0.84f, 0.0f); // Gold
  glPushMatrix();
  glTranslatef(0, 5.8f, 0);
  glScalef(1.5f, 0.1f, 1.5f);
  glutSolidCube(2.0f);
  glPopMatrix();

  glPopMatrix();
}

void DesertLevel::renderPalmTree(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

  if (treeModel && treeModel->getWidth() > 0) {
    glScalef(1.5f, 1.5f, 1.5f);
    treeModel->render();
  } else {
    glColor3f(0.55f, 0.35f, 0.2f);
    GLUquadric *quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, 0.5f, 0.3f, 6, 12, 1);

    glColor3f(0.2f, 0.6f, 0.2f);
    for (int i = 0; i < 6; i++) {
      glPushMatrix();
      float angle = i * 60.0f;
      glRotatef(angle, 0, 0, 1);
      glTranslatef(0, 0, 6.5f);
      glRotatef(30, 1, 0, 0);
      glScalef(0.5f, 0.5f, 2.0f);
      glutSolidSphere(1.0f, 8, 8);
      glPopMatrix();
    }
    gluDeleteQuadric(quad);
  }
  glPopMatrix();
}

void DesertLevel::renderCactus(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

  if (cactusModel && cactusModel->getWidth() > 0) {
    // Rotate cactus upright (90 degrees anticlockwise)
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    // Your size scale
    glScalef(0.1f, 0.1f, 0.1f);

    // Green color - SOLID (no transparency)
    glColor3f(0.2f, 0.6f, 0.2f);
    cactusModel->render();
  } else {
    // Fallback cactus cube - SOLID
    glColor3f(0.2f, 0.6f, 0.2f);
    glScalef(0.5f, 2.0f, 0.5f);
    glutSolidCube(1.0f);
  }

  glPopMatrix();
}

void DesertLevel::renderPyramid(float x, float y, float z, float baseSize,
                                float height) {
  glPushMatrix();
  glTranslatef(x, y, z);

  // Professional Manual OpenGL Pyramid Implementation
  float halfSize = baseSize / 2.0f;

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, desertWallTexture.id); // Re-use wall texture

  glColor3f(1.0f, 1.0f, 1.0f); // White to apply texture

  glBegin(GL_TRIANGLES);

  // Front Face
  glNormal3f(0.0f, 0.5f, 1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-halfSize, 0.0f, halfSize);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(halfSize, 0.0f, halfSize);
  glTexCoord2f(0.5f, 1.0f);
  glVertex3f(0.0f, height, 0.0f);

  // Right Face
  glNormal3f(1.0f, 0.5f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(halfSize, 0.0f, halfSize);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(halfSize, 0.0f, -halfSize);
  glTexCoord2f(0.5f, 1.0f);
  glVertex3f(0.0f, height, 0.0f);

  // Back Face
  glNormal3f(0.0f, 0.5f, -1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(halfSize, 0.0f, -halfSize);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-halfSize, 0.0f, -halfSize);
  glTexCoord2f(0.5f, 1.0f);
  glVertex3f(0.0f, height, 0.0f);

  // Left Face
  glNormal3f(-1.0f, 0.5f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-halfSize, 0.0f, -halfSize);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-halfSize, 0.0f, halfSize);
  glTexCoord2f(0.5f, 1.0f);
  glVertex3f(0.0f, height, 0.0f);

  glEnd();

  // Bottom Face (Square)
  glBegin(GL_QUADS);
  glNormal3f(0.0f, -1.0f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-halfSize, 0.0f, halfSize);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-halfSize, 0.0f, -halfSize);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(halfSize, 0.0f, -halfSize);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(halfSize, 0.0f, halfSize);
  glEnd();

  // Capstone (Gold)
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 0.84f, 0.0f); // Gold
  glPushMatrix();
  glTranslatef(0, height - 0.5f, 0);
  glScalef(0.1f, 0.1f, 0.1f);
  // Simple diamond shape for capstone
  glBegin(GL_TRIANGLES);
  // (Simplified mini pyramid logic or just a small cube rotated)
  glEnd();
  glutSolidOctahedron(); // Easy professional capstone shape
  glPopMatrix();

  glEnable(GL_TEXTURE_2D);

  glPopMatrix();
}

void DesertLevel::renderRock(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

  if (rockModel && rockModel->getWidth() > 0) {
    // Armadillo is usually unit size or small, let's scale it up
    glScalef(3.0f, 3.0f, 3.0f);
    rockModel->render();
  } else {
    glColor3f(0.5f, 0.5f, 0.5f);
    glutSolidSphere(1.0f, 8, 8);
  }
  glPopMatrix();
}

void DesertLevel::renderOrb(Collectible *orb) {
  glPushMatrix();
  glTranslatef(orb->x, orb->y, orb->z);

  // Animation: Bobbing and Rotating
  float rotation = orb->rotation + glutGet(GLUT_ELAPSED_TIME) * 0.1f;
  float bob = sin(glutGet(GLUT_ELAPSED_TIME) * 0.003f) * 0.2f;

  if (orb->isCollecting) {
    // Scale up and spin fast
    float scale = 1.0f + (orb->collectTimer / 0.5f) * 2.0f;
    glScalef(scale, scale, scale);
    rotation *= 10.0f; // Fast spin
  } else {
    glTranslatef(0, bob, 0);
  }

  glRotatef(rotation, 0, 1, 0);

  glColor3f(1.0f, 0.84f, 0.0f);
  glutSolidSphere(orb->radius, 20, 20);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glColor4f(1.0f, 0.84f, 0.0f, 0.3f);
  glutSolidSphere(orb->radius * 1.3f, 20, 20);
  glDisable(GL_BLEND);

  glPopMatrix();
}

void DesertLevel::renderChest(Chest *chest) {
  glPushMatrix();

  // Floating Animation
  float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  // Offset based on position to unsync chests
  float bobOffset = sin(time * 2.0f + chest->x) * 0.3f;

  glTranslatef(chest->x, chest->y + 0.5f + bobOffset,
               chest->z); // Lift up slightly for floating effect

  // Magical Particle Ring (Spinning)
  if (!chest->opened) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (int i = 0; i < 8; i++) {
      float angle = time * 2.0f + i * (2.0f * 3.14159f / 8.0f);
      float r = 1.8f; // Radius
      float px = sin(angle) * r;
      float pz = cos(angle) * r;
      float py = sin(time * 3.0f + i) * 0.5f; // Vertical variance

      glPushMatrix();
      glTranslatef(px, py, pz);
      // Sparkle color (Gold/Magic)
      glColor4f(1.0f, 0.9f, 0.4f, 0.8f);
      glScalef(0.15f, 0.15f, 0.15f);
      glutSolidOctahedron();
      glPopMatrix();
    }
    glDisable(GL_BLEND);
  }

  if (chestModel && chestModel->getWidth() > 0) {
    // Scale to appropriate size
    glScalef(0.5f, 0.5f, 0.5f);

    // Set color to brown/wood
    glColor3f(0.6f, 0.4f, 0.2f);
    chestModel->render();
  } else {
    // Fallback: primitive chest with better visibility

    // Chest base (larger and more visible)
    glColor3f(0.6f, 0.4f, 0.2f); // Brown
    glPushMatrix();
    glScalef(2.0f, 1.2f, 1.5f); // Larger chest
    glutSolidCube(1.0f);
    glPopMatrix();

    // Animate lid opening (Logic moved to update for frame-rate independence)
    if (!chest->opened && chest->hasCoins) {
      chest->lidAngle = 15.0f;
    }

    // Chest lid
    glPushMatrix();
    glTranslatef(0, 0.6f, -0.75f);        // Position at back of chest
    glRotatef(-chest->lidAngle, 1, 0, 0); // Rotate lid open
    glTranslatef(0, 0, 0.75f);
    glColor3f(0.7f, 0.5f, 0.3f); // Lighter brown for lid
    glScalef(2.0f, 0.2f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Add glow effect for unopened chests with orbs
    if (!chest->opened && chest->hasOrb) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);

      // Pulsing Glow
      float pulse = 0.5f + 0.5f * sin(time * 4.0f);
      glColor4f(1.0f, 0.84f, 0.0f, 0.2f + 0.2f * pulse); // Golden glow

      glutSolidSphere(2.0f, 20, 20); // Larger sphere
      glDisable(GL_BLEND);
    }
  }

  glPopMatrix();
}

void DesertLevel::renderScorpion(Enemy *enemy) {
  // Distance Culling
  float dx = enemy->x - player->getX();
  float dz = enemy->z - player->getZ();
  float distSq = dx * dx + dz * dz;

  // Cull if further than 80 units (squared = 6400)
  // But always render if within view frustum ideally. Simple distance check for
  // now.
  if (distSq > 6400.0f)
    return;

  glPushMatrix();
  glTranslatef(enemy->x, enemy->y, enemy->z);
  glRotatef(enemy->rotation, 0, 1, 0);

  if (snakeModel && snakeModel->getWidth() > 0) {
    glScalef(0.05f, 0.05f, 0.05f); // Adjust scale as needed
    snakeModel->render();
  } else {
    // Fallback rendering
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.5f, 20, 20);
  }
  glPopMatrix();
}

void DesertLevel::renderPortal() {
  if (!portal->active && player->getOrbsCollected() < totalOrbs)
    return;

  glPushMatrix();
  glTranslatef(portal->x, portal->y, portal->z);
  // glRotatef(portal->rotation, 0, 1, 0); // REMOVED ROTATION

  // Scale the whole gate structure
  float gateScale = 1.5f;
  glScalef(gateScale, gateScale, gateScale);

  // --- ANCIENT EGYPTIAN GATE DESIGN (SIMPLIFIED) ---
  // Using simple cubes for a monolithic, ancient look.

  // 1. Left Pillar (Monolithic Block)
  glPushMatrix();
  glTranslatef(-2.5f, 3.0f, 0);
  glColor3f(0.82f, 0.70f, 0.55f); // Sandstone
  glScalef(1.5f, 6.0f, 1.5f);
  glutSolidCube(1.0f);
  glPopMatrix();

  // 2. Right Pillar (Monolithic Block)
  glPushMatrix();
  glTranslatef(2.5f, 3.0f, 0);
  glColor3f(0.82f, 0.70f, 0.55f); // Sandstone
  glScalef(1.5f, 6.0f, 1.5f);
  glutSolidCube(1.0f);
  glPopMatrix();

  // 3. Lintel (Top Beam)
  glPushMatrix();
  glTranslatef(0, 6.5f, 0);
  glColor3f(0.82f, 0.70f, 0.55f); // Sandstone
  glScalef(8.0f, 1.5f, 1.8f);
  glutSolidCube(1.0f);
  glPopMatrix();

  // 4. Decorative Gold Cornice (Simple Strip)
  glPushMatrix();
  glTranslatef(0, 7.3f, 0);
  glColor3f(1.0f, 0.84f, 0.0f); // Gold
  glScalef(8.2f, 0.3f, 2.0f);
  glutSolidCube(1.0f);
  glPopMatrix();

  // 5. Portal Energy Field (The actual "gate")
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  if (portal->active) {
    // Pulsing blue/gold energy
    float pulse = 0.5f + 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) / 200.0f);
    glColor4f(0.2f, 0.6f, 1.0f, 0.6f * pulse);
  } else {
    // Dim inactive state
    glColor4f(0.1f, 0.1f, 0.1f, 0.3f);
  }

  glPushMatrix();
  glTranslatef(0, 3.0f, 0);
  glScalef(4.0f, 5.5f, 0.2f);
  glutSolidCube(1.0f);
  glPopMatrix();

  // Swirling particles effect for active portal
  if (portal->active) {
    glColor4f(1.0f, 0.9f, 0.5f, 0.8f);
    float time = glutGet(GLUT_ELAPSED_TIME) / 500.0f;
    for (int i = 0; i < 8; i++) {
      glPushMatrix();
      glTranslatef(0, 3.0f, 0);
      glRotatef(time * 100.0f + i * 45.0f, 0, 0,
                1); // Internal swirl is fine, just not the gate itself
      glTranslatef(1.5f, 0, 0);
      glScalef(0.2f, 0.2f, 0.2f);
      glutSolidDodecahedron();
      glPopMatrix();
    }

    // Golden Glow Aura (Large outer glow)
    float goldenPulse = 0.6f + 0.4f * sin(glutGet(GLUT_ELAPSED_TIME) / 250.0f);
    glColor4f(1.0f, 0.84f, 0.0f, 0.3f * goldenPulse); // Golden color
    glPushMatrix();
    glTranslatef(0, 3.0f, 0);
    glScalef(3.5f, 5.0f, 3.5f); // Large aura sphere
    glutSolidSphere(1.0f, 24, 24);
    glPopMatrix();

    // Bright Golden Core Glow
    glColor4f(1.0f, 0.9f, 0.4f, 0.5f * goldenPulse); // Bright golden
    glPushMatrix();
    glTranslatef(0, 3.0f, 0);
    glScalef(2.2f, 4.0f, 2.2f); // Mid-sized glow
    glutSolidSphere(1.0f, 20, 20);
    glPopMatrix();
  }

  glDisable(GL_BLEND);
  glPopMatrix();
}

// ============================================================================
// ICE LEVEL IMPLEMENTATION
// ============================================================================

IceLevel::IceLevel() : Level() {
  survivalTimer = 0.0f;
  maxTime = 60.0f;
  victoryPlayed = false;
  icicleSpawnTimer = 0.0f;
  icicleSpawnInterval = 3.0f;
}

IceLevel::~IceLevel() {}

void IceLevel::init(Player *p) {
  player = p;
  levelComplete = false;
  player = p;
  levelComplete = false;
  survivalTimer = 0.0f;

  // Set Snow Physics (Low acceleration, Low friction/sliding, Higher max speed)
  player->setPhysics(15.0f, 1.5f, 9.0f);

  // Cold blue lighting
  sunLight.position = {0, 50, 0, 1};
  sunLight.ambient = {0.3f, 0.35f, 0.4f, 1.0f}; // Cool ambient
  sunLight.diffuse = {0.6f, 0.7f, 0.9f, 1.0f};  // Blue-tinted light
  sunLight.specular = {0.9f, 0.95f, 1.0f, 1.0f};

  spawnEnemies();
  spawnObstacles();

  portal = new Portal(0, 1, -35);

  loadCommonAssets();

  // Load ice-specific textures
  snowTexture = loadBMP("assets/snow_ground.bmp");
  iceWallTexture = loadBMP("assets/ice_wall.bmp");

  // Initialize snow particles
  for (int i = 0; i < 2000; i++) { // Increased to 2000 for "a lot" of snow
    Snowflake s;
    s.x = (rand() % 100) - 50.0f;
    s.y = (rand() % 50);
    s.z = (rand() % 100) - 50.0f;
    s.speed = 2.0f + (float)(rand() % 100) / 50.0f;
    snowParticles.push_back(s);
  }
}

void IceLevel::spawnEnemies() {
  enemies.clear();

  Enemy *elemental1 = new Enemy(15, 1, 0);
  elemental1->patrolPoints.push_back({15, 1, 0});
  elemental1->patrolPoints.push_back({15, 1, 15});
  elemental1->patrolPoints.push_back({-15, 1, 15});
  elemental1->patrolPoints.push_back({-15, 1, 0});
  enemies.push_back(elemental1);

  Enemy *elemental2 = new Enemy(-15, 1, -10);
  elemental2->patrolPoints.push_back({-15, 1, -10});
  elemental2->patrolPoints.push_back({15, 1, -10});
  elemental2->patrolPoints.push_back({15, 1, 10});
  elemental2->patrolPoints.push_back({-15, 1, 10});
  enemies.push_back(elemental2);

  // --- NEW ENEMIES (5 Added) ---

  // 3. Guarding the exit area
  Enemy *guard1 = new Enemy(0, 1, -25);
  guard1->patrolPoints.push_back({-5, 1, -25});
  guard1->patrolPoints.push_back({5, 1, -25});
  enemies.push_back(guard1);

  // 4. Roaming the center
  Enemy *roamer1 = new Enemy(0, 1, 0);
  roamer1->patrolPoints.push_back({0, 1, 5});
  roamer1->patrolPoints.push_back({5, 1, 0});
  roamer1->patrolPoints.push_back({0, 1, -5});
  roamer1->patrolPoints.push_back({-5, 1, 0});
  enemies.push_back(roamer1);

  // 5. Far corner ambusher
  Enemy *ambusher1 = new Enemy(30, 1, 30);
  ambusher1->patrolPoints.push_back({30, 1, 30});
  ambusher1->patrolPoints.push_back({20, 1, 20});
  enemies.push_back(ambusher1);

  // 6. Another corner guard
  Enemy *guard2 = new Enemy(-30, 1, 30);
  guard2->patrolPoints.push_back({-30, 1, 30});
  guard2->patrolPoints.push_back({-30, 1, 10});
  enemies.push_back(guard2);

  // 7. Fast interceptor
  Enemy *interceptor = new Enemy(20, 1, -20);
  interceptor->speed = 3.5f; // Faster than others
  interceptor->patrolPoints.push_back({20, 1, -20});
  interceptor->patrolPoints.push_back({-20, 1, -20});
  enemies.push_back(interceptor);
}

void IceLevel::spawnObstacles() {
  obstacles.clear();

  // Mix of Ice Pillars and Christmas Trees
  obstacles.push_back(new Obstacle(10, 0, 10, 2, 6, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(-12, 0, -12, 2, 6, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(20, 0, -15, 2, 6, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(-18, 0, 8, 2, 6, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(5, 0, -25, 2, 6, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(-25, 0, 5, 2, 6, 2, ICE_PILLAR));

  // Christmas Trees (Nature decoration)
  obstacles.push_back(new Obstacle(15, 0, 5, 2, 5, 2, CHRISTMAS_TREE));
  obstacles.push_back(new Obstacle(-5, 0, 15, 2, 5, 2, CHRISTMAS_TREE));
  obstacles.push_back(new Obstacle(-22, 0, -22, 2, 5, 2, CHRISTMAS_TREE));
  obstacles.push_back(new Obstacle(22, 0, 22, 2, 5, 2, CHRISTMAS_TREE));

  // Snowmen (Rocks)
  obstacles.push_back(new Obstacle(15, 0, 20, 2, 4, 2, ROCK));   // Snowman 1
  obstacles.push_back(new Obstacle(-20, 0, 15, 2, 4, 2, ROCK));  // Snowman 2
  obstacles.push_back(new Obstacle(25, 0, -10, 2, 4, 2, ROCK));  // Snowman 3
  obstacles.push_back(new Obstacle(-15, 0, -20, 2, 4, 2, ROCK)); // Snowman 4

  // Glowing Crystals embedded in walls
  obstacles.push_back(new Obstacle(35, 2, 35, 1, 1, 1, CRYSTAL));
  obstacles.push_back(new Obstacle(-35, 2, 35, 1, 1, 1, CRYSTAL));
  obstacles.push_back(new Obstacle(35, 2, -35, 1, 1, 1, CRYSTAL));
  obstacles.push_back(new Obstacle(-35, 2, -35, 1, 1, 1, CRYSTAL));

  // --- ARENA WALLS (Rigid Boundaries) ---
  float wallSize = 45.0f;
  float wallHeight = 15.0f;
  float wallThickness = 4.0f;
  // North (z = -wallSize)
  obstacles.push_back(new Obstacle(0, 0, -wallSize, wallSize * 2, wallHeight,
                                   wallThickness, WALL));
  // South (z = +wallSize)
  obstacles.push_back(new Obstacle(0, 0, wallSize, wallSize * 2, wallHeight,
                                   wallThickness, WALL));
  // West (x = -wallSize)
  obstacles.push_back(new Obstacle(-wallSize, 0, 0, wallThickness, wallHeight,
                                   wallSize * 2, WALL));
  // East (x = +wallSize)
  obstacles.push_back(new Obstacle(wallSize, 0, 0, wallThickness, wallHeight,
                                   wallSize * 2, WALL));

  // Spawn many ground traps (SPIKE_TRAP) - increased to 50 for harder
  // gameplay
  for (int i = 0; i < 50; i++) {
    float x = (rand() % 80) - 40.0f;
    float z = (rand() % 80) - 40.0f;
    // Avoid spawning too close to center (start area) - reduced safe zone
    if (abs(x) > 3 || abs(z) > 3) { // Reduced from 5 to 3 for more traps
      traps.push_back(new Trap(x, 0.1f, z, SPIKE_TRAP));
    }
  }
}

void IceLevel::spawnIcicle() {
  float x = (rand() % 60) - 30.0f;
  float z = (rand() % 60) - 30.0f;

  Trap *icicle = new Trap(x, 15, z, FALLING_ICICLE);
  icicle->showWarning = true;
  playSound(SOUND_ICICLE_CRACK); // Warning sound
  traps.push_back(icicle);
}

void IceLevel::update(float deltaTime) {
  updateTimer(deltaTime);
  updateIcicles(deltaTime);
  updateEnemies(deltaTime);
  updateEnemies(deltaTime);
  checkEnemyCollision();

  // Check trap collisions (Ground traps and falling icicles)
  for (auto trap : traps) {
    // Use a larger radius for better gameplay feel
    float trapRadius = (trap->type == FALLING_ICICLE) ? 1.5f : 1.5f;

    if (player->checkCollision(trap->x, trap->z, trapRadius)) {
      // For falling icicles, check height and active status
      if (trap->type == FALLING_ICICLE) {
        if (trap->active && trap->y < 4.0f && player->canTakeDamage()) {
          player->takeDamage(15);
          trap->active = false; // Destroy on impact
        }
      } else if (trap->type == SPIKE_TRAP) {
        // Ground traps always hit if player can take damage (no active check
        // needed)
        if (player->canTakeDamage()) {
          player->takeDamage(20); // Increased damage for ground traps
          extern Camera *camera;
          if (camera)
            camera->triggerShake(0.5f, 0.4f);
        }
      }
    }
  }

  for (auto obs : obstacles) {
    if (obs->type == WALL) {
      // Box collision for Walls
      player->resolveCollisionWithBox(obs->x, obs->z, obs->width, obs->depth);
    } else {
      // Radius collision for other rounded objects
      float obsRadius = obs->width / 2.0f;
      if (player->checkCollision(obs->x, obs->z, obsRadius)) {
        player->resolveCollision(obs->x, obs->z, obsRadius);
      }
    }
  }

  // --- Map Boundary Clamping ---
  float mapSize = 44.0f; // Slightly less than 45 to keep inside visual walls
  float px = player->getX();
  float pz = player->getZ();
  bool clamped = false;

  if (px > mapSize) {
    px = mapSize;
    clamped = true;
  }
  if (px < -mapSize) {
    px = -mapSize;
    clamped = true;
  }
  if (pz > mapSize) {
    pz = mapSize;
    clamped = true;
  }
  if (pz < -mapSize) {
    pz = -mapSize;
    clamped = true;
  }

  if (clamped) {
    player->setPosition(px, player->getY(), pz);
  }

  if (portal->active &&
      player->checkCollision(portal->x, portal->z, portal->radius)) {
    if (!isExiting) {
      isExiting = true;
      // playSound(SOUND_VICTORY); // Already played by timer logic in IceLevel,
      // but let's confirm
    }
  }

  if (isExiting) {
    exitTimer += deltaTime;
    if (exitTimer > 2.0f) {
      levelComplete = true;
    }
  }

  // Update snow particles
  for (auto &s : snowParticles) {
    s.y -= s.speed * deltaTime;
    if (s.y < 0) {
      s.y = 50.0f;
      s.x = (rand() % 100) - 50.0f; // Randomize X/Z on respawn for variety
      s.z = (rand() % 100) - 50.0f;
    }
  }
}

void IceLevel::updateTimer(float deltaTime) {
  survivalTimer += deltaTime;

  icicleSpawnTimer += deltaTime;
  if (icicleSpawnTimer >= icicleSpawnInterval) {
    spawnIcicle();
    icicleSpawnTimer = 0.0f;

    // Make it progressively harder - spawn faster over time
    if (icicleSpawnInterval >
        0.5f) { // Reduced from 1.5f to 0.5f for much more snowballs
      icicleSpawnInterval -= 0.15f; // Faster reduction
    }
  }

  if (survivalTimer >= maxTime && !portal->active) {
    portal->active = true;
    if (!victoryPlayed) {
      playSound(SOUND_VICTORY);
      victoryPlayed = true;
    }
  }

  if (portal->active) {
    portal->rotation += 50.0f * deltaTime;
    portal->scale = 1.0f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) / 200.0f);
  }
}

void IceLevel::updateIcicles(float deltaTime) {
  for (int i = traps.size() - 1; i >= 0; i--) {
    Trap *icicle = traps[i];

    if (icicle->showWarning) {
      icicle->warningTime -= deltaTime;
      if (icicle->warningTime <= 0) {
        icicle->showWarning = false;
        icicle->active = true;
        playSound(SOUND_ICICLE_FALL); // Falling sound
      }
    } else if (icicle->active) {
      icicle->y -= 15.0f * deltaTime;

      if (icicle->y <= 0.5f) {
        // Damage increases over time to make game harder
        int baseDamage = 25;
        int timeBonusDamage =
            (int)(survivalTimer / 10.0f) * 5; // +5 damage every 10 seconds
        int totalDamage = baseDamage + timeBonusDamage;

        // Increased damage radius for better hit detection
        if (player->checkCollision(icicle->x, icicle->z,
                                   icicle->radius * 2.0f)) {
          player->takeDamage(totalDamage);
          extern Camera *camera;
          if (camera)
            camera->triggerShake(0.5f, 0.5f);

          traps.erase(traps.begin() + i);
        } else if (icicle->y <= 0.5f) {
          // Hit ground - Shatter logic
          playSound(SOUND_ICICLE_CRACK); // Shatter sound
          // Spawn shatter particles (simple burst using existing snow system
          // for now, or just logic)
          for (int j = 0; j < 20; j++) {
            Snowflake s;
            s.x = icicle->x + (rand() % 20 - 10) / 20.0f;
            s.y = 0.5f;
            s.z = icicle->z + (rand() % 20 - 10) / 20.0f;
            s.speed = -5.0f; // Fly up? No, simpler to just delete for now or
                             // reuse particle system if accessible
            // Actually, we can just delete. The sound is the key feedback.
          }
          traps.erase(traps.begin() + i);
        }
      }
    }
  }
}

void IceLevel::updateEnemies(float deltaTime) {
  for (auto enemy : enemies) {
    // Handle hit reaction
    if (enemy->isHit) {
      enemy->hitTimer -= deltaTime;
      if (enemy->hitTimer <= 0) {
        enemy->isHit = false;
      }
    }

    if (enemy->patrolPoints.empty())
      continue;

    Vec3 target = enemy->patrolPoints[enemy->patrolIndex];
    float dx = target.x - enemy->x;
    float dz = target.z - enemy->z;
    float dist = sqrt(dx * dx + dz * dz);

    if (dist < 0.5f) {
      enemy->patrolIndex =
          (enemy->patrolIndex + 1) % enemy->patrolPoints.size();
    } else {
      if (!enemy->isHit) { // Only move if not hit
        enemy->x += (dx / dist) * enemy->speed * deltaTime;
        enemy->z += (dz / dist) * enemy->speed * deltaTime;
        enemy->rotation = atan2(dx, dz) * 180.0f / PI;
      }
    }
  }
}

void IceLevel::checkEnemyCollision() {
  for (auto enemy : enemies) {
    if (player->checkCollision(enemy->x, enemy->z, enemy->radius)) {
      if (player->canTakeDamage()) {
        player->takeDamage(20);
        extern Camera *camera;
        if (camera)
          camera->triggerShake(0.5f, 0.4f);
        // Trigger enemy reaction
        enemy->isHit = true;
        enemy->hitTimer = 0.5f;

        // Push enemy back
        float dx = enemy->x - player->getX();
        float dz = enemy->z - player->getZ();
        float dist = sqrt(dx * dx + dz * dz);
        if (dist > 0) {
          enemy->x += (dx / dist) * 2.0f;
          enemy->z += (dz / dist) * 2.0f;
        }
      }
      player->resolveCollision(enemy->x, enemy->z, enemy->radius + 1.5f);
    }
  }
}

void IceLevel::interact(float px, float py, float pz) {
  // No interactions
}

void IceLevel::reset() {
  levelComplete = false;
  victoryPlayed = false;
  survivalTimer = 0.0f;
  icicleSpawnTimer = 0.0f;
  icicleSpawnInterval = 3.0f;
  icicleSpawnInterval = 3.0f;
  portal->active = false;
  // Set Snow Physics
  player->setPhysics(15.0f, 1.5f, 9.0f);

  for (auto trap : traps)
    delete trap;
  traps.clear();
}

void IceLevel::renderIcePillar(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Professional Ice Material
  GLfloat iceSpec[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glMaterialfv(GL_FRONT, GL_SPECULAR, iceSpec);
  glMaterialf(GL_FRONT, GL_SHININESS, 120.0f);

  // Icy Blue Color with transparency
  glColor4f(0.5f, 0.7f, 1.0f, 0.7f);

  // 1. Central Main Shard (Large Hexagonal Crystal)
  glPushMatrix();
  glScalef(1.2f, 1.5f, 1.2f);
  glRotatef(-90, 1, 0, 0); // Upright

  // Base shaft
  GLUquadric *quad = gluNewQuadric();
  gluCylinder(quad, 1.0f, 0.6f, 4.0f, 6, 1); // Tapering slightly

  // Pointed Top
  glPushMatrix();
  glTranslatef(0, 0, 4.0f);
  glutSolidCone(0.6f, 1.5f, 6, 1);
  glPopMatrix();

  gluDeleteQuadric(quad);
  glPopMatrix();

  // 2. Surrounding Crystal Clusters
  for (int i = 0; i < 5; i++) {
    glPushMatrix();
    float angle = i * 72.0f;
    glRotatef(angle, 0, 1, 0);
    glTranslatef(0.8f, 0, 0);            // Move out
    glRotatef(15.0f, 0, 0, 1);           // Tilt outward
    glRotatef(rand() % 45, 0, 1, 0);     // Random twist
    float scale = 0.5f + (i % 3) * 0.2f; // Varied sizes

    glScalef(scale, scale * 1.5f, scale);
    glRotatef(-90, 1, 0, 0); // Upright

    // Small crystal shard
    glutSolidCone(0.5f, 3.0f, 5, 1);

    glPopMatrix();
  }

  // 3. Inner Glow (Core)
  glDisable(GL_LIGHTING);
  glColor4f(0.8f, 0.9f, 1.0f, 0.9f); // Bright core
  glPushMatrix();
  glScalef(0.4f, 4.0f, 0.4f);
  glutSolidSphere(1.0f, 8, 8);
  glPopMatrix();
  glEnable(GL_LIGHTING);

  // Reset material defaults
  glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
  glDisable(GL_BLEND);
  glPopMatrix();
}

void IceLevel::renderCrystal(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

  // Glowing crystal
  glColor3f(0.4f, 0.7f, 1.0f);
  glRotatef(45, 0, 1, 0);
  glScalef(0.5f, 1.5f, 0.5f);
  glutSolidOctahedron();

  // Glow effect
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glColor4f(0.4f, 0.7f, 1.0f, 0.3f);
  glScalef(1.5f, 1.5f, 1.5f);
  glutSolidSphere(1.0f, 12, 12);
  glDisable(GL_BLEND);

  glPopMatrix();
}

void IceLevel::renderIcicle(Trap *icicle) {
  glPushMatrix();
  glTranslatef(icicle->x, icicle->y, icicle->z);

  if (icicle->type == FALLING_ICICLE) {
    // Render as Ice Ball (Sphere)
    glColor3f(0.8f, 0.9f, 1.0f);   // Ice color
    glutSolidSphere(1.0f, 16, 16); // Ice ball
  } else if (icicle->type == SPIKE_TRAP) {
    // Render as Spike Trap (Ground Trap)
    if (trapModel && trapModel->getWidth() > 0) {
      glScalef(0.2f, 0.2f, 0.2f); // Adjust scale as needed
      trapModel->render();
    } else {
      // Fallback
      glColor3f(0.5f, 0.5f, 0.5f);
      glutSolidCone(0.5f, 1.0f, 8, 1);
    }
  }

  glPopMatrix();
}

void IceLevel::renderWarningCircle(float x, float z, float radius) {
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushMatrix();
  glTranslatef(x, 0.05f, z);
  glRotatef(-90, 1, 0, 0);

  // Pulsing red warning circle
  float pulse = 0.5f + 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) / 100.0f);
  glColor4f(1.0f, 0.0f, 0.0f, 0.4f * pulse);

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 0, 0);
  for (int i = 0; i <= 32; i++) {
    float angle = i * 2.0f * PI / 32.0f;
    glVertex3f(cos(angle) * radius, sin(angle) * radius, 0);
  }
  glEnd();

  // Red outline
  glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
  glLineWidth(3.0f);
  glBegin(GL_LINE_LOOP);
  for (int i = 0; i < 32; i++) {
    float angle = i * 2.0f * PI / 32.0f;
    glVertex3f(cos(angle) * radius, sin(angle) * radius, 0);
  }
  glEnd();

  glPopMatrix();
  glDisable(GL_BLEND);
  glEnable(GL_LIGHTING);
}

void IceLevel::renderIceElemental(Enemy *enemy) {
  glPushMatrix();
  glTranslatef(enemy->x, enemy->y, enemy->z);
  glRotatef(enemy->rotation, 0, 1, 0);

  // Crystalline body
  glColor3f(0.6f, 0.8f, 1.0f);
  glutSolidSphere(0.7f, 12, 12);

  // Floating shards around it
  float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  for (int i = 0; i < 4; i++) {
    glPushMatrix();
    float angle = i * 90.0f + time * 50.0f;
    glRotatef(angle, 0, 1, 0);
    glTranslatef(1.2f, sin(time * 2 + i) * 0.3f, 0);
    glRotatef(time * 100 + i * 30, 1, 1, 0);
    glScalef(0.2f, 0.5f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();
  }

  glPopMatrix();
}

void IceLevel::renderPortal() {
  if (!portal->active)
    return;

  glPushMatrix();
  glTranslatef(portal->x, portal->y + 2, portal->z);
  glRotatef(portal->rotation, 0, 1, 0);
  glScalef(portal->scale, portal->scale, portal->scale);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glColor4f(0.4f, 0.7f, 1.0f, 0.7f);
  glutSolidTorus(0.3f, 2.0f, 20, 30);

  glColor4f(0.6f, 0.9f, 1.0f, 0.5f);
  glutSolidSphere(1.8f, 20, 20);

  glDisable(GL_BLEND);
  glPopMatrix();
}

void IceLevel::renderTimer3D() {
  float timeLeft = maxTime - survivalTimer;
  if (timeLeft < 0)
    timeLeft = 0;

  glDisable(GL_LIGHTING);
  glPushMatrix();

  // Position timer in 3D space above portal area
  glTranslatef(portal->x, 8, portal->z - 10);

  // Billboard effect - face camera (simplified)
  glRotatef(180, 0, 1, 0);

  // Scale based on urgency
  float scale = 1.0f;
  if (timeLeft < 10.0f) {
    scale = 1.0f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) / 100.0f);
  }
  glScalef(scale, scale, scale);

  // Color based on time remaining
  if (timeLeft < 10.0f) {
    glColor3f(1.0f, 0.0f, 0.0f);
  } else if (timeLeft < 20.0f) {
    glColor3f(1.0f, 0.5f, 0.0f);
  } else if (timeLeft < 30.0f) {
    glColor3f(1.0f, 1.0f, 0.0f);
  } else {
    glColor3f(1.0f, 1.0f, 1.0f);
  }

  // Render time as 3D numbers
  char timeStr[16];
  sprintf(timeStr, "%.0f", timeLeft);

  float x = -0.5f * strlen(timeStr);
  for (char *c = timeStr; *c != '\0'; c++) {
    glPushMatrix();
    glTranslatef(x, 0, 0);
    glScalef(0.02f, 0.03f, 0.02f);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    glPopMatrix();
    x += 1.0f;
  }

  glPopMatrix();
  glEnable(GL_LIGHTING);
}

void IceLevel::render() {
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, sunLight.position.data());
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunLight.ambient.data());
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLight.diffuse.data());
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunLight.specular.data());

  // Reset color to white to prevent state leakage (e.g. from red timer)
  glColor3f(1.0f, 1.0f, 1.0f);

  renderIceEnvironment();

  for (auto enemy : enemies) {
    renderIceElemental(enemy);
  }

  for (auto icicle : traps) {
    if (icicle->showWarning) {
      renderWarningCircle(icicle->x, icicle->z, icicle->radius);
    } else {
      renderIcicle(icicle);
    }
  }
  for (auto obs : obstacles) {
    if (obs->type == ICE_PILLAR) {
      renderIcePillar(obs->x, obs->y, obs->z);
    } else if (obs->type == CRYSTAL) {
      renderCrystal(obs->x, obs->y, obs->z);
    } else if (obs->type == CHRISTMAS_TREE) {
      glPushMatrix();
      glTranslatef(obs->x, obs->y, obs->z);
      if (christmasTreeModel && christmasTreeModel->getWidth() > 0) {
        glScalef(0.15f, 0.15f,
                 0.15f);             // Increased scale from 0.05f to 0.15f
        glColor3f(1.0f, 1.0f, 1.0f); // Reset color to white
        christmasTreeModel->render();
      } else {
        // Fallback: Green cone
        glColor3f(0.0f, 0.5f, 0.0f);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(2.0f, 5.0f, 8, 1);
      }
      glPopMatrix();
    } else if (obs->type == ROCK) { // We're using ROCK type for snowmen
      renderSnowman(obs->x, obs->y, obs->z);
    }
  }

  if (portal)
    renderPortal();
  renderTimer3D();

  // --- RED WARNING LIGHT FOR ICICLES (GL_LIGHT2) ---
  bool warningActive = false;
  float wx, wz;
  for (auto icicle : traps) {
    if (icicle->showWarning) {
      warningActive = true;
      wx = icicle->x;
      wz = icicle->z;
      break; // One light for now
    }
  }

  if (warningActive) {
    glEnable(GL_LIGHT2);
    float pulse = 0.5f + 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.02f);
    GLfloat lightPos[] = {wx, 2.0f, wz, 1.0f};
    GLfloat lightColor[] = {1.0f * pulse, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightColor);

    // Attenuation to keep it local
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.5f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.2f);
  } else {
    glDisable(GL_LIGHT2);
  }

  // --- PORTAL PULSING LIGHT (GL_LIGHT3) ---
  if (portal && portal->active) {
    glEnable(GL_LIGHT3);
    float pulse = 0.8f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f);
    GLfloat lightPos[] = {portal->x, portal->y + 2.0f, portal->z, 1.0f};
    GLfloat lightColor[] = {1.0f * pulse, 0.8f, 0.2f, 1.0f}; // Gold
    glLightfv(GL_LIGHT3, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColor);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.1f);
  } else {
    glDisable(GL_LIGHT3);
  }
}

void IceLevel::renderIceEnvironment() {
  // Render icy ground with high specularity
  glEnable(GL_LIGHTING);
  glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat[]){1.0f, 1.0f, 1.0f, 1.0f});
  glMaterialf(GL_FRONT, GL_SHININESS, 100.0f); // High shininess for ice

  renderGround(50, snowTexture);

  // Reset material properties
  glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat[]){0.0f, 0.0f, 0.0f, 1.0f});
  glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);

  // Cold blue sky
  renderSkybox(0.6f, 0.7f, 0.85f);

  // Icy blue walls
  renderWalls(45, 8, iceWallTexture);

  // Render snow particles as small spheres for better visibility
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 1.0f);
  for (const auto &s : snowParticles) {
    glPushMatrix();
    glTranslatef(s.x, s.y, s.z);
    glutSolidSphere(0.1f, 4, 4); // Small sphere
    glPopMatrix();
  }
  glEnable(GL_LIGHTING);
}
void IceLevel::renderSnowman(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

  if (snowmanModel && snowmanModel->getWidth() > 0) {
    // Rotate snowman upright if needed
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

    // Scale to appropriate size
    glScalef(1.0f, 1.0f, 1.0f);

    // White color for snowman
    glColor3f(1.0f, 1.0f, 1.0f);
    snowmanModel->render();
  } else {
    // Fallback: Simple snowman made of spheres
    glColor3f(1.0f, 1.0f, 1.0f);

    // Bottom sphere
    glPushMatrix();
    glTranslatef(0, 0.8f, 0);
    glutSolidSphere(0.8f, 16, 16);
    glPopMatrix();

    // Middle sphere
    glPushMatrix();
    glTranslatef(0, 1.8f, 0);
    glutSolidSphere(0.6f, 16, 16);
    glPopMatrix();

    // Head sphere
    glPushMatrix();
    glTranslatef(0, 2.6f, 0);
    glutSolidSphere(0.4f, 16, 16);
    glPopMatrix();

    // Carrot nose
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
    glTranslatef(0, 2.6f, 0.4f);
    glRotatef(90, 1, 0, 0);
    glutSolidCone(0.1f, 0.3f, 8, 1);
    glPopMatrix();
  }

  glPopMatrix();
}
