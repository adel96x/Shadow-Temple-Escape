// ============================================================================
// Level.cpp - Complete Level Implementation
// Includes Desert Level (Level 1) and Ice Level (Level 2)
// ============================================================================

#include "level.h"
#include <cstdio>
#include <cstdlib>

#define PI 3.14159265359f

// ============================================================================
// BASE LEVEL CLASS
// ============================================================================

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
}

void Level::renderGround(float size, float r, float g, float b) {
  glDisable(GL_LIGHTING);
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
  glVertex3f(-size, 0, -size);
  glVertex3f(size, 0, -size);
  glVertex3f(size, 0, size);
  glVertex3f(-size, 0, size);
  glEnd();
  glEnable(GL_LIGHTING);
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

void Level::renderWalls(float size, float height) {
  glEnable(GL_LIGHTING);
  glColor3f(0.6f, 0.5f, 0.4f);

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
}

DesertLevel::~DesertLevel() {
  for (auto c : chests)
    delete c;
}

void DesertLevel::init(Player *p) {
  player = p;
  levelComplete = false;

  // Setup lighting
  sunLight.position = {0, 50, 0, 1};
  sunLight.ambient = {0.3f, 0.3f, 0.3f, 1.0f};
  sunLight.diffuse = {0.8f, 0.7f, 0.5f, 1.0f};
  sunLight.specular = {1.0f, 1.0f, 0.8f, 1.0f};

  // Spawn level elements
  spawnOrbs();
  spawnChests();
  spawnEnemies();
  spawnObstacles();

  // Create portal
  portal = new Portal(0, 1, -40);
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
  chests.push_back(new Chest(25, 0.5f, 0, true));
  chests.push_back(new Chest(-20, 0.5f, 20, false));
  chests.push_back(new Chest(0, 0.5f, 25, true));
  chests.push_back(new Chest(30, 0.5f, -15, false));
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
}

void DesertLevel::spawnObstacles() {
  obstacles.clear();

  // Stone pillars
  obstacles.push_back(new Obstacle(5, 0, 5, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(-8, 0, -8, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(15, 0, -5, 2, 6, 2, PILLAR));
  obstacles.push_back(new Obstacle(-12, 0, 12, 2, 6, 2, PILLAR));

  // Palm trees
  obstacles.push_back(new Obstacle(30, 0, 10, 1.5f, 8, 1.5f, TREE));
  obstacles.push_back(new Obstacle(-25, 0, -15, 1.5f, 8, 1.5f, TREE));

  // Spike traps
  traps.push_back(new Trap(0, 0.1f, 10, SPIKE_TRAP));
  traps.push_back(new Trap(18, 0.1f, -12, SPIKE_TRAP));
}

void DesertLevel::update(float deltaTime) {
  updateDayNightCycle(deltaTime);
  checkOrbCollection();
  updateEnemies(deltaTime);
  checkEnemyCollision();

  // Check trap collisions
  for (auto trap : traps) {
    if (player->checkCollision(trap->x, trap->z, trap->radius)) {
      player->takeDamage(10);
    }
  }

  // Check obstacle collisions
  for (auto obs : obstacles) {
    float obsRadius = obs->width / 2.0f;
    if (player->checkCollision(obs->x, obs->z, obsRadius)) {
      player->resolveCollision(obs->x, obs->z, obsRadius);
    }
  }

  // Activate portal when all orbs collected
  if (player->getOrbsCollected() >= totalOrbs) {
    portal->active = true;
    portal->rotation += 50.0f * deltaTime;
    portal->scale = 1.0f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) / 200.0f);

    // Check if player enters portal
    if (player->checkCollision(portal->x, portal->z, portal->radius)) {
      levelComplete = true;
    }
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
      player->takeDamage(15);
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

    if (dist < 3.0f && !chest->opened) {
      chest->opened = true;
      if (chest->hasOrb) {
        collectibles.push_back(
            new Collectible(chest->x, chest->y + 1, chest->z));
      }
    }
  }
}

void DesertLevel::reset() {
  levelComplete = false;
  portal->active = false;

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
    else if (obs->type == TREE)
      renderPalmTree(obs->x, obs->y, obs->z);
  }

  // Render spike traps
  glColor3f(0.4f, 0.4f, 0.4f);
  for (auto trap : traps) {
    glPushMatrix();
    glTranslatef(trap->x, trap->y, trap->z);
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
    glPopMatrix();
  }

  // Render portal
  if (portal)
    renderPortal();
}

void DesertLevel::renderDesertEnvironment() {
  renderGround(50, 0.76f, 0.70f, 0.50f);

  float skyR = 0.53f + 0.2f * sin(timeOfDay);
  float skyG = 0.81f + 0.1f * sin(timeOfDay);
  float skyB = 0.92f;
  renderSkybox(skyR, skyG, skyB);

  renderWalls(45, 8);
}

void DesertLevel::renderPillar(float x, float y, float z) {
  glColor3f(0.7f, 0.6f, 0.5f);
  glPushMatrix();
  glTranslatef(x, y + 3, z);
  glScalef(1, 3, 1);
  glutSolidCube(2.0f);

  glTranslatef(0, 1.1f, 0);
  glScalef(1.3f, 0.3f, 1.3f);
  glutSolidCube(2.0f);
  glPopMatrix();
}

void DesertLevel::renderPalmTree(float x, float y, float z) {
  glPushMatrix();
  glTranslatef(x, y, z);

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

  glColor3f(0.45f, 0.3f, 0.15f);
  glScalef(1.5f, 1.0f, 1.0f);
  glutSolidCube(1.0f);

  if (chest->opened && chest->lidAngle < 90) {
    chest->lidAngle += 2.0f;
  }
  glTranslatef(0, 0.5f, -0.5f);
  glRotatef(-chest->lidAngle, 1, 0, 0);
  glTranslatef(0, 0, 0.5f);
  glColor3f(0.5f, 0.35f, 0.2f);
  glutSolidCube(1.0f);

  glPopMatrix();
}

void DesertLevel::renderScorpion(Enemy *enemy) {
  glPushMatrix();
  glTranslatef(enemy->x, enemy->y, enemy->z);
  glRotatef(enemy->rotation, 0, 1, 0);

  glColor3f(0.4f, 0.25f, 0.1f);
  glScalef(1.2f, 0.5f, 0.8f);
  glutSolidSphere(0.7f, 12, 12);

  glPopMatrix();
}

void DesertLevel::renderPortal() {
  if (!portal->active && player->getOrbsCollected() < totalOrbs)
    return;

  glPushMatrix();
  glTranslatef(portal->x, portal->y + 2, portal->z);
  glRotatef(portal->rotation, 0, 1, 0);
  glScalef(portal->scale, portal->scale, portal->scale);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  if (portal->active) {
    glColor4f(1.0f, 0.84f, 0.0f, 0.7f);
  } else {
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
  }

  glutSolidTorus(0.3f, 2.0f, 20, 30);

  glColor4f(0.8f, 0.7f, 1.0f, 0.5f);
  glutSolidSphere(1.8f, 20, 20);

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
  survivalTimer = 0.0f;

  sunLight.position = {0, 50, 0, 1};
  sunLight.ambient = {0.2f, 0.2f, 0.3f, 1.0f};
  sunLight.diffuse = {0.4f, 0.5f, 0.7f, 1.0f};
  sunLight.specular = {0.8f, 0.8f, 1.0f, 1.0f};

  spawnEnemies();
  spawnObstacles();

  portal = new Portal(0, 1, -35);
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
}

void IceLevel::spawnObstacles() {
  obstacles.clear();

  obstacles.push_back(new Obstacle(10, 0, 10, 2, 5, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(-10, 0, -10, 2, 5, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(20, 0, -15, 2, 5, 2, ICE_PILLAR));
  obstacles.push_back(new Obstacle(-18, 0, 8, 2, 5, 2, ICE_PILLAR));
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
  checkEnemyCollision();

  for (auto obs : obstacles) {
    float obsRadius = obs->width / 2.0f;
    if (player->checkCollision(obs->x, obs->z, obsRadius)) {
      player->resolveCollision(obs->x, obs->z, obsRadius);
    }
  }

  if (portal->active &&
      player->checkCollision(portal->x, portal->z, portal->radius)) {
    levelComplete = true;
  }
}

void IceLevel::updateTimer(float deltaTime) {
  survivalTimer += deltaTime;

  icicleSpawnTimer += deltaTime;
  if (icicleSpawnTimer >= icicleSpawnInterval) {
    spawnIcicle();
    icicleSpawnTimer = 0.0f;

    if (icicleSpawnInterval > 1.5f) {
      icicleSpawnInterval -= 0.1f;
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
        if (player->checkCollision(icicle->x, icicle->z, icicle->radius)) {
          player->takeDamage(25);
        }

        delete icicle;
        traps.erase(traps.begin() + i);
      }
    }
  }
}

void IceLevel::updateEnemies(float deltaTime) {
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

void IceLevel::checkEnemyCollision() {
  for (auto enemy : enemies) {
    if (player->checkCollision(enemy->x, enemy->z, enemy->radius)) {
      player->takeDamage(20);
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
  portal->active = false;

  for (auto trap : traps)
    delete trap;
  traps.clear();
}

void IceLevel::render() {
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, sunLight.position.data());
  glLightfv(GL_LIGHT0, GL_AMBIENT, sunLight.ambient.data());
  glLightfv(GL_LIGHT0, GL_DIFFUSE, sunLight.diffuse.data());
  glLightfv(GL_LIGHT0, GL_SPECULAR, sunLight.specular.data());

  renderIceEnvironment();

  for (auto obs : obstacles) {
    renderIcePillar(obs->x, obs->y, obs->z);
  }

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

  if (portal)
    renderPortal();
  renderTimer3D();
}

void IceLevel::renderIceEnvironment() {
  renderGround(50, 0.7f, 0.85f, 0.95f);
  renderSkybox(0.4f, 0.5f, 0.7f);

  glColor3f(0.6f, 0.7f, 0.8f);
  renderWalls(45, 8);
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

  glColor3f(0.8f, 0.9f, 1.0f);
  glRotatef(180, 1, 0, 0);
  glutSolidCone(0.3f, 2.0f, 8, 1);

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