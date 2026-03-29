#include "AnimationPlayer.h"

#define QUARTER_CIRCLE_DELTA_TO_BE_REMOVED 0

using namespace AnimationFlatbuffer;

AnimationPlayer::AnimationPlayer(Adafruit_ST7789& _tft, GFXcanvas16& _canvas):tft(_tft), canvas(_canvas)
{
}

void AnimationPlayer::start(const uint8_t _animation[], unsigned long currentTime)
{
  if(isPlaying) return;
  animationFB = GetSizePrefixedAnimationFB(_animation);
  if (animationFB->frames()->size()==0) return;
  currentFrameNumber = 0;
  currentFrameExpiration = currentTime + animationFB->frames()->Get(currentFrameNumber)->duration();
  drawFrame(animationFB->frames()->Get(currentFrameNumber));
  isPlaying = true;
}

void AnimationPlayer::update(unsigned long currentTime)
{
  if(!isPlaying) return;
  if(currentTime < currentFrameExpiration) return;
  if(currentFrameNumber < animationFB->frames()->size() - 1)
  {
      currentFrameNumber++;
      currentFrameExpiration = currentTime + animationFB->frames()->Get(currentFrameNumber)->duration();
      drawFrame(animationFB->frames()->Get(currentFrameNumber));
  }
  else
  {
      currentFrameNumber = 0;
      isPlaying = false;
  }
}

void AnimationPlayer::drawFrame(const AnimationFlatbuffer::AnimationFrameFB* frame)
{
      canvas.fillScreen(frame->fill_color());
      for(int primitiveCounter = 0; primitiveCounter < frame->primitives()->size(); primitiveCounter++){
        PrimitiveFB primitiveType = frame->primitives_type()->GetEnum<PrimitiveFB>(primitiveCounter);
        const void* primitiveData = frame->primitives()->Get(primitiveCounter);
        switch(primitiveType)
        {
          case PrimitiveFB_LineFB:
          {
            const LineFB* lineFB = static_cast<const LineFB*>(primitiveData);
            int16_t x0 = animationFB->x0() + frame->x0() + lineFB->x0();
            int16_t y0 = animationFB->y0() + frame->y0() + lineFB->y0();
            int16_t x1 = animationFB->x0() + frame->x0() + lineFB->x1();
            int16_t y1 = animationFB->y0() + frame->y0() + lineFB->y1();
            canvas.drawLine(x0, y0, x1, y1, lineFB->color());
            break;
          }
          case PrimitiveFB_TriangleFB:
          {
            const TriangleFB* triangleFB = static_cast<const TriangleFB*>(primitiveData);
            int16_t x0 = animationFB->x0() + frame->x0() + triangleFB->x0();
            int16_t y0 = animationFB->y0() + frame->y0() + triangleFB->y0();
            int16_t x1 = animationFB->x0() + frame->x0() + triangleFB->x1();
            int16_t y1 = animationFB->y0() + frame->y0() + triangleFB->y1();
            int16_t x2 = animationFB->x0() + frame->x0() + triangleFB->x2();
            int16_t y2 = animationFB->y0() + frame->y0() + triangleFB->y2();
            canvas.fillTriangle(x0, y0, x1, y1, x2, y2, triangleFB->color());
            break;
          }
          case PrimitiveFB_RoundRectFB:
          {
            const RoundRectFB* roundRectFB = static_cast<const RoundRectFB*>(primitiveData);
            int16_t x0 = animationFB->x0() + frame->x0() + roundRectFB->x0();
            int16_t y0 = animationFB->y0() + frame->y0() + roundRectFB->y0();
            canvas.fillRoundRect(x0, y0, roundRectFB->w(), roundRectFB->h(), roundRectFB->radius(), roundRectFB->color());
            break;
          }
          case PrimitiveFB_RotatedRectFB:
          {
            const RotatedRectFB* rotatedRectFB = static_cast<const RotatedRectFB*>(primitiveData);
            int16_t x0 = animationFB->x0() + frame->x0() + rotatedRectFB->x0();
            int16_t y0 = animationFB->y0() + frame->y0() + rotatedRectFB->y0();
            canvas.fillRotatedRect(x0, y0, rotatedRectFB->w(), rotatedRectFB->h(), rotatedRectFB->angle_deg(), rotatedRectFB->color());
            break;
          }
          case PrimitiveFB_CircleFB:
          {
            const CircleFB* circleFB = static_cast<const CircleFB*>(primitiveData);
            int16_t x0 = animationFB->x0() + frame->x0() + circleFB->x0();
            int16_t y0 = animationFB->y0() + frame->y0() + circleFB->y0();
            canvas.fillQuarterCircle(x0, y0, circleFB->r(), circleFB->quadrants(), QUARTER_CIRCLE_DELTA_TO_BE_REMOVED, circleFB->color());
            break;
          }
        }
      }
      tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
}

