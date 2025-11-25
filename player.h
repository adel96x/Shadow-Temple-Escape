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
#include <cmath>

class Player {
private:
  // Position and orientation
  float x, y, z;
  float yaw;

  // Movement
  float moveSpeed;
  float turnSpeed;

  // Jumping
  float jumpSpeed;
  float velocityY;
  float gravity;
  bool isJumping;
  bool isGrounded;

  // Collision
  float radius;
  float height;

  // Game state
  int health;
  int maxHealth;
  int orbsCollected;

  // Animation
  float bobPhase;
  float bobAmount;

  // Damage cooldown
  float damageCooldown;

  // Initial position for reset
  float initialX, initialY, initialZ;

public:
  Player(float startX, float startY, float startZ);
  ~Player();

  void update(float deltaTime);
  void render();
  void move(float forward, float strafe, float deltaTime);
  void jump();
  void takeDamage(int amount);
  void collectOrb();
  void reset();
  void resetPosition(float newX, float newY, float newZ);

  // Collision helpers
  bool checkCollision(float objX, float objZ, float objRadius);
  void resolveCollision(float objX, float objZ, float objRadius);

  // Getters
  float getX() const { return x; }
  float getY() const { return y; }
  float getZ() const { return z; }
  float getYaw() const { return yaw; }
  float getRadius() const { return radius; }
  float getHeight() const { return height; }
  int getHealth() const { return health; }
  int getOrbsCollected() const { return orbsCollected; }
  bool isAlive() const { return health > 0; }
  bool canTakeDamage() const { return damageCooldown <= 0.0f; }

  // Setters
  void setPosition(float newX, float newY, float newZ);
  void setYaw(float newYaw) { yaw = newYaw; }
};

#endif // PLAYER_H