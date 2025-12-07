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

  if (pillarModel)
    delete pillarModel;
  if (treeModel)
    delete treeModel;
  if (rockModel)
    delete rockModel;
  if (groundModel)
    delete groundModel;
  if (cactusModel)
    delete cactusModel;

  // Static models are not deleted here
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
  player->setPhysics(60.0f, 10.0f, 6.5f);

  // Spawn level elements
  spawnOrbs();
  spawnChests();
  spawnEnemies();
  spawnEnemies();
  spawnObstacles();

  // Spawn Torches (New Feature)
  torches.clear();
  torches.push_back(new Torch(-4, 2, -38)); // Left of gate
  torches.push_back(new Torch(4, 2, -38));  // Right of gate
  torches.push_back(new Torch(10, 2, 0));   // Near start
  torches.push_back(new Torch(-10, 2, 0));  // Near start

  // Create portal
  portal = new Portal(0, 1, -40);

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

  // Stone pillars
  obstacles.push_back(new Obstacle(5, 0, 5, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(-8, 0, -8, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(15, 0, -5, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(-12, 0, 12, 2, 6, 2, PILLAR));
  // Additional Pillars for grand entrance look
  obstacles.push_back(new Obstacle(8, 0, -30, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(-8, 0, -30, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(8, 0, -20, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(-8, 0, -20, 2, 6, 2, PILLAR));

  // ASSET PILLARS - Using the loaded model (different look)
  // Repositioned to avoid overlap with existing objects
  obstacles.push_back(
      new Obstacle(40, 0, 10, 3, 6, 3, PILLAR_ASSET)); // Far Left
  obstacles.push_back(
      new Obstacle(-35, 0, -10, 3, 6, 3, PILLAR_ASSET)); // Far Right
  obstacles.push_back(
      new Obstacle(10, 0, -42, 3, 6, 3, PILLAR_ASSET)); // Near Gate Left
  obstacles.push_back(
      new Obstacle(-10, 0, -42, 3, 6, 3, PILLAR_ASSET)); // Near Gate Right

  // Palm trees
  obstacles.push_back(new Obstacle(30, 0, 10, 1.5f, 8, 1.5f, TREE));
  obstacles.push_back(new Obstacle(-25, 0, -15, 1.5f, 8, 1.5f, TREE));

  // Cacti - cylindrical collision
  obstacles.push_back(new Obstacle(10, 0, -15, 1, 4, 1, CACTUS));
  obstacles.push_back(new Obstacle(-5, 0, 25, 1, 4, 1, CACTUS));
  obstacles.push_back(new Obstacle(20, 0, 15, 1, 4, 1, CACTUS));
  obstacles.push_back(new Obstacle(-30, 0, -5, 1, 4, 1, CACTUS));

  // Small realistic pyramids with SOLID collision - player CANNOT pass through
  // Increased sizes and count as requested
  obstacles.push_back(
      new Obstacle(-30, 0, 30, 12, 10, 12, PYRAMID)); // Large pyramid
  obstacles.push_back(
      new Obstacle(35, 0, -30, 15, 12, 15, PYRAMID)); // Huge pyramid
  obstacles.push_back(
      new Obstacle(15, 0, 25, 10, 8, 10, PYRAMID)); // Medium pyramid
  obstacles.push_back(
      new Obstacle(-20, 0, -25, 14, 11, 14, PYRAMID)); // New Large pyramid
  obstacles.push_back(
      new Obstacle(30, 0, 35, 18, 14, 18,
                   PYRAMID)); // Moved from (5,0,-35) to avoid Portal overlap
  obstacles.push_back(
      new Obstacle(-40, 0, 0, 10, 8, 10, PYRAMID)); // New Medium pyramid

  // Add walls as obstacles for collision
  float wallSize = 45.0f;
  float wallThickness = 2.0f;
  float wallHeight = 8.0f;

  // North wall (z = -wallSize)
  obstacles.push_back(new Obstacle(0, 0, -wallSize, wallSize * 2, wallHeight,
                                   wallThickness, WALL));
  // South wall (z = +wallSize)
  obstacles.push_back(new Obstacle(0, 0, wallSize, wallSize * 2, wallHeight,
                                   wallThickness, WALL));
  // West wall (x = -wallSize)
  obstacles.push_back(new Obstacle(-wallSize, 0, 0, wallThickness, wallHeight,
                                   wallSize * 2, WALL));
  // East wall (x = +wallSize)
  obstacles.push_back(new Obstacle(wallSize, 0, 0, wallThickness, wallHeight,
                                   wallSize * 2, WALL));

  // Spike traps
  traps.push_back(new Trap(0, 0.1f, 10, SPIKE_TRAP));
  traps.push_back(new Trap(18, 0.1f, -12, SPIKE_TRAP));
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
      levelComplete = true;
    }
  }

  // Map Boundaries (Simple box constraint)
  float mapSize = 48.0f; // Slightly less than 50 to keep inside walls
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
  timeOfDay += daySpeed * deltaTime;
  if (timeOfDay > 2.0f * PI)
    timeOfDay -= 2.0f * PI;

  // Update sun color based on time
  float sunFactor = (sin(timeOfDay) + 1.0f) / 2.0f;
  sunLight.diffuse[0] = 0.5f + 0.5f * sunFactor;
  sunLight.diffuse[1] = 0.4f + 0.4f * sunFactor;
  sunLight.diffuse[2] = 0.3f + 0.2f * sunFactor;
}

void DesertLevel::checkOrbCollection() {
  for (auto orb : collectibles) {
    if (!orb->collected &&
        player->checkCollision(orb->x, orb->z, orb->radius)) {
      orb->collected = true;
      player->collectOrb();
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

  renderDesertEnvironment();

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
        glutSolidCube(2.0f);
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
  glTranslatef(sunLight.position[0], sunLight.position[1],
               sunLight.position[2]);
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 0.8f); // Bright yellow-white
  glutSolidSphere(5.0f, 20, 20);
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
  orb->rotation += 1.0f;
  orb->bobPhase += 0.05f;
  float bobOffset = sin(orb->bobPhase) * 0.3f;

  glPushMatrix();
  glTranslatef(orb->x, orb->y + bobOffset, orb->z);
  glRotatef(orb->rotation, 0, 1, 0);

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
  glTranslatef(chest->x, chest->y, chest->z);

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
    // if (chest->opened && chest->lidAngle < 90) {
    //   chest->lidAngle += 2.0f;
    // } else if (!chest->opened && chest->hasCoins) {
    //   // Slightly ajar if it has coins
    //   chest->lidAngle = 15.0f;
    // }
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
      glColor4f(1.0f, 0.84f, 0.0f, 0.3f); // Golden glow
      glutSolidSphere(1.5f, 16, 16);
      glDisable(GL_BLEND);
    }
  }

  glPopMatrix();
}

void DesertLevel::renderScorpion(Enemy *enemy) {
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
  snowTexture = loadBMP("assets/snow.bmp");
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

  obstacles.push_back(new Obstacle(10, 0, 10, 2, 5, 2, CHRISTMAS_TREE));
  obstacles.push_back(new Obstacle(-10, 0, -10, 2, 5, 2, CHRISTMAS_TREE));
  obstacles.push_back(new Obstacle(20, 0, -15, 2, 5, 2, CHRISTMAS_TREE));
  obstacles.push_back(new Obstacle(-18, 0, 8, 2, 5, 2, CHRISTMAS_TREE));
  obstacles.push_back(new Obstacle(15, 0, 20, 2, 4, 2, ROCK));   // Snowman 1
  obstacles.push_back(new Obstacle(-20, 0, 15, 2, 4, 2, ROCK));  // Snowman 2
  obstacles.push_back(new Obstacle(25, 0, -10, 2, 4, 2, ROCK));  // Snowman 3
  obstacles.push_back(new Obstacle(-15, 0, -20, 2, 4, 2, ROCK)); // Snowman 4

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
    float obsRadius = obs->width / 2.0f;
    if (player->checkCollision(obs->x, obs->z, obsRadius)) {
      // DAMAGE LOGIC FOR SNOWMEN (ROCKS)
      if (obs->type == ROCK && player->canTakeDamage()) {
        player->takeDamage(10);
        // Knockback logic could be added here similar to enemies
        float dx = obs->x - player->getX();
        float dz = obs->z - player->getZ();
        // Simple push back
        if (abs(dx) > 0.1f || abs(dz) > 0.1f) {
          // Basic push away not implemented here, but damage is key request
        }
        extern Camera *camera;
        if (camera)
          camera->triggerShake(0.3f, 0.2f);
      }
      player->resolveCollision(obs->x, obs->z, obsRadius);
    }
  }

  if (portal->active &&
      player->checkCollision(portal->x, portal->z, portal->radius)) {
    levelComplete = true;
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

  glColor3f(0.7f, 0.85f, 0.95f);
  GLUquadric *quad = gluNewQuadric();

  // Crystalline pillar shape
  for (int i = 0; i < 6; i++) {
    glPushMatrix();
    glRotatef(i * 60.0f, 0, 1, 0);
    glTranslatef(0.5f, 2.5f, 0);
    glScalef(0.2f, 2.5f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();
  }

  // Center column
  glRotatef(-90, 1, 0, 0);
  gluCylinder(quad, 0.8f, 0.5f, 5, 6, 1);

  gluDeleteQuadric(quad);
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
