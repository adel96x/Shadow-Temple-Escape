// ============================================================================
// Player.cpp - Player Character Implementation
// ============================================================================

#include "player.h"
#include "utils.h"

// Helper if utils.h clamp is not found
#ifndef CLAMP_DEFINED
#define CLAMP_DEFINED
inline float clamp_local(float value, float min, float max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}
#endif

#define PI 3.14159265359f

Player::Player(float startX, float startY, float startZ) {
  x = initialX = startX;
  y = initialY = startY;
  z = initialZ = startZ;
  yaw = 0.0f;

  moveSpeed = 5.0f;
  turnSpeed = 180.0f;

  jumpSpeed = 6.0f;
  velocityY = 0.0f;
  gravity = -20.0f;
  isJumping = false;
  isGrounded = true;

  radius = 0.5f;
  height = 2.0f;

  health = 100;
  maxHealth = 100;
  orbsCollected = 0;

  bobPhase = 0.0f;
  bobAmount = 0.1f;

  damageCooldown = 0.0f;
  damageFlashTimer = 0.0f;
  alive = true;

  playerModel = new Model();
  // Try to load, will fail silently if file missing
  playerModel->load("assets/player.obj");
}

Player::~Player() {
  if (playerModel)
    delete playerModel;
}

void Player::loadModel(const char *filename) {
  if (playerModel)
    playerModel->load(filename);
}

void Player::update(float deltaTime) {
  // Apply gravity
  if (!isGrounded || isJumping) {
    velocityY += gravity * deltaTime;
    y += velocityY * deltaTime;

    // Ground check
    if (y <= 1.0f) {
      y = 1.0f;
      velocityY = 0.0f;
      isJumping = false;
      isGrounded = true;
    } else {
      isGrounded = false;
    }
  }

  // Update damage cooldown
  if (damageCooldown > 0.0f) {
    damageCooldown -= deltaTime;
  }

  // Update damage flash timer
  if (damageFlashTimer > 0.0f) {
    damageFlashTimer -= deltaTime;
  }
}

void Player::move(float forward, float strafe, float deltaTime) {
  if (forward == 0.0f && strafe == 0.0f)
    return;

  // Calculate movement direction
  float yawRad = yaw * PI / 180.0f;

  // Fix inverted controls:
  // Forward should be -Z (standard OpenGL)
  // Right should be +X
  // Yaw 0 -> Facing -Z

  // Forward Vector: (-sin(yaw), 0, -cos(yaw))
  // Right Vector: (cos(yaw), 0, -sin(yaw))

  float moveX = forward * -sin(yawRad) + strafe * cos(yawRad);
  float moveZ = forward * -cos(yawRad) + strafe * -sin(yawRad);

  // Normalize diagonal movement
  float length = sqrt(moveX * moveX + moveZ * moveZ);
  if (length > 0.0f) {
    moveX /= length;
    moveZ /= length;
  }

  // Apply movement
  x += moveX * moveSpeed * deltaTime;
  z += moveZ * moveSpeed * deltaTime;

  // Update bobbing animation when moving
  bobPhase += deltaTime * 10.0f;
}

void Player::jump() {
  if (isGrounded && !isJumping) {
    velocityY = jumpSpeed;
    isJumping = true;
    isGrounded = false;
    playSound(SOUND_JUMP); // Play jump sound
  }
}

void Player::takeDamage(int amount) {
  if (damageCooldown <= 0.0f) {
    health -= amount;
    playSound(SOUND_DAMAGE); // Play damage sound
    if (health <= 0) {
      health = 0;
      alive = false;
    }
    damageCooldown = 1.0f;   // 1 second invincibility
    damageFlashTimer = 0.3f; // Flash red for 0.3 seconds
  }
}

void Player::collectOrb() { orbsCollected++; }

void Player::reset() {
  x = initialX;
  y = initialY;
  z = initialZ;
  yaw = 0.0f;
  velocityY = 0.0f;
  isJumping = false;
  isGrounded = true;
  health = maxHealth;
  alive = true;
  orbsCollected = 0;
  damageCooldown = 0.0f;
  damageFlashTimer = 0.0f;
}

void Player::resetPosition(float newX, float newY, float newZ) {
  x = initialX = newX;
  y = initialY = newY;
  z = initialZ = newZ;
  velocityY = 0.0f;
  isJumping = false;
  isGrounded = true;
  orbsCollected = 0;
}

bool Player::checkCollision(float objX, float objZ, float objRadius) {
  float dx = x - objX;
  float dz = z - objZ;
  float distance = sqrt(dx * dx + dz * dz);
  return distance < (radius + objRadius);
}

bool Player::checkCollisionWithBox(float boxX, float boxZ, float width,
                                   float depth) {
  // Simple AABB vs Circle collision
  // Find the closest point on the box to the circle
  float closestX = clamp_local(x, boxX - width / 2, boxX + width / 2);
  float closestZ = clamp_local(z, boxZ - depth / 2, boxZ + depth / 2);

  // Calculate distance between circle center and this closest point
  float dx = x - closestX;
  float dz = z - closestZ;

  float distanceSquared = dx * dx + dz * dz;
  return distanceSquared < (radius * radius);
}

void Player::resolveCollisionWithBox(float boxX, float boxZ, float width,
                                     float depth) {
  float closestX = clamp_local(x, boxX - width / 2, boxX + width / 2);
  float closestZ = clamp_local(z, boxZ - depth / 2, boxZ + depth / 2);

  float dx = x - closestX;
  float dz = z - closestZ;
  float distance = sqrt(dx * dx + dz * dz);

  if (distance < radius && distance > 0.0f) {
    float overlap = radius - distance;
    float pushX = (dx / distance) * overlap;
    float pushZ = (dz / distance) * overlap;

    x += pushX;
    z += pushZ;
  } else if (distance == 0.0f) {
    // Center is inside the box, push out towards nearest edge
    // This is a simplification, ideally check min penetration
    x += radius * 1.1f;
  }
}

void Player::resolveCollision(float objX, float objZ, float objRadius) {
  float dx = x - objX;
  float dz = z - objZ;
  float distance = sqrt(dx * dx + dz * dz);

  if (distance < (radius + objRadius) && distance > 0.0f) {
    // Push player away
    float overlap = (radius + objRadius) - distance;
    float pushX = (dx / distance) * overlap;
    float pushZ = (dz / distance) * overlap;

    x += pushX;
    z += pushZ;
  }
}

void Player::setPosition(float newX, float newY, float newZ) {
  x = newX;
  y = newY;
  z = newZ;
}

void Player::render() {
  glPushMatrix();

  // Apply bobbing when moving
  float bobOffset = sin(bobPhase) * bobAmount;
  glTranslatef(x, y + bobOffset, z);
  glRotatef(-yaw, 0, 1, 0);

  // Draw player
  if (damageCooldown > 0.0f && (int)(damageCooldown * 10) % 2 == 0) {
    glColor3f(1.0f, 0.3f, 0.3f); // Flash red when damaged
  } else {
    glColor3f(1.0f, 1.0f, 1.0f);
  }

  if (playerModel && playerModel->getWidth() > 0) {
    glPushMatrix();

    // Rotate upright (model was laying on the ground)
    glRotatef(90, 1, 0, 0); // <<< FIX: rotate 90 degrees around X axis

    glScalef(0.1f, 0.1f, 0.1f);
    glRotatef(180, 0, 1, 0); // keep your original rotation
    playerModel->render();

    glPopMatrix();
  } else {
    // Fallback: Body (cylinder)
    glColor3f(0.8f, 0.6f, 0.4f);
    GLUquadric *quad = gluNewQuadric();

    // Fallback model already has correct orientation fix:
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, radius * 0.7f, radius * 0.7f, height * 0.6f, 16, 1);

    // Head
    glTranslatef(0, 0, height * 0.6f);
    glutSolidSphere(radius * 0.5f, 16, 16);

    // Backpack
    glColor3f(0.4f, 0.3f, 0.2f);
    glTranslatef(0, -radius * 0.4f, 0);
    glScalef(0.5f, 0.6f, 0.3f);
    glutSolidCube(1.0f);

    gluDeleteQuadric(quad);
  }
  glPopMatrix();
}