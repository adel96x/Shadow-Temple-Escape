// ============================================================================
// Camera.cpp - Camera System Implementation
// ============================================================================

#include "camera.h"

#define PI 3.14159265359f

Camera::Camera() {
  mode = THIRD_PERSON;

  posX = posY = posZ = 0.0f;
  targetX = targetY = targetZ = 0.0f;
  upX = 0.0f;
  upY = 1.0f;
  upZ = 0.0f;

  // Zoomed out camera for better view
  distanceBehind = 10.0f; // Much further back for wider view
  heightAbove = 4.0f;     // Higher for better perspective

  yaw = 0.0f;
  pitch = 0.0f;

  // Responsive camera smoothing
  smoothSpeed = 0.15f;
  currentYaw = 0.0f;
}

Camera::~Camera() {}

void Camera::update(float playerX, float playerY, float playerZ,
                    float playerYaw, float deltaTime) {
  // Camera no longer auto-rotates with player - only responds to mouse input

  if (mode == THIRD_PERSON) {
    float yawRad = currentYaw * PI / 180.0f;

    // Calculate desired camera position behind player
    float desiredX = playerX + distanceBehind * sin(yawRad);
    float desiredZ = playerZ + distanceBehind * cos(yawRad);
    float desiredY = playerY + heightAbove;

    // Smooth camera position with frame-independent interpolation
    float positionLerp = 1.0f - pow(1.0f - smoothSpeed, deltaTime * 60.0f);
    posX += (desiredX - posX) * positionLerp;
    posY += (desiredY - posY) * positionLerp;
    posZ += (desiredZ - posZ) * positionLerp;

    // Look at player center
    targetX = playerX;
    targetY = playerY + 1.2f;
    targetZ = playerZ;

  } else if (mode == FIRST_PERSON) {
    // First-person camera at eye level
    posX = playerX;
    posY = playerY + 1.65f; // Eye level height
    posZ = playerZ;

    float yawRad = yaw * PI / 180.0f;
    float pitchRad = pitch * PI / 180.0f;

    // Calculate look direction
    targetX = posX + sin(yawRad) * cos(pitchRad);
    targetY = posY + sin(pitchRad);
    targetZ = posZ + cos(yawRad) * cos(pitchRad);
  }
}

void Camera::apply() {
  gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, upX, upY, upZ);
}

void Camera::toggleMode() {
  if (mode == FIRST_PERSON) {
    mode = THIRD_PERSON;
  } else {
    mode = FIRST_PERSON;
    yaw = currentYaw; // sync to third-person orientation
    pitch = 0.0f;
  }
}

void Camera::updateMouse(int deltaX, int deltaY) {
  if (mode == FIRST_PERSON) {
    // Professional first-person sensitivity
    float sensitivity = 0.12f;

    yaw += deltaX * sensitivity;
    pitch -= deltaY * sensitivity;

    // Clamp pitch to prevent camera flipping
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    // Normalize yaw angle
    if (yaw > 360.0f)
      yaw -= 360.0f;
    if (yaw < 0.0f)
      yaw += 360.0f;
  } else if (mode == THIRD_PERSON) {
    // More responsive third-person rotation
    float sensitivity = 0.35f;
    currentYaw += deltaX * sensitivity;
  }
}
