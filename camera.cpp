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
  distanceBehind = 8.0f; // Closer for intimate action feel
  heightAbove = 3.5f;    // Lower perspective

  yaw = 0.0f;
  pitch = 0.0f;

  // Responsive camera smoothing
  smoothSpeed = 0.15f;
  currentYaw = 0.0f;

  // Initialize effects
  shakeTimer = 0.0f;
  shakeMagnitude = 0.0f;
  bobTimer = 0.0f;
  bobFrequency = 10.0f; // Footstep speed
  bobAmplitude = 0.1f;  // Bob height
}

Camera::~Camera() {}

void Camera::update(float playerX, float playerY, float playerZ,
                    float playerYaw, float deltaTime, bool isMoving) {
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

  // Update Shake
  if (shakeTimer > 0) {
    shakeTimer -= deltaTime;
    if (shakeTimer < 0)
      shakeTimer = 0;
  }

  // Update Bobbing
  if (isMoving && (mode == FIRST_PERSON || mode == THIRD_PERSON)) {
    bobTimer += deltaTime * bobFrequency;
  } else {
    // Reset bobbing to neutral when stopped
    bobTimer = 0.0f;
  }
}

void Camera::apply() {
  float shakeOffsetX = 0.0f;
  float shakeOffsetY = 0.0f;

  // Apply Shake
  if (shakeTimer > 0) {
    float progress = shakeTimer; // Fade out? Or constant?
    shakeOffsetX = ((float)rand() / RAND_MAX - 0.5f) * shakeMagnitude;
    shakeOffsetY = ((float)rand() / RAND_MAX - 0.5f) * shakeMagnitude;
  }

  // Apply Bobbing (Vertical only)
  float bobOffsetY = 0.0f;
  if (mode == FIRST_PERSON) {
    bobOffsetY = sin(bobTimer) * bobAmplitude;
  }

  gluLookAt(posX + shakeOffsetX, posY + shakeOffsetY + bobOffsetY, posZ,
            targetX + shakeOffsetX, targetY + shakeOffsetY + bobOffsetY,
            targetZ, upX, upY, upZ);
}

void Camera::triggerShake(float duration, float magnitude) {
  shakeTimer = duration;
  shakeMagnitude = magnitude;
}

void Camera::toggleMode() {
  if (mode == FIRST_PERSON) {
    mode = THIRD_PERSON;
    // Sync third-person camera to first-person yaw
    currentYaw = yaw;
  } else {
    mode = FIRST_PERSON;
    // Sync first-person yaw to third-person camera
    yaw = currentYaw;
    pitch = 0.0f;
  }
}

void Camera::updateMouse(int deltaX, int deltaY) {
  if (mode == FIRST_PERSON) {
    // Increased sensitivity for better responsiveness
    float sensitivity = 0.25f; // Increased from 0.12f

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
