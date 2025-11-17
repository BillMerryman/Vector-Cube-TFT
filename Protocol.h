// Protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef uint16_t TransmissionType;

const TransmissionType _Animation = 1;
const TransmissionType _Image = 2;

typedef uint16_t PrimitiveType;

const PrimitiveType _Line = 10;
const PrimitiveType _Triangle = 25;
const PrimitiveType _RoundRect = 35;
const PrimitiveType _RotatedRect = 45;
const PrimitiveType _Circle = 55;
const PrimitiveType _QuarterCircle = 65;

typedef struct __attribute__((packed)){
  int16_t x0;
  int16_t y0;
  int16_t x1;
  int16_t y1;
  uint16_t color;
} Line;

typedef struct __attribute__((packed)){
  int16_t x0;
  int16_t y0;
  int16_t w;
  int16_t h;
  uint16_t color;
} Rect;

typedef struct __attribute__((packed)){
  int16_t x0;
  int16_t y0;
  int16_t w;
  int16_t h;
  int16_t radius;
  uint16_t color;
} RoundRect;

typedef struct __attribute__((packed)){
  int16_t cenX;
  int16_t cenY;
  int16_t w;
  int16_t h;
  int16_t angleDeg;
  uint16_t color;
} RotatedRect;

typedef struct __attribute__((packed)){
  int16_t x0;
  int16_t y0;
  int16_t x1;
  int16_t y1;
  int16_t x2;
  int16_t y2;
  uint16_t color;
} Triangle;

typedef struct __attribute__((packed)){
  int16_t x0;
  int16_t y0;
  int16_t r;
  uint16_t color;
} Circle;

typedef struct __attribute__((packed)){
  int16_t x0;
  int16_t y0;
  int16_t r;
  uint8_t quadrants;
  int16_t delta;
  uint16_t color;
} QuarterCircle;

typedef struct __attribute__((packed)){
  PrimitiveType type;
  union{
    Line line;
    RoundRect roundRect;
    RotatedRect rotatedRect;
    Triangle triangle;
    Circle circle;
    QuarterCircle quarterCircle;
  };
} Primitive;

typedef struct  __attribute__((packed)){
  uint32_t duration;
  uint16_t fillColor;
  uint16_t primitiveCount;
  Primitive primitives[12];
} AnimationFrame;

typedef struct  __attribute__((packed)){
  uint16_t frameCount;
  AnimationFrame frames[12];
} Animation;

#endif
