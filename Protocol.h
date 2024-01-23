// Protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef uint16_t TransmissionType;

const TransmissionType _Animation = 1;
const TransmissionType _Image = 2;

typedef uint16_t PrimitiveType;

const PrimitiveType _Circle = 1;
const PrimitiveType _QuarterCircle = 2;
const PrimitiveType _Triangle = 3;
const PrimitiveType _RoundRect = 4;
const PrimitiveType _Line = 5;

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
  int16_t w;
  int16_t h;
  int16_t radius;
  uint16_t color;
} RoundRect;

typedef struct __attribute__((packed)){
  int16_t x0;
  int16_t y0;
  int16_t x1;
  int16_t y1;
  uint16_t color;
} Line;

typedef struct __attribute__((packed)){
  PrimitiveType type;
  union{
    Circle circle;
    QuarterCircle quarterCircle;
    Triangle triangle;
    RoundRect roundRect;
    Line line;
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
