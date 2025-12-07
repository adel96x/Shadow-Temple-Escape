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
  moveSpeed = 11.0f;  // Faster, more responsive movement
  turnSpeed = 900.0f; // Quicker turning for better control

  // Default Physics (Desert/Normal)
  velocityX = 0.0f;
  velocityZ = 0.0f;
  acceleration = 80.0f; // High acceleration for snappy feel
  friction = 10.0f;     // High friction for quick stops
  maxSpeed = 11.0f;

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

  // Apply velocity to position
  x += velocityX * deltaTime;
  z += velocityZ * deltaTime;

  // Apply friction
  float speed = sqrt(velocityX * velocityX + velocityZ * velocityZ);
  if (speed > 0.0f) {
    float drop = speed * friction * deltaTime;
    float newSpeed = speed - drop;
    if (newSpeed < 0.0f)
      newSpeed = 0.0f;

    velocityX *= newSpeed / speed;
    velocityZ *= newSpeed / speed;
  }
}

void Player::move(float forward, float strafe, float deltaTime,
                  bool skipRotation) {
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

    // Only calculate rotation in third person mode
    if (!skipRotation) {
      // Calculate target rotation based on movement direction
      float targetYaw = atan2(moveX, moveZ) * 180.0f / PI;
      float diff = targetYaw - yaw;

      // Wrap angle difference to [-180, 180] range
      while (diff > 180.0f)
        diff -= 360.0f;
      while (diff < -180.0f)
        diff += 360.0f;

      // Smooth, responsive rotation with frame-independent interpolation
      float rotationSpeed = 12.0f; // Higher = faster turning
      float maxRotation = turnSpeed * deltaTime;
      float rotation = diff * rotationSpeed * deltaTime;

      // Clamp rotation to max turn speed
      if (rotation > maxRotation)
        rotation = maxRotation;
      else if (rotation < -maxRotation)
        rotation = -maxRotation;

      yaw += rotation;

      // Normalize yaw to [0, 360]
      while (yaw >= 360.0f)
        yaw -= 360.0f;
      while (yaw < 0.0f)
        yaw += 360.0f;
    }

    // Move in the direction of current yaw (smooth movement)
    float yawRad = yaw * PI / 180.0f;

    // In First Person (skipRotation), direction depends on forward/strafe
    // inputs relative to Look Angle
    float dirX, dirZ;

    if (skipRotation) {
      // Standard FPS Movement
      // Forward vector (sin, cos) for this coordinate system
      float fwdX = sin(yawRad);
      float fwdZ = cos(yawRad);
      // Right vector (cos, -sin)
      float rightX = cos(yawRad); // Derived from rotating forward -90 or +90
      float rightZ = -sin(yawRad);

      // Combine inputs
      // forward/strafe are +/- 1.0
      dirX = (fwdX * forward) + (rightX * strafe);
      dirZ = (fwdZ * forward) + (rightZ * strafe);

      // Normalize if moving diagonally
      float len = sqrt(dirX * dirX + dirZ * dirZ);
      if (len > 0) {
        dirX /= len;
        dirZ /= len;
      }
    } else {
      // Third Person: Just move forward in the direction we are currently
      // facing
      dirX = sin(yawRad);
      dirZ = cos(yawRad);
    }

    // Apply acceleration based on calculated direction
    float accelX = dirX * acceleration;
    float accelZ = dirZ * acceleration;

    velocityX += accelX * deltaTime;
    velocityZ += accelZ * deltaTime;

    // Cap speed
    float currentSpeed = sqrt(velocityX * velocityX + velocityZ * velocityZ);
    if (currentSpeed > maxSpeed) {
      velocityX = (velocityX / currentSpeed) * maxSpeed;
      velocityZ = (velocityZ / currentSpeed) * maxSpeed;
    }

    // Update head bob animation
    // Faster bob for clearer step rhythm
    bobPhase += deltaTime * 18.0f * (currentSpeed / maxSpeed);
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

    // Play hurt sound
    playSound(SOUND_DAMAGE);
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
  velocityX = 0.0f;
  velocityZ = 0.0f;
}

void Player::setPhysics(float accel, float fric, float maxSpd) {
  acceleration = accel;
  friction = fric;
  maxSpeed = maxSpd;
}

void Player::render() {
  glPushMatrix();
  float bobOffset = sin(bobPhase) * 0.12f; // Increased bob amount (was 0.08)
  // Lowered by 0.35f to ground feet, added bob
  glTranslatef(x, y - 0.35f + bobOffset, z);
  glRotatef(yaw, 0, 1, 0); // Match movement direction

  // Add sway (waddle) for natural running look
  float sway = sin(bobPhase) * 2.5f;
  glRotatef(sway, 0, 0, 1); // Z-axis sway

  if (damageCooldown > 0.0f && ((int)(damageCooldown * 10) % 2 == 0))
    glColor3f(1.0f, 0.3f, 0.3f);
  else
    glColor3f(1.0f, 1.0f, 1.0f);

  if (playerModel && playerModel->getWidth() > 0) {
    glPushMatrix();
    glRotatef(-90, 1, 0, 0); // Flip to stand upright
    glScalef(0.1f, 0.1f, 0.1f);
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
