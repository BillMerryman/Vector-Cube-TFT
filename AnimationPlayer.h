#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "TFT.h"
#include "Protocol.h"
#include "AnimationFlatbuffer.h"

class AnimationPlayer
{
  public:
    AnimationPlayer(Adafruit_ST7789& _tft);
    void start(const uint8_t _animation[], size_t animationSize, unsigned long currentTime);
    void update(unsigned long currentTime);

  protected:
    void drawFrame(const AnimationFlatbuffer::AnimationFrameFB* frame);
    Adafruit_ST7789& tft;
    byte animation[1024];
    GFXcanvas16 canvas = GFXcanvas16(SCREEN_WIDTH, SCREEN_HEIGHT);
    bool isPlaying = false;
    unsigned int currentFrameNumber;
    unsigned long currentFrameExpiration;
    const AnimationFlatbuffer::AnimationFB* animationFB;
};