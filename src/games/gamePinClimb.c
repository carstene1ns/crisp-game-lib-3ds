#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include "cglp.h"

static char *title = "PIN CLIMB";
static char *description = "[Hold] Stretch";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {};
static int charactersCount = 0;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 1, .isDarkColor = false};

typedef struct {
  Vector pos;
  bool isAlive;
} Pin;
typedef struct {
  float angle;
  float length;
  Pin *pin;
} Cord;
static Cord cord;
#define MAX_PIN_COUNT 32
static Pin pins[MAX_PIN_COUNT];
static int pinIndex;
static float nextPinDist;
#define CORD_LENGTH 7

static void addPin(float x, float y) {
  ASSIGN_ARRAY_ITEM(pins, pinIndex, Pin, p);
  p->pos.x = x;
  p->pos.y = y;
  p->isAlive = true;
  pinIndex = wrap(pinIndex + 1, 0, MAX_PIN_COUNT);
}

static void update() {
  if (!ticks) {
    INIT_UNALIVED_ARRAY(pins);
    pinIndex = 0;
    addPin(50, 5);
    nextPinDist = 5;
    cord.angle = 0;
    cord.length = CORD_LENGTH;
    cord.pin = &pins[0];
    barCenterPosRatio = 0;
  }
  float scr = difficulty * 0.02;
  if (cord.pin->pos.y < 80) {
    scr += (80 - cord.pin->pos.y) * 0.1;
  }
  if (input.isJustPressed) {
    play(SELECT);
  }
  if (input.isPressed) {
    cord.length += difficulty;
  } else {
    cord.length += (CORD_LENGTH - cord.length) * 0.1;
  }
  cord.angle += difficulty * 0.05;
  bar(cord.pin->pos.x, cord.pin->pos.y, cord.length, cord.angle);
  if (cord.pin->pos.y > 98) {
    play(EXPLOSION);
    gameOver();
  }
  Pin *nextPin = NULL;
  FOR_EACH(pins, i) {
    ASSIGN_ARRAY_ITEM(pins, i, Pin, p);
    SKIP_IS_NOT_ALIVE(p);
    p->pos.y += scr;
    if (box(p->pos.x, p->pos.y, 3, 3).isColliding.rect[BLACK] &&
        p != cord.pin) {
      nextPin = p;
    }
    p->isAlive = p->pos.y <= 102;
  }
  if (nextPin != NULL) {
    play(POWER_UP);
    addScore(ceil(distanceTo(&cord.pin->pos, nextPin->pos.x, nextPin->pos.y)),
             nextPin->pos.x, nextPin->pos.y);
    cord.pin = nextPin;
    cord.length = CORD_LENGTH;
  }
  nextPinDist -= scr;
  while (nextPinDist < 0) {
    addPin(rnd(10, 90), -2 - nextPinDist);
    nextPinDist += rnd(5, 15);
  }
}

void addGamePinCLimb() {
  addGame(title, description, characters, charactersCount, options, update);
}