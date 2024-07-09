
#include "stewart_platform.h"
#include <cmath>
#include "raymath.h"
#include "rlgl.h"
#include "imgui.h"

void getCurrentServoAngles(float* angles, int count){
    for (size_t i = 0; i < count; i++)
    {
        // TODO: maybe move angle somewhere else?
        // angles[i] = THIS.legs[i].angle;
        // TODO: limit the servo angle to a servo range
        // printf("[%d] %.2f\n",i, RAD2DEG * THIS.legs[i].angle );
    }
}
