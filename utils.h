// ============================================================================
// Utils.h - Utility Structures and Helper Functions
// Contains: Vec3, Lighting, Collision, TrapType, ObstacleType enums
// ============================================================================

#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// ============================================================================
// MATH STRUCTURES
// ============================================================================

struct Vec3 {
  float x, y, z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(float px, float py, float pz) : x(px), y(py), z(pz) {}

  float length() const { return sqrt(x * x + y * y + z * z); }

  Vec3 normalized() const {
    float len = length();
    if (len > 0.0f) {
      return Vec3(x / len, y / len, z / len);
    }
    return Vec3(0, 0, 0);
  }

  float distanceTo(const Vec3 &other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
  }
};

// ============================================================================
// LIGHTING STRUCTURES
// ============================================================================

#include <array>

struct LightSource {
  std::array<float, 4> position; // {x, y, z, w}
  std::array<float, 4> ambient;  // {r, g, b, a}
  std::array<float, 4> diffuse;  // {r, g, b, a}
  std::array<float, 4> specular; // {r, g, b, a}

  LightSource() {
    position = {0, 10, 0, 1};
    ambient = {0.2f, 0.2f, 0.2f, 1.0f};
    diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
    specular = {1.0f, 1.0f, 1.0f, 1.0f};
  }
};

// ============================================================================
// ENUMERATIONS
// ============================================================================

enum TrapType { SPIKE_TRAP, FALLING_ICICLE };

enum ObstacleType {
  PILLAR,
  TREE,
  ICE_PILLAR,
  WALL,
  ROCK,
  CACTUS,
  PYRAMID,
  SPHINX,
  CHRISTMAS_TREE,
  PILLAR_ASSET // NEW for model-based pillars
};

// ============================================================================
// COLLISION HELPERS
// ============================================================================

inline bool checkSphereCollision(float x1, float y1, float z1, float r1,
                                 float x2, float y2, float z2, float r2) {
  float dx = x1 - x2;
  float dy = y1 - y2;
  float dz = z1 - z2;
  float distSq = dx * dx + dy * dy + dz * dz;
  float radiusSum = r1 + r2;
  return distSq <= (radiusSum * radiusSum);
}

inline bool checkAABBCollision(float x1, float y1, float z1, float w1, float h1,
                               float d1, float x2, float y2, float z2, float w2,
                               float h2, float d2) {
  return (fabs(x1 - x2) < (w1 + w2) / 2.0f) &&
         (fabs(y1 - y2) < (h1 + h2) / 2.0f) &&
         (fabs(z1 - z2) < (d1 + d2) / 2.0f);
}

// ============================================================================
// MATH HELPERS
// ============================================================================

inline float clamp(float value, float min, float max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

inline float randomFloat(float min, float max) {
  return min + (rand() / (float)RAND_MAX) * (max - min);
}

inline int randomInt(int min, int max) {
  return min + (rand() % (max - min + 1));
}

// ============================================================================
// COLOR HELPERS
// ============================================================================

struct Color {
  float r, g, b, a;

  Color() : r(1), g(1), b(1), a(1) {}
  Color(float red, float green, float blue, float alpha = 1.0f)
      : r(red), g(green), b(blue), a(alpha) {}

  Color lerp(const Color &other, float t) const {
    return Color(::lerp(r, other.r, t), ::lerp(g, other.g, t),
                 ::lerp(b, other.b, t), ::lerp(a, other.a, t));
  }
};

// Predefined colors
namespace Colors {
const Color White(1.0f, 1.0f, 1.0f);
const Color Black(0.0f, 0.0f, 0.0f);
const Color Red(1.0f, 0.0f, 0.0f);
const Color Green(0.0f, 1.0f, 0.0f);
const Color Blue(0.0f, 0.0f, 1.0f);
const Color Yellow(1.0f, 1.0f, 0.0f);
const Color Cyan(0.0f, 1.0f, 1.0f);
const Color Magenta(1.0f, 0.0f, 1.0f);
const Color Orange(1.0f, 0.5f, 0.0f);
const Color Gold(1.0f, 0.84f, 0.0f);
const Color Silver(0.75f, 0.75f, 0.75f);
const Color Brown(0.6f, 0.4f, 0.2f);
const Color Sand(0.76f, 0.7f, 0.5f);
const Color Ice(0.7f, 0.85f, 0.95f);
} // namespace Colors

// ============================================================================
// TEXTURE LOADING (Placeholder - implement if needed)
// ============================================================================

struct Texture {
  unsigned int id;
  int width;
  int height;

  Texture() : id(0), width(0), height(0) {}
};

// Simple BMP loader
inline Texture loadBMP(const char *filename) {
  Texture tex;
  FILE *file = fopen(filename, "rb");
  if (!file) {
    printf("Image could not be opened: %s\n", filename);
    return tex;
  }

  unsigned char header[54];
  if (fread(header, 1, 54, file) != 54) {
    printf("Not a correct BMP file: %s\n", filename);
    fclose(file);
    return tex;
  }

  if (header[0] != 'B' || header[1] != 'M') {
    printf("Not a correct BMP file: %s\n", filename);
    fclose(file);
    return tex;
  }

  unsigned int dataPos = *(int *)&(header[0x0A]);
  unsigned int imageSize = *(int *)&(header[0x22]);
  unsigned int width = *(int *)&(header[0x12]);
  unsigned int height = *(int *)&(header[0x16]);

  if (imageSize == 0)
    imageSize = width * height * 3;
  if (dataPos == 0)
    dataPos = 54;

  unsigned char *data = new unsigned char[imageSize];
  fread(data, 1, imageSize, file);
  fclose(file);

  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR,
               GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  tex.width = width;
  tex.height = height;

  delete[] data;
  printf("Loaded texture: %s\n", filename);
  return tex;
}

// ============================================================================
// DRAWING HELPERS
// ============================================================================

inline void drawGrid(float size, float step, float y = 0.0f) {
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glColor3f(0.3f, 0.3f, 0.3f);
  glBegin(GL_LINES);
  for (float i = -size; i <= size; i += step) {
    // Lines parallel to X axis
    glVertex3f(-size, y, i);
    glVertex3f(size, y, i);
    // Lines parallel to Z axis
    glVertex3f(i, y, -size);
    glVertex3f(i, y, size);
  }
  glEnd();
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

inline void drawAxis(float length = 5.0f) {
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glLineWidth(3.0f);
  glBegin(GL_LINES);
  // X axis - Red
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(0, 0, 0);
  glVertex3f(length, 0, 0);

  // Y axis - Green
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0, 0, 0);
  glVertex3f(0, length, 0);

  // Z axis - Blue
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, length);
  glEnd();
  glLineWidth(1.0f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

inline void drawBoundingBox(float x, float y, float z, float w, float h,
                            float d) {
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 0.0f, 0.0f);
  glLineWidth(2.0f);

  // float halfW = w / 2.0f;
  // float halfD = d / 2.0f;

  glPushMatrix();
  glTranslatef(x, y + h / 2, z);
  glScalef(w, h, d);
  glutWireCube(1.0f);
  glPopMatrix();

  glLineWidth(1.0f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

inline void drawBoundingSphere(float x, float y, float z, float radius) {
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glColor3f(0.0f, 1.0f, 0.0f);
  glPushMatrix();
  glTranslatef(x, y, z);
  glutWireSphere(radius, 16, 16);
  glPopMatrix();
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

// ============================================================================
// PARTICLE SYSTEM (Simple)
// ============================================================================

struct Particle {
  Vec3 position;
  Vec3 velocity;
  Color color;
  float life;
  float maxLife;
  float size;

  Particle() : life(0), maxLife(1), size(0.1f) {}

  void update(float deltaTime) {
    if (life > 0) {
      position.x += velocity.x * deltaTime;
      position.y += velocity.y * deltaTime;
      position.z += velocity.z * deltaTime;

      velocity.y -= 9.8f * deltaTime; // Gravity
      life -= deltaTime;
    }
  }

  bool isAlive() const { return life > 0; }
};

// ============================================================================
// SOUND EFFECTS
// ============================================================================

enum SoundEffect {
  SOUND_COLLECT_ORB,
  SOUND_CHEST_OPEN,
  SOUND_ICICLE_CRACK,
  SOUND_ICICLE_FALL,
  SOUND_DAMAGE,
  SOUND_FOOTSTEP,
  SOUND_JUMP,
  SOUND_PORTAL_ACTIVATE,
  SOUND_VICTORY,
  SOUND_ENEMY_GROWL
};

inline void playSound(SoundEffect sound) {
#ifdef __APPLE__
  // Simple macOS sound player using system command
  // In a real engine, use OpenAL or irrKlang
  const char *soundFile = "";

  switch (sound) {
  case SOUND_COLLECT_ORB:
    soundFile = "assets/collect.wav";
    break;
  case SOUND_CHEST_OPEN:
    soundFile = "assets/chest.wav";
    break;
  case SOUND_ICICLE_CRACK:
    soundFile = "assets/crack.wav";
    break;
  case SOUND_ICICLE_FALL:
    soundFile = "assets/fall.wav";
    break;
  case SOUND_DAMAGE:
    soundFile = "assets/damage.wav";
    break;
  case SOUND_FOOTSTEP:
    return; // Too frequent for system()
  case SOUND_JUMP:
    soundFile = "assets/jump.wav";
    break;
  case SOUND_PORTAL_ACTIVATE:
    soundFile = "assets/portal.wav";
    break;
  case SOUND_VICTORY:
    soundFile = "assets/win.wav";
    break;
  case SOUND_ENEMY_GROWL:
    soundFile = "assets/growl.wav";
    break;
  }

  if (soundFile[0] != '\0') {
    char command[256];
    sprintf(command, "afplay %s &", soundFile);
    system(command);
  }
#endif
}

// ============================================================================
// TIMER UTILITY
// ============================================================================

class GameTimer {
private:
  float elapsed;
  float duration;
  bool running;

public:
  GameTimer(float durationSeconds = 0.0f)
      : elapsed(0), duration(durationSeconds), running(false) {}

  void start() {
    running = true;
    elapsed = 0;
  }
  void stop() { running = false; }
  void reset() { elapsed = 0; }

  void update(float deltaTime) {
    if (running) {
      elapsed += deltaTime;
    }
  }

  float getElapsed() const { return elapsed; }
  float getRemaining() const { return duration - elapsed; }
  bool isFinished() const { return elapsed >= duration; }
  bool isRunning() const { return running; }

  float getProgress() const {
    if (duration <= 0)
      return 1.0f;
    return clamp(elapsed / duration, 0.0f, 1.0f);
  }
};

// ============================================================================
// DEBUG HELPERS
// ============================================================================

inline void printVec3(const char *label, const Vec3 &v) {
  printf("%s: (%.2f, %.2f, %.2f)\n", label, v.x, v.y, v.z);
}

inline void printFloat(const char *label, float value) {
  printf("%s: %.2f\n", label, value);
}

// ============================================================================
// INTERPOLATION HELPERS
// ============================================================================

inline float smoothStep(float t) { return t * t * (3.0f - 2.0f * t); }

inline float smootherStep(float t) {
  return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline float easeInQuad(float t) { return t * t; }

inline float easeOutQuad(float t) { return t * (2.0f - t); }

inline float easeInOutQuad(float t) {
  return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

#endif // UTILS_H