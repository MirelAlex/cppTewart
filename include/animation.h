#pragma once

#include <cmath> // For cos and sin functions
#include "raylib.h" // For Vector3 and Quaternion
#include "raymath.h"
#include "stewart_platform.h" // Include StewartPlatform class
#include <iostream>
#include <stdexcept>
#include <vector>

enum class AnimationType {
    ROTATE,
    WOBBLE,
    TILT,
    LISSAJOUS,
    length
};

struct PathPoint {
    Vector3 position;
};


class Animation {
public:
    Animation(StewartPlatform* platform);
    void runAnimation();
    void drawPath();

    // float getPct() const;
    // void setPct(float pct);

    // float getDtFactor() const;
    void setDtFactor(float dtFactor);

    // AnimationType getAnimationTypeActive() const;
    void setAnimationTypeActive(AnimationType animationTypeActive);
    void setAnimationDuration(float duration);

    // bool isAnimationTypeEditMode() const;
    // void setAnimationTypeEditMode(bool animationTypeEditMode);

    StewartPlatform* platform; // Pointer to the associated StewartPlatform
private:

    Vector3 translation;
    Quaternion orientation;

    float pct;
    float dtFactor;
    float duration;
    AnimationType animationTypeActive;
    bool animationShouldCycle;
    bool animationTypeEditMode;

    std::vector<Vector3> pathBuffer;
    void addPathPoint(const Vector3& position);
    void clearPath();

    void applyTransformationToPlatform(Vector3 T, Quaternion O);
    void incrAnimationTime();

};
