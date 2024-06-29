#ifndef STEW
#define STEW
#include "raylib.h"

typedef unsigned char uint8;
typedef char sint8;
typedef unsigned short uint16;
typedef short sint16;
typedef unsigned int uint32;
typedef int sint32;
typedef bool boolean;


typedef struct{
    Vector3 pos;
    float radius;
    float shaftDistance;
} Base;

typedef struct {
    Vector3 pos;
    float radius;
    float anchorDistance;
} Platform;

typedef struct
{
    Vector3 baseJoint;
    Vector3 platformJoint;
    float motorRot;
    float angle;
} Legs;

typedef struct {
    Vector3 pos;
    Vector3 translation;
    Quaternion orientation;
    float legsLength;
    float hornLength;
    float rodLength;
    Legs legs[6];
    Vector3 B[6];
    Vector3 P[6];
    Vector3 q[6];
    Vector3 l[6];
    Vector3 H[6];
    Vector3 T0;
    float sinBeta[6];
    float cosBeta[6];
    Base base;
    Platform plat;
} cTewart;

// cTewart stewartPlatform;

typedef enum {
    WOBBLE,
    ROTATE,
    TILT
} AnimationType;

typedef struct {
    float pct;
    float dtFactor;
    AnimationType animationTypeActive;
    bool animationTypeEditMode;
} Animation;

// Animation animation;

extern cTewart stewartPlatform;
extern Animation animation;

#define THIS stewartPlatform
#define BASE (THIS.base)
#define PLAT (THIS.plat)
#define zero_vec (Vector3){0.0f, 0.0f, 0.0f}
#define zero_qua (Quaternion){0.0f, 0.0f, 0.0f, 0.0f}
#define ANIM animation


void InitStewart(cTewart *stewartPlatform);

void InitAnimation(Animation *a);

void RunAnimation();

void UpdateStewart(Vector3 translation, Quaternion orientation);

void _drawFrame(Vector3 refPos);

void DrawStewart();

void IncrAnimationTime();

#endif