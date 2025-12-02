// ============================================================================
// main.cpp - Shadow Temple Escape
// Entry Point, Game Loop, and Core Game Management
// ============================================================================

#include "camera.h"
#include "level.h"
#include "player.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdlib>
#include <ctime>

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Game state
enum GameState { MENU, LEVEL1, LEVEL2, PAUSED, WIN, GAME_OVER };
GameState currentState = MENU;

// Core game objects
Camera *camera = nullptr;
Player *player = nullptr;
Level *currentLevel = nullptr;

// Input state
bool keys[256] = {false};
bool specialKeys[256] = {false};
int lastMouseX = WINDOW_WIDTH / 2;
int lastMouseY = WINDOW_HEIGHT / 2;

// Timing
int lastFrameTime = 0;
float deltaTime = 0.0f;

// Menu selection
int menuSelection = 0;

// ============================================================================
// INITIALIZATION
// ============================================================================
void initOpenGL() {
  glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);

  // Enable texturing
  glEnable(GL_TEXTURE_2D);

  // Fog for atmosphere (optional)
  glEnable(GL_FOG);
  GLfloat fogColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_START, 50.0f);
  glFogf(GL_FOG_END, 150.0f);
}

// Cleanup function to stop background music on exit
void cleanupMusic() {
#ifdef __APPLE__
  system("killall afplay 2>/dev/null");
#endif
}

void startGame() {
  initOpenGL();
  srand(time(NULL));

  // Register cleanup function to stop music on exit
  atexit(cleanupMusic);

  // Start background music (continuous loop with low volume)
#ifdef __APPLE__
  system("killall afplay 2>/dev/null");
  system("(while true; do afplay assets/background.wav -v 0.3; sleep 0.1; "
         "done) >/dev/null 2>&1 &");
#endif

  // Initialize camera
  camera = new Camera();

  // Initialize player at starting position
  player = new Player(0.0f, 1.0f, 0.0f);
  player->loadModel("assets/player.obj");

  // Load Level 1
  currentLevel = new DesertLevel();
  currentLevel->init(player);

  currentState = LEVEL1;
}

void nextLevel() {
  if (currentState == LEVEL1) {
    delete currentLevel;
    currentLevel = new IceLevel();
    currentLevel->init(player);
    player->resetPosition(0.0f, 1.0f, 0.0f);
    currentState = LEVEL2;
  } else if (currentState == LEVEL2) {
    currentState = WIN;
  }
}

void restartLevel() {
  if (currentLevel) {
    currentLevel->reset();

    // Reset player to initial spawn of the current level
    if (currentState == LEVEL1) {
      player->resetPosition(0.0f, 1.0f, 0.0f);
    } else if (currentState == LEVEL2) {
      // Keep player stats if just restarting level 2?
      // Or full reset? User said "restart button not working well".
      // Usually restart means try again.
      player->resetPosition(0.0f, 1.0f, 0.0f);
      // If it's a full game restart, we should reset health too.
      // But 'R' usually means "retry level".
      // Let's ensure health is reset if they died.
      if (!player->isAlive()) {
        player->reset();
      }
    }

    // Reset camera
    camera->setMode(THIRD_PERSON);
  }
}

void cleanup() {
  if (camera)
    delete camera;
  if (player)
    delete player;
  if (currentLevel)
    delete currentLevel;

  Level::cleanupCommonAssets();
}

// ============================================================================
// UPDATE LOGIC
// ============================================================================
void update(int value) {
  // Calculate delta time
  int currentTime = glutGet(GLUT_ELAPSED_TIME);
  deltaTime = (currentTime - lastFrameTime) / 1000.0f;
  lastFrameTime = currentTime;

  // Cap delta time to prevent large jumps
  if (deltaTime > 0.05f)
    deltaTime = 0.05f;

  if (currentState == LEVEL1 || currentState == LEVEL2) {
    // Handle player input
    float forward = 0.0f, strafe = 0.0f;

    if (keys['w'] || keys['W'] || specialKeys[GLUT_KEY_UP])
      forward += 1.0f; // UP arrow = forward
    if (keys['s'] || keys['S'] || specialKeys[GLUT_KEY_DOWN])
      forward -= 1.0f; // DOWN arrow = backward
    if (keys['a'] || keys['A'] || specialKeys[GLUT_KEY_LEFT])
      strafe -= 1.0f;
    if (keys['d'] || keys['D'] || specialKeys[GLUT_KEY_RIGHT])
      strafe += 1.0f;

    // Update player
    if (camera->getMode() == FIRST_PERSON) {
      player->setYaw(camera->getYaw());
    }
    player->move(forward, strafe, deltaTime);
    player->update(deltaTime);

    // Update level
    currentLevel->update(deltaTime);

    // Update camera
    camera->update(player->getX(), player->getY(), player->getZ(),
                   player->getYaw(), deltaTime);

    // Check win condition
    if (currentLevel->isComplete()) {
      nextLevel();
    }

    // Check game over
    if (!player->isAlive()) {
      currentState = GAME_OVER;
    }
  }

  glutPostRedisplay();
  glutTimerFunc(16, update, 0); // ~60 FPS
}

// ============================================================================
// RENDERING
// ============================================================================
void renderText(float x, float y, const char *text,
                void *font = GLUT_BITMAP_HELVETICA_18) {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glColor3f(1.0f, 1.0f, 1.0f);

  glRasterPos2f(x, y);
  for (const char *c = text; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void renderMenu() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderText(WINDOW_WIDTH / 2.0f - 150, WINDOW_HEIGHT / 2.0f + 100,
             "SHADOW TEMPLE ESCAPE", GLUT_BITMAP_TIMES_ROMAN_24);

  glColor3f(menuSelection == 0 ? 1.0f : 0.7f, 0.8f, 0.2f);
  renderText(WINDOW_WIDTH / 2.0f - 80, WINDOW_HEIGHT / 2.0f,
             "Start Game (ENTER)");

  glColor3f(menuSelection == 1 ? 1.0f : 0.7f, 0.8f, 0.2f);
  renderText(WINDOW_WIDTH / 2.0f - 80, WINDOW_HEIGHT / 2.0f - 40, "Quit (ESC)");

  glColor3f(0.6f, 0.6f, 0.6f);
  renderText(WINDOW_WIDTH / 2.0f - 200, 100,
             "Controls: WASD/Arrows-Move | SPACE-Jump | C-Camera | E-Interact");
}

void renderHUD() {
  // Setup 2D projection
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  char buffer[64];

  // --- Top Left: Level Info ---
  // Background box
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
  glBegin(GL_QUADS);
  glVertex2f(10, WINDOW_HEIGHT - 10);
  glVertex2f(250, WINDOW_HEIGHT - 10);
  glVertex2f(250, WINDOW_HEIGHT - 80);
  glVertex2f(10, WINDOW_HEIGHT - 80);
  glEnd();
  glDisable(GL_BLEND);

  // Border
  glColor3f(1.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(10, WINDOW_HEIGHT - 10);
  glVertex2f(250, WINDOW_HEIGHT - 10);
  glVertex2f(250, WINDOW_HEIGHT - 80);
  glVertex2f(10, WINDOW_HEIGHT - 80);
  glEnd();

  // Text
  glColor3f(1.0f, 1.0f, 1.0f);
  if (currentState == LEVEL1) {
    DesertLevel *desert = (DesertLevel *)currentLevel;
    renderText(20, WINDOW_HEIGHT - 35, "Level 1: Desert Temple");
    sprintf(buffer, "Orbs: %d / %d", player->getOrbsCollected(),
            desert->getTotalOrbs());
    glColor3f(1.0f, 0.84f, 0.0f); // Gold
    renderText(20, WINDOW_HEIGHT - 60, buffer);
  } else if (currentState == LEVEL2) {
    IceLevel *ice = (IceLevel *)currentLevel;
    renderText(20, WINDOW_HEIGHT - 35, "Level 2: Ice Cave");
    float timeLeft = ice->getTimeRemaining();
    sprintf(buffer, "Time: %.1f", timeLeft);

    if (timeLeft < 10.0f)
      glColor3f(1.0f, 0.2f, 0.2f);
    else if (timeLeft < 20.0f)
      glColor3f(1.0f, 0.6f, 0.0f);
    else
      glColor3f(0.6f, 0.8f, 1.0f);

    renderText(20, WINDOW_HEIGHT - 60, buffer);
  }

  // --- Bottom Left: Health Bar ---
  float healthPercent = (float)player->getHealth() / 100.0f;

  // Bar Background
  glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
  glBegin(GL_QUADS);
  glVertex2f(20, 20);
  glVertex2f(220, 20);
  glVertex2f(220, 50);
  glVertex2f(20, 50);
  glEnd();

  // Health Fill
  if (healthPercent > 0.5f)
    glColor3f(0.2f, 0.8f, 0.2f);
  else if (healthPercent > 0.2f)
    glColor3f(0.9f, 0.6f, 0.1f);
  else
    glColor3f(0.9f, 0.1f, 0.1f);

  glBegin(GL_QUADS);
  glVertex2f(22, 22);
  glVertex2f(22 + 196 * healthPercent, 22);
  glVertex2f(22 + 196 * healthPercent, 48);
  glVertex2f(22, 48);
  glEnd();

  // Bar Border
  glColor3f(0.8f, 0.8f, 0.8f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(20, 20);
  glVertex2f(220, 20);
  glVertex2f(220, 50);
  glVertex2f(20, 50);
  glEnd();

  sprintf(buffer, "%d%%", player->getHealth());
  glColor3f(1.0f, 1.0f, 1.0f);
  renderText(230, 28, buffer);

  // --- Top Right: Camera Mode ---
  glColor3f(0.8f, 0.8f, 0.8f);
  renderText(WINDOW_WIDTH - 220, WINDOW_HEIGHT - 30,
             camera->getMode() == FIRST_PERSON ? "[C] First Person"
                                               : "[C] Third Person");

  // --- Damage Overlay (Red Flash) ---
  float flash = player->getDamageFlashTimer();
  if (flash > 0.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.0f, 0.0f, flash * 1.5f); // Fade out
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    glDisable(GL_BLEND);
  }

  // Restore state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void renderPaused() {
  renderText(WINDOW_WIDTH / 2.0f - 50, WINDOW_HEIGHT / 2.0f, "PAUSED",
             GLUT_BITMAP_TIMES_ROMAN_24);
  renderText(WINDOW_WIDTH / 2.0f - 100, WINDOW_HEIGHT / 2.0f - 50,
             "Press ESC to Resume");
  renderText(WINDOW_WIDTH / 2.0f - 80, WINDOW_HEIGHT / 2.0f - 80,
             "Press R to Restart");
}

void renderWin() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0f, 0.84f, 0.0f);
  renderText(WINDOW_WIDTH / 2.0f - 100, WINDOW_HEIGHT / 2.0f, "VICTORY!",
             GLUT_BITMAP_TIMES_ROMAN_24);
  renderText(WINDOW_WIDTH / 2.0f - 120, WINDOW_HEIGHT / 2.0f - 50,
             "You Escaped the Temple!");
  renderText(WINDOW_WIDTH / 2.0f - 150, WINDOW_HEIGHT / 2.0f - 100,
             "Press ENTER to Play Again");
}

void renderGameOver() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0f, 0.0f, 0.0f);
  renderText(WINDOW_WIDTH / 2.0f - 80, WINDOW_HEIGHT / 2.0f, "GAME OVER",
             GLUT_BITMAP_TIMES_ROMAN_24);
  renderText(WINDOW_WIDTH / 2.0f - 100, WINDOW_HEIGHT / 2.0f - 50,
             "Press R to Restart");
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (currentState == MENU) {
    renderMenu();
  } else if (currentState == WIN) {
    renderWin();
  } else if (currentState == GAME_OVER) {
    renderGameOver();
  } else if (currentState == LEVEL1 || currentState == LEVEL2) {
    // Setup 3D projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply camera
    camera->apply();

    // Render level
    currentLevel->render();

    // Render player (only in third person)
    if (camera->getMode() == THIRD_PERSON) {
      player->render();
    }

    // Render HUD
    renderHUD();

    // Render paused overlay
    if (currentState == PAUSED) {
      renderPaused();
    }
  }

  glutSwapBuffers();
}

// ============================================================================
// INPUT CALLBACKS
// ============================================================================
void keyboard(unsigned char key, int x, int y) {
  keys[key] = true;

  if (currentState == MENU) {
    if (key == 13) { // ENTER
      if (menuSelection == 0)
        startGame();
      else
        exit(0);
    }
    if (key == 27)
      exit(0); // ESC
  } else if (currentState == LEVEL1 || currentState == LEVEL2 ||
             currentState == PAUSED) {
    if (key == 27) { // ESC - toggle pause
      currentState = (currentState == PAUSED)
                         ? (currentLevel->isDesert() ? LEVEL1 : LEVEL2)
                         : PAUSED;
    }
    if (key == 'c' || key == 'C') {
      camera->toggleMode();
    }
    if (key == ' ') {
      player->jump();
    }
    if (key == 'e' || key == 'E') {
      currentLevel->interact(player->getX(), player->getY(), player->getZ());
    }
    if (key == 'r' || key == 'R') {
      restartLevel();
    }
  } else if (currentState == WIN) {
    if (key == 13) { // ENTER - restart
      cleanup();
      startGame();
    }
  } else if (currentState == GAME_OVER) {
    if (key == 'r' || key == 'R') {
      restartLevel();
      currentState = currentLevel->isDesert() ? LEVEL1 : LEVEL2;
    }
  }
}

void keyboardUp(unsigned char key, int x, int y) { keys[key] = false; }

void specialKey(int key, int x, int y) { specialKeys[key] = true; }

void specialKeyUp(int key, int x, int y) { specialKeys[key] = false; }

void mouse(int button, int state, int x, int y) {
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    camera->toggleMode();
  }
}

void mouseMotion(int x, int y) {
  if (currentState != LEVEL1 && currentState != LEVEL2)
    return;

  int centerX = WINDOW_WIDTH / 2;
  int centerY = WINDOW_HEIGHT / 2;

  int deltaX = x - centerX;
  int deltaY = y - centerY;

  if (deltaX == 0 && deltaY == 0)
    return;

  camera->updateMouse(deltaX, deltaY);

  // Re-center pointer for infinite look
  glutWarpPointer(centerX, centerY);
}

void reshape(int width, int height) {
  if (height == 0)
    height = 1;
  glViewport(0, 0, width, height);
}

// ============================================================================
// MAIN
// ============================================================================
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Shadow Temple Escape");

  initOpenGL();

  // Register callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutKeyboardUpFunc(keyboardUp);
  glutSpecialFunc(specialKey);
  glutSpecialUpFunc(specialKeyUp);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(mouseMotion);
  glutTimerFunc(0, update, 0);

  // Hide cursor for immersion
  glutSetCursor(GLUT_CURSOR_CROSSHAIR);

  lastFrameTime = glutGet(GLUT_ELAPSED_TIME);

  glutMainLoop();

  cleanup();
  return 0;
}