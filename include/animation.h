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
    void togglePathDrawing(){ bDrawPath = !bDrawPath;}
    boolean bDrawPath = false;

    StewartPlatform* getPlatform() const { return platform;}
    void initPlatform() { platform->init();}

    // float getDtFactor() const;
    void setDtFactor(float dtFactor);

    // AnimationType getAnimationTypeActive() const;
    void setAnimationTypeActive(AnimationType animationTypeActive);
    void setAnimationDuration(float duration);

    // bool isAnimationTypeEditMode() const;
    // void setAnimationTypeEditMode(bool animationTypeEditMode);
    bool isAnimationPaused() { return !isAnimationRunning; }
    void setAnimationRunning(bool isRunning) { isAnimationRunning = isRunning; }
    void toggleAnimationRunning() { isAnimationRunning = !isAnimationRunning; }
private:
    StewartPlatform* platform; // Pointer to the associated StewartPlatform
    bool isAnimationRunning = true;
    Vector3 translation;
    Quaternion orientation;

    float pct;
    float dtFactor;
    float duration;
    AnimationType animationTypeActive;
    bool animationShouldCycle;
    bool animationTypeEditMode;

    std::vector<Vector3> pathBuffer;
    void addPathPoint(const Vector3& position){ pathBuffer.push_back(position);}
    void clearPath(){ pathBuffer.clear();}


    void applyTransformationToPlatform(Vector3 T, Quaternion O);
    void incrAnimationTime();

};
