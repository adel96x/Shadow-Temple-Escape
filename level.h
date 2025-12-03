// ============================================================================
// Level.h - Enhanced Level System with Professional Assets
// ============================================================================

#ifndef LEVEL_H
#define LEVEL_H

#include "model.h"
#include "player.h"
#include "utils.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <vector>

// ============================================================================
// ENTITY STRUCTURES
// ============================================================================

struct Collectible {
  float x, y, z;
  bool collected;
  float rotation;
  float bobPhase;
  float radius;

  Collectible(float px, float py, float pz)
      : x(px), y(py), z(pz), collected(false), rotation(0), bobPhase(0),
        radius(0.5f) {}
};

struct Enemy {
  float x, y, z;
  float rotation;
  float speed;
  int patrolIndex;
  std::vector<Vec3> patrolPoints;
  float radius;

  bool isHit;
  float hitTimer;
  float recoilDist;

  Enemy(float px, float py, float pz)
      : x(px), y(py), z(pz), rotation(0), speed(2.0f), patrolIndex(0),
        radius(0.7f), isHit(false), hitTimer(0), recoilDist(0) {}
};

struct Trap {
  float x, y, z;
  bool active;
  float timer;
  float radius;
  TrapType type;
  bool showWarning;
  float warningTime;

  Trap(float px, float py, float pz, TrapType t)
      : x(px), y(py), z(pz), active(false), timer(0), radius(1.0f), type(t),
        showWarning(false), warningTime(2.0f) {}
};

struct Torch {
  float x, y, z;
  float flickerOffset;

  Torch(float px, float py, float pz) : x(px), y(py), z(pz), flickerOffset(0) {}
};

struct Obstacle {
  float x, y, z;
  float width, height, depth;
  ObstacleType type;

  Obstacle(float px, float py, float pz, float w, float h, float d,
           ObstacleType t)
      : x(px), y(py), z(pz), width(w), height(h), depth(d), type(t) {}
};

struct Portal {
  float x, y, z;
  bool active;
  float rotation;
  float scale;
  float radius;

  Portal(float px, float py, float pz)
      : x(px), y(py), z(pz), active(false), rotation(0), scale(0.5f),
        radius(2.0f) {}
};

struct Chest {
  float x, y, z;
  bool opened;
  bool hasOrb;
  float lidAngle;

  Chest(float px, float py, float pz, bool orb)
      : x(px), y(py), z(pz), opened(false), hasOrb(orb), lidAngle(0) {}
};

// ============================================================================
// BASE LEVEL CLASS
// ============================================================================

class Level {
protected:
  Player *player;
  Portal *portal;
  std::vector<Collectible *> collectibles;
  std::vector<Enemy *> enemies;
  std::vector<Trap *> traps;
  std::vector<Obstacle *> obstacles;
  std::vector<Torch *> torches; // New torches vector
  bool levelComplete;

  // Lighting
  LightSource sunLight;
  std::vector<LightSource> lights;

  // Resources
  Texture wallTexture;
  Texture groundTexture;
  Model *sphinxModel;
  static Model *pillarModel;
  Model *treeModel;
  Model *rockModel;
  Model *groundModel;
  Model *cactusModel;  // NEW
  Model *pyramidModel; // NEW
  static Model *snowmanModel;
  static Model *christmasTreeModel;
  static Model *snakeModel;
  static Model *trapModel;
  static Model *chestModel;

public:
  static void cleanupCommonAssets();

  Level();
  virtual ~Level();

  virtual void init(Player *p) = 0;
  virtual void update(float deltaTime) = 0;
  virtual void render() = 0;
  virtual void reset() = 0;
  virtual void interact(float px, float py, float pz) = 0;
  virtual bool isDesert() const = 0;

  bool isComplete() const { return levelComplete; }

  // Common render helpers - UPDATED SIGNATURES
  void renderGround(float size, Texture &texture);
  void renderSkybox(float r, float g, float b);
  void renderWalls(float size, float height, Texture &texture);

  void loadCommonAssets();
};

// ============================================================================
// DESERT LEVEL (Level 1) - Enhanced
// ============================================================================

class DesertLevel : public Level {
private:
  int totalOrbs;
  std::vector<Chest *> chests;

  // Day/night cycle
  float timeOfDay;
  float daySpeed;

  // Desert-specific textures
  Texture sandTexture;
  Texture desertWallTexture;

public:
  DesertLevel();
  ~DesertLevel();

  void init(Player *p) override;
  void update(float deltaTime) override;
  void render() override;
  void reset() override;
  void interact(float px, float py, float pz) override;
  bool isDesert() const override { return true; }

  int getTotalOrbs() const { return totalOrbs; }

private:
  void spawnOrbs();
  void spawnChests();
  void spawnEnemies();
  void spawnObstacles();
  void updateDayNightCycle(float deltaTime);
  void checkOrbCollection();
  void checkChestInteraction(float px, float py, float pz);
  void updateEnemies(float deltaTime);
  void checkEnemyCollision();

  void renderDesertEnvironment();
  void renderPillar(float x, float y, float z);
  void renderPalmTree(float x, float y, float z);
  void renderCactus(float x, float y, float z); // NEW
  void renderPyramid(float x, float y, float z, float baseSize,
                     float height); // NEW
  void renderSphinx(float x, float y, float z, float scale = 1.0f);
  void renderRock(float x, float y, float z);
  void renderOrb(Collectible *orb);
  void renderChest(Chest *chest);
  void renderScorpion(Enemy *enemy);
  void renderPortal();
};

// ============================================================================
// ICE LEVEL (Level 2) - Enhanced
// ============================================================================

class IceLevel : public Level {
private:
  float survivalTimer;
  float maxTime;
  float icicleSpawnTimer;
  float icicleSpawnInterval;

  // Ice-specific textures
  Texture snowTexture;
  Texture iceWallTexture;

  struct Snowflake {
    float x, y, z;
    float speed;
  };
  std::vector<Snowflake> snowParticles;

public:
  IceLevel();
  ~IceLevel();

  void init(Player *p) override;
  void update(float deltaTime) override;
  void render() override;
  void reset() override;
  void interact(float px, float py, float pz) override;
  bool isDesert() const override { return false; }

  float getTimeRemaining() const { return maxTime - survivalTimer; }

private:
  void spawnEnemies();
  void spawnObstacles();
  void spawnIcicle();
  void updateTimer(float deltaTime);
  void updateIcicles(float deltaTime);
  void updateEnemies(float deltaTime);
  void checkEnemyCollision();
  void spawnSphinx();

  void renderIceEnvironment();
  void renderIcePillar(float x, float y, float z);
  void renderCrystal(float x, float y, float z);
  void renderIcicle(Trap *icicle);
  void renderWarningCircle(float x, float z, float radius);
  void renderIceElemental(Enemy *enemy);
  void renderPortal();
  void renderSnowman(float x, float y, float z);
  void renderTimer3D();
};

#endif // LEVEL_H