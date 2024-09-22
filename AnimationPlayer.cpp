#include "AnimationPlayer.h"

AnimationPlayer::AnimationPlayer(Adafruit_ST7789& _tft):tft(_tft)
{
}

void AnimationPlayer::start(const uint8_t _animation[], unsigned long currentTime)
{
  if(isPlaying) return;
  memcpy(&animation, _animation, sizeof(Animation));
  if (animation.frameCount==0) return;
  currentFrameNumber = 0;
  drawFrame(animation.frames[currentFrameNumber]);
  isPlaying = true;
}

void AnimationPlayer::update(unsigned long currentTime)
{
  if(!isPlaying) return;
  if(currentTime < currentFrameExpiration) return;
  if(currentFrameNumber < animation.frameCount - 1)
  {
      currentFrameNumber++;
      currentFrameExpiration = currentTime + animation.frames[currentFrameNumber].duration;
      drawFrame(animation.frames[currentFrameNumber]);
  }
  else
  {
      currentFrameNumber = 0;
      isPlaying = false;
  }
}

void AnimationPlayer::drawFrame(AnimationFrame &frame)
{
      canvas.fillScreen(frame.fillColor);
      for(int primitiveCounter = 0; primitiveCounter < frame.primitiveCount; primitiveCounter++){
        Primitive primitive = frame.primitives[primitiveCounter];
        switch(primitive.type){
          case _Circle:
            Circle circle;
            circle = primitive.circle;
            canvas.fillCircle(circle.x0, circle.y0, circle.r, circle.color);
            break;
          case _QuarterCircle:
            QuarterCircle quarterCircle;
            quarterCircle = primitive.quarterCircle;
            canvas.fillCircleHelper(quarterCircle.x0, quarterCircle.y0, quarterCircle.r, quarterCircle.quadrants, quarterCircle.delta, quarterCircle.color);
            break;
          case _Triangle:
            Triangle triangle;
            triangle = primitive.triangle;
            canvas.fillTriangle(triangle.x0, triangle.y0, triangle.x1, triangle.y1, triangle.x2, triangle.y2, triangle.color);
            break;
          case _RoundRect:
            RoundRect roundRect;
            roundRect = primitive.roundRect;
            canvas.fillRoundRect(roundRect.x0, roundRect.y0, roundRect.w, roundRect.h, roundRect.radius, roundRect.color);
            break;
          case _Line:
            Line line;
            line = primitive.line;
            canvas.drawLine(line.x0, line.y0, line.x1, line.y1, line.color);
            break;
        }
      }
      tft.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);
}

