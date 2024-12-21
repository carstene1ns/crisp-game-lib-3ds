
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <assert.h>

#include "cglp.h"
#include "machineDependent.h"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
static C3D_RenderTarget* top = NULL;
//static struct timeval start;

static int view_width = SCREEN_WIDTH;
static int view_height = SCREEN_HEIGHT;
static float z = 0.5f;
static int a = 0xFF;

// library implementation
void md_drawRect(float x, float y, float w, float h, unsigned char r,
                 unsigned char g, unsigned char b) {
  // skip invalid
  if(x+w < 0.f || y+h < 0.f || x > view_width || y > view_height) {
    //puts("invalid rect!");
    return;
  }

  // sanitize
  while(x+w > view_width)
    w--;
  while(y+h > view_height)
    h--;
  while(x < 0.f) {
    x++;
    w--;
  }
  while(y < 0.f) {
    y++;
    h--;
  }
  if(w <= 0 || h <= 0)
    return;

  C2D_DrawRectSolid(x, y, z, w, h, C2D_Color32(r, g, b, a));
}

void md_drawCharacter(unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3],
                      float x, float y, int hash) {
  // skip invalid
  if(x+CHARACTER_WIDTH < 0.f || y+CHARACTER_HEIGHT < 0.f || x > view_width || y > view_height) {
    //puts("invalid rect!");
    return;
  }

  for(int i = 0; i < CHARACTER_HEIGHT; i++) {
    for(int j = 0; j < CHARACTER_WIDTH; j++) {
      unsigned char r = grid[i][j][0];
      unsigned char g = grid[i][j][1];
      unsigned char b = grid[i][j][2];

      // sanitize
      if(x+j < 0.f || y+i < 0.f || x+j > view_width-1 || y+i > view_height-1)
        continue;

      // skip transparent
      if(!r && !g && !b)
        continue;

      C2D_DrawRectSolid(x+j, y+i, z, 1.f, 1.f, C2D_Color32(r, g, b, a));
    }
  }
}

void md_clearView(unsigned char r, unsigned char g, unsigned char b) {
  // skip invalid
  if(view_width <= 0 || view_height <= 0) {
    puts("invalid clear!");
    return;
  }

  C2D_DrawRectSolid(0.f, 0.f, z, view_width, view_height, C2D_Color32(r, g, b, a));
}

void md_clearScreen(unsigned char r, unsigned char g, unsigned char b) {
  //C2D_DrawRectSolid(0.f, 0.f, z, SCREEN_WIDTH, SCREEN_HEIGHT, C2D_Color32(r, g, b, a));
  C2D_TargetClear(top, C2D_Color32(r, g, b, a));
}

void md_playTone(float freq, float duration, float when) {

}

void md_stopTone() {

}

float md_getAudioTime() {
  return (float)C3D_FrameCounter(0);

  //struct timeval now;

  //gettimeofday(&now, NULL);
  //u32 ticks = (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000;

  //return (float)ticks;
}

void md_initView(int w, int h) {
  view_width = w;
  view_height = h;

  // scale
  float f_x = SCREEN_WIDTH/(float)w;
  float f_y = SCREEN_HEIGHT/(float)h;
  float f = roundf(fminf(f_x, f_y));
  // center
  float x = (SCREEN_WIDTH - w * f) / 2.f;
  float y = (SCREEN_HEIGHT - h * f) / 2.f;

  printf("view %dx%d, scale %.2fx%.2f(%.2f), trans %.2fx%.2f\n",
    w, h, f_x, f_y, f, x, y);

  C2D_ViewReset();
  C2D_ViewTranslate(x, y);
  C2D_ViewScale(f, f);
  //C3D_SetScissor(GPU_SCISSOR_NORMAL, x, y, SCREEN_HEIGHT - x, SCREEN_WIDTH - y);
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
  consoleSetWindow(&infoWindow, 1, 23, 39, 6);
  consoleSetWindow(&otherWindow, 1, 1, 39, 22);
  consoleSelect(&otherWindow);
  //consoleDebugInit(debugDevice_SVC);

  top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

  //gettimeofday(&start, NULL);

  // init library
  //disableSound();
  initGame();

  // main loop
  while(aptMainLoop()) {
    // update keys
    hidScanInput();
    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();

    // return to hbmenu
    if(kDown & KEY_START)
      break;

    // back to game menu
    if(!isInMenu) {
      if(kDown & KEY_SELECT) {
        goToMenu();
      }
    }

    // push key states
    setButtonState(kHeld & KEY_LEFT, kHeld & KEY_RIGHT,
      kHeld & KEY_UP, kHeld & KEY_DOWN, kHeld & KEY_B,
      kHeld & (KEY_A|KEY_TOUCH));

    // stats
    consoleSelect(&infoWindow);
    printf("\x1b[1;1H----------------\x1b[K");
    printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.f);
    printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.f);
    printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.f);
    consoleSelect(&otherWindow);

    // start rendering
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_SceneBegin(top);

    // update logic
    updateFrame();

    // finish rendering
    C3D_FrameEnd(0);
  }

  // close services
  C2D_Fini();
  C3D_Fini();
  gfxExit();

  return 0;
}
