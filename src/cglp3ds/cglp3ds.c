
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <assert.h>

#include "cglp.h"
#include "machineDependent.h"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
static C3D_RenderTarget* top = NULL;
static struct timeval start;

static int view_width = SCREEN_WIDTH;
static int view_height = SCREEN_HEIGHT;

// library implementation
void md_drawRect(float x, float y, float w, float h, unsigned char r,
                         unsigned char g, unsigned char b) {

  if(x < 0.f || y < 0.f || w > view_width || h > view_height) {
    printf("invalid!\n");
    return;
  }

  C2D_DrawRectSolid(x, y, 0.4f, w, h, C2D_Color32(r, g, b, 0xFF));
}

void md_drawCharacter(unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3],
  float x, float y, int hash) {

  for (int i = 0; i < CHARACTER_HEIGHT; i++) {
    for (int j = 0; j < CHARACTER_WIDTH; j++) {
      unsigned char r = grid[i][j][0];
      unsigned char g = grid[i][j][1];
      unsigned char b = grid[i][j][2];

      if(r > 0 || g > 0 || b > 0)
        C2D_DrawRectSolid(x+j, y+i, 0.6f, 1.f, 1.f, C2D_Color32(r, g, b, 0xff));
    }
  }
}

void md_clearView(unsigned char r, unsigned char g, unsigned char b) {
  printf("view clear\n");

  if(view_width <= 0 || view_height <= 0) {
    printf("invalid!\n");
    return;
  }

  C2D_DrawRectSolid(0.f, 0.f, 0.1f, view_width, view_height, C2D_Color32(r, g, b, 0xFF));
}

void md_clearScreen(unsigned char r, unsigned char g, unsigned char b) {
  printf("screen clear\n");
  
  //C2D_DrawRectSolid(0.f, 0.f, 0.1f, SCREEN_WIDTH, SCREEN_HEIGHT, C2D_Color32(r, g, b, 0xFF));
}

void md_playTone(float freq, float duration, float when) {

}

void md_stopTone() {

}

float md_getAudioTime() {
  struct timeval now;

  gettimeofday(&now, NULL);
  u32 ticks = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;

  return (float)ticks;
}

void md_initView(int w, int h) {
  view_width = w;
  view_height = h;
  printf("view %dx%d\n", w, h);

  float factor = fminf(SCREEN_WIDTH/(float)view_width, SCREEN_HEIGHT/(float)view_height);
  printf("scale %fx%f\n", SCREEN_WIDTH/(float)view_width, SCREEN_HEIGHT/(float)view_height);

  float x = (SCREEN_WIDTH - view_width * factor) / 2.f;
  float y = (SCREEN_HEIGHT - view_height * factor) / 2.f;
  printf("trans %fx%f\n", x, y);

  C2D_ViewReset();
  C2D_ViewTranslate(x, y);
  C2D_ViewScale(factor, factor);
}

void md_consoleLog(char *msg) {
  puts(msg);
}
// end of library implementation

// main
int main(int argc, char **argv) {
  gfxInitDefault();
  C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
  C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
  C2D_Prepare();

  PrintConsole infoWindow, otherWindow;
  consoleInit(GFX_BOTTOM, &infoWindow);
  consoleInit(GFX_BOTTOM, &otherWindow);
  consoleSetWindow(&infoWindow, 1, 23, 46, 6);
  consoleSetWindow(&otherWindow, 1, 1, 46, 22);
  consoleSelect(&otherWindow);
  //consoleDebugInit(debugDevice_SVC);

  top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

  gettimeofday(&start, NULL);

  // init library
  disableSound();
  initGame();

  // main loop
  while(aptMainLoop()) {
    // update keys
    hidScanInput();
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();

    // return to hbmenu
    if (kDown & KEY_START)
      break;

    // back to game menu
    if (!isInMenu) {
      if (kDown & KEY_SELECT) {
        goToMenu();
      }
    }

    // push key states
    setButtonState(kHeld & KEY_LEFT, kHeld & KEY_RIGHT,
      kHeld & KEY_UP, kHeld & KEY_DOWN, kHeld & KEY_B, kHeld & KEY_A);

    // stats
    consoleSelect(&infoWindow);
    printf("\x1b[1;1H----------------\x1b[K");
    printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.f);
    printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.f);
    printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.f);
    consoleSelect(&otherWindow);

    // start rendering
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32f(0.f, 0.f, 0.f, 1.f));
    C2D_SceneBegin(top);

    // update logic
    updateFrame();

    // finish rendering
    C3D_FrameEnd(0);
  }

  C2D_Fini();
  C3D_Fini();
  gfxExit();
  return 0;
}
