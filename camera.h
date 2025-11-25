// ============================================================================
// Camera.h - Camera System
// Supports both First-Person and Third-Person perspectives
// ============================================================================

#ifndef CAMERA_H
#define CAMERA_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>

enum CameraMode { FIRST_PERSON, THIRD_PERSON };

class Camera {
private:
  CameraMode mode;

  // Position and orientation
  float posX, posY, posZ;
  float targetX, targetY, targetZ;
  float upX, upY, upZ;

  // Third person settings
  float distanceBehind;
  float heightAbove;

  // First person settings
  float yaw;
  float pitch;

  // Smooth camera movement
  float smoothSpeed;
  float currentYaw;

public:
  Camera();
  ~Camera();

  void update(float playerX, float playerY, float playerZ, float playerYaw,
              float deltaTime);
  void apply();
  void toggleMode();
  void updateMouse(int deltaX, int deltaY);

  CameraMode getMode() const { return mode; }
  void setMode(CameraMode newMode) { mode = newMode; }

  // Getters
  float getX() const { return posX; }
  float getY() const { return posY; }
  float getZ() const { return posZ; }
  float getYaw() const { return yaw; }
};

#endif // CAMERA_H