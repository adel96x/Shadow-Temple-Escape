#include "player.h"
#include "utils.h"

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

  // Professional movement parameters
  moveSpeed = 6.5f;   // Faster, more responsive movement
  turnSpeed = 900.0f; // Quicker turning for better control

  // Improved jump physics
  jumpSpeed = 7.0f; // Higher jump
  velocityY = 0.0f;
  gravity = -22.0f; // Slightly stronger gravity for better feel
  isJumping = false;
  isGrounded = true;

  radius = 0.5f;
  height = 2.0f;

  health = 100;
  maxHealth = 100;
  orbsCollected = 0;

  bobPhase = 0.0f;
  bobAmount = 0.08f; // Reduced bob for smoother feel

  damageCooldown = 0.0f;
  damageFlashTimer = 0.0f;
  alive = true;

  playerModel = new Model();
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
  if (!isGrounded || isJumping) {
    velocityY += gravity * deltaTime;
    y += velocityY * deltaTime;

    if (y <= 1.0f) {
      y = 1.0f;
      velocityY = 0.0f;
      isJumping = false;
      isGrounded = true;
    } else {
      isGrounded = false;
    }
  }

  if (damageCooldown > 0.0f)
    damageCooldown -= deltaTime;
  if (damageFlashTimer > 0.0f)
    damageFlashTimer -= deltaTime;
}

void Player::move(float forward, float strafe, float deltaTime) {
  // Professional movement with acceleration and smooth rotation

  if (forward == 0.0f && strafe == 0.0f)
    return;

  // Compute movement direction in world space
  float moveX = strafe;   // left/right
  float moveZ = -forward; // forward/back

  // Normalize movement vector
  float length = sqrt(moveX * moveX + moveZ * moveZ);
  if (length > 0.0f) {
    moveX /= length;
    moveZ /= length;

    // Calculate target rotation based on movement direction
    float targetYaw = atan2(moveX, moveZ) * 180.0f / PI;
    float diff = targetYaw - yaw;

    // Wrap angle difference to [-180, 180] range
    while (diff > 180.0f)
      diff -= 360.0f;
    while (diff < -180.0f)
      diff += 360.0f;

    // Smooth rotation with frame-independent interpolation
    float rotationLerp = 1.0f - pow(0.001f, deltaTime); // Very fast rotation
    yaw += diff * rotationLerp;

    // Normalize yaw to [0, 360]
    while (yaw >= 360.0f)
      yaw -= 360.0f;
    while (yaw < 0.0f)
      yaw += 360.0f;

    // Move in the direction of current yaw
    float yawRad = yaw * PI / 180.0f;
    float speed = moveSpeed * length;

    x += sin(yawRad) * speed * deltaTime;
    z += cos(yawRad) * speed * deltaTime;

    // Update head bob animation
    bobPhase += deltaTime * 12.0f * length;
  }
}

void Player::jump() {
  if (isGrounded && !isJumping) {
    velocityY = jumpSpeed;
    isJumping = true;
    isGrounded = false;
  }
}

void Player::takeDamage(int amount) {
  if (damageCooldown <= 0.0f) {
    health -= amount;
    if (health <= 0)
      alive = false;
    damageCooldown = 1.0f;
    damageFlashTimer = 0.3f;
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
  float dx = x - objX, dz = z - objZ;
  return sqrt(dx * dx + dz * dz) < (radius + objRadius);
}

bool Player::checkCollisionWithBox(float boxX, float boxZ, float width,
                                   float depth) {
  float closestX = clamp_local(x, boxX - width / 2, boxX + width / 2);
  float closestZ = clamp_local(z, boxZ - depth / 2, boxZ + depth / 2);

  float dx = x - closestX, dz = z - closestZ;
  return (dx * dx + dz * dz) < (radius * radius);
}

void Player::resolveCollision(float objX, float objZ, float objRadius) {
  float dx = x - objX, dz = z - objZ;
  float dist = sqrt(dx * dx + dz * dz);

  if (dist < radius + objRadius && dist > 0.0f) {
    float overlap = (radius + objRadius) - dist;
    x += dx / dist * overlap;
    z += dz / dist * overlap;
  }
}

void Player::resolveCollisionWithBox(float boxX, float boxZ, float width,
                                     float depth) {
  float closestX = clamp_local(x, boxX - width / 2, boxX + width / 2);
  float closestZ = clamp_local(z, boxZ - depth / 2, boxZ + depth / 2);
  float dx = x - closestX, dz = z - closestZ;
  float dist = sqrt(dx * dx + dz * dz);
  if (dist < radius && dist > 0.0f) {
    float overlap = radius - dist;
    x += dx / dist * overlap;
    z += dz / dist * overlap;
  }
}

void Player::setPosition(float newX, float newY, float newZ) {
  x = newX;
  y = newY;
  z = newZ;
}

void Player::render() {
  glPushMatrix();
  float bobOffset = sin(bobPhase) * bobAmount;
  glTranslatef(x, y + bobOffset, z);
  glRotatef(-yaw, 0, 1, 0);

  if (damageCooldown > 0.0f && ((int)(damageCooldown * 10) % 2 == 0))
    glColor3f(1.0f, 0.3f, 0.3f);
  else
    glColor3f(1.0f, 1.0f, 1.0f);

  if (playerModel && playerModel->getWidth() > 0) {
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glScalef(0.1f, 0.1f, 0.1f);
    glRotatef(180, 0, 1, 0);
    playerModel->render();
    glPopMatrix();
  } else {
    glColor3f(0.8f, 0.6f, 0.4f);
    GLUquadric *quad = gluNewQuadric();
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, radius * 0.7f, radius * 0.7f, height * 0.6f, 16, 1);
    glTranslatef(0, 0, height * 0.6f);
    glutSolidSphere(radius * 0.5f, 16, 16);
    glColor3f(0.4f, 0.3f, 0.2f);
    glTranslatef(0, -radius * 0.4f, 0);
    glScalef(0.5f, 0.6f, 0.3f);
    glutSolidCube(1.0f);
    gluDeleteQuadric(quad);
  }

  glPopMatrix();
}
