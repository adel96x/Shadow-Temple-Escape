// ============================================================================
// Player.h - Player Character
// Handles movement, jumping, collision, and state
// ============================================================================

#ifndef PLAYER_H
#define PLAYER_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "model.h"
#include <cmath>

class Player {
private:
  Model *playerModel;
  // Position and orientation
  float x, y, z;
  float yaw;

  // Movement
  float moveSpeed;
  float turnSpeed;

  // Physics Movement
  float velocityX, velocityZ;
  float acceleration;
  float friction;
  float maxSpeed;

  // Jumping
  float jumpSpeed;
  float velocityY;
  float gravity;
  bool isJumping;
  bool isGrounded;

  float radius;
  float height;

  int health;
  int maxHealth;
  int orbsCollected;
  bool alive;

  float bobPhase;
  float bobAmount;
  float damageCooldown;
  float damageFlashTimer;
  float glowTimer;

  float footstepTimer;
  float landTimer;
  bool wasGrounded;

  float initialX, initialY, initialZ;

public:
  Player(float startX, float startY, float startZ);
  ~Player();

  void loadModel(const char *filename);
  void update(float deltaTime);
  void render();
  void move(float forward, float strafe, float deltaTime,
            bool skipRotation = false);
  void jump();
  void takeDamage(int amount);
  void collectOrb();
  void triggerGlow();
  void reset();
  void resetPosition(float newX, float newY, float newZ);
  void setPosition(float newX, float newY, float newZ);
  void setYaw(float newYaw) { yaw = newYaw; }
  void setPhysics(float accel, float fric, float maxSpd);

  // Collision
  bool checkCollision(float objX, float objZ, float objRadius);
  void resolveCollision(float objX, float objZ, float objRadius);
  bool checkCollisionWithBox(float boxX, float boxZ, float width, float depth);
  void resolveCollisionWithBox(float boxX, float boxZ, float width,
                               float depth);

  // Getters
  float getX() const { return x; }
  float getY() const { return y; }
  float getZ() const { return z; }
  float getYaw() const { return yaw; }
  float getRadius() const { return radius; }
  float getHeight() const { return height; }
  int getHealth() const { return health; }
  int getOrbsCollected() const { return orbsCollected; }
  bool isAlive() const { return alive; }
  float getDamageFlashTimer() const { return damageFlashTimer; }
  float getGlowTimer() const { return glowTimer; }
  float getLandTimer() const { return landTimer; }
  bool canTakeDamage() const { return damageCooldown <= 0.0f; }
};

#endif // PLAYER_H