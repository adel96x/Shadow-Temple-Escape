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

  distanceBehind = 7.0f;
  heightAbove = 3.5f;

  yaw = 0.0f;
  pitch = 0.0f;
  currentYaw = 0.0f;

  smoothSpeed = 0.1f;
}

Camera::~Camera() {
  // Nothing to clean up
}

void Camera::update(float playerX, float playerY, float playerZ,
                    float playerYaw, float deltaTime) {
  if (mode == THIRD_PERSON) {
    // Smooth yaw transition
    // currentYaw += (playerYaw - currentYaw) * smoothSpeed;

    // Calculate desired camera position behind player
    float yawRad = currentYaw * PI / 180.0f;
    float desiredX = playerX - distanceBehind * sin(yawRad);
    float desiredZ = playerZ - distanceBehind * cos(yawRad);
    float desiredY = playerY + heightAbove;

    // Smooth interpolation to desired position
    posX += (desiredX - posX) * smoothSpeed;
    posY += (desiredY - posY) * smoothSpeed;
    posZ += (desiredZ - posZ) * smoothSpeed;

    // Look at player's upper body
    targetX = playerX;
    targetY = playerY + 1.8f;
    targetZ = playerZ;

  } else if (mode == FIRST_PERSON) {
    // Camera at player's eye level
    posX = playerX;
    posY = playerY + 1.7f; // Eye height
    posZ = playerZ;

    // Calculate look direction from yaw and pitch
    float yawRad = yaw * PI / 180.0f;
    float pitchRad = pitch * PI / 180.0f;

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
    yaw = currentYaw; // Sync first person yaw with third person
    pitch = 0.0f;
  }
}

void Camera::updateMouse(int deltaX, int deltaY) {
  if (mode == FIRST_PERSON) {
    float sensitivity = 0.15f;
    yaw += deltaX * sensitivity;
    pitch -= deltaY * sensitivity;

    // Clamp pitch to prevent camera flipping
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    // Wrap yaw
    if (yaw > 360.0f)
      yaw -= 360.0f;
    if (yaw < 0.0f)
      yaw += 360.0f;
  } else if (mode == THIRD_PERSON) {
    // Optional: rotate camera around player in third person
    float sensitivity = 0.2f;
    currentYaw += deltaX * sensitivity;
  }
}