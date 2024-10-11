#include "animation.h"
#include <iostream>

Animation::Animation(StewartPlatform* platform):
    platform(platform),
    translation((Vector3){0.0f,0.0f,0.0f}),
    orientation((Quaternion){0.0f,0.0f,0.0f,1.0f}),
    pct(0.0f),
    dtFactor(1.0f),
    duration(5.0f),
    animationTypeActive(AnimationType::WOBBLE),
    animationTypeEditMode(false),
    animationShouldCycle(true)
    {}

void Animation::runAnimation() {
    if (!isAnimationRunning) return;
    float b = 0;
    float amplitude = 0.98;

    switch (animationTypeActive)
    {
        case AnimationType::WOBBLE:
            setAnimationDuration(15);
            // wobble effect
            b = pct * 2.0f * PI;
            translation.x = cos(-b) * 2.0f * amplitude;
            translation.y = 0.0f;
            translation.z = sin(-b) * 2.0f * amplitude;
            orientation = (Quaternion){-cos(b), 0 , sin(b), amplitude * 10};
            orientation = QuaternionNormalize(orientation);
            applyTransformationToPlatform(translation,orientation);
            break;
        case AnimationType::ROTATE:
            // rotate
            setAnimationDuration(10);
            b = powf(sin(pct * PI * 2 - PI * 8), 5) / amplitude;
            translation.x = 0.0f;
            translation.y = 0.0f;
            translation.z = 0.0f;
            orientation = QuaternionFromAxisAngle((Vector3){0,1,0}, b);
            applyTransformationToPlatform(translation,orientation);
            break;
        case AnimationType::TILT:
            // tilt on 2 axis
            setAnimationDuration(10);
            Vector3 v;
            b = powf(sin(pct * PI * 2 - PI * 8), 5) / (amplitude+1);
            translation.x = 0.0f;
            translation.y = 0.0f;
            translation.z = 0.0f;
            if (pct < (float)(1.0/2.0))
            {
                v = (Vector3){1,0,0};
            }else{
                v = (Vector3){0,0,1};
            }
            orientation = QuaternionFromAxisAngle(v, b);
            applyTransformationToPlatform(translation,orientation);
            break;
        case AnimationType::LISSAJOUS:
            // lissajous figure
            setAnimationDuration(10);
            translation.x = sin(3 * pct * 2 * PI) * 3.0f;
            translation.y = 0;
            translation.z = sin(pct * 2 * 2 * PI) * 3.0f;
            orientation = Quaternion{0.0f, 0.0f, 0.0f, 1.0f};
            applyTransformationToPlatform(translation,orientation);
            break;
        default:
            break;
    }

    incrAnimationTime();

    // draw path if enabled
    addPathPoint(translation);
}

void Animation::incrAnimationTime() {
    uint8 currAnim, nextAnim;
    pct += (GetFrameTime() * dtFactor) / duration;
    if (pct > 1.0f) {
        pct = 0.0f;
        if (animationShouldCycle)
        {
            currAnim = static_cast<uint8>(animationTypeActive);
            nextAnim = (currAnim + 1) % static_cast<uint8>(AnimationType::length);
            setAnimationTypeActive(static_cast<AnimationType>(nextAnim));
        }

    }
}

void Animation::drawPath() {
    if (!bDrawPath) return;
    // Draw path lines
    rlPushMatrix();
        rlTranslatef(platform->pos.x + platform->T0.x, platform->pos.y + platform->T0.y + 0.2, platform->pos.z + platform->T0.z);
        for (size_t i = 0; i < pathBuffer.size()-1; i++) {
            DrawLine3D(pathBuffer[i], pathBuffer[i+1], RED);
        }
    // Draw sphere at the last vertex
    DrawSphere(pathBuffer[0], 0.1f, GREEN);
    rlPopMatrix();

    // Limit path buffer size
    if (pathBuffer.size() > 1000)
    {
        // remove first element
        pathBuffer.erase(pathBuffer.begin());
    }

}


void Animation::applyTransformationToPlatform(Vector3 T, Quaternion O) {
    this->platform->update(T, O);
}

// Getters and Setters for Animation class
// float Animation::getPct() const {
//     return pct;
// }

// void Animation::setPct(float pct) {
//     this->pct = pct;
// }

// float Animation::getDtFactor() const {
//     return dtFactor;
// }

void Animation::setDtFactor(float dtFactor) {
    this->dtFactor = dtFactor;
}

// AnimationType Animation::getAnimationTypeActive() const {
//     return animationTypeActive;
// }

void Animation::setAnimationTypeActive(AnimationType animationTypeActive) {
    this->animationTypeActive = animationTypeActive;
    clearPath();
}

void Animation::setAnimationDuration(float duration){
    this->duration = duration;
}

// bool Animation::isAnimationTypeEditMode() const {
//     return animationTypeEditMode;
// }

// void Animation::setAnimationTypeEditMode(bool animationTypeEditMode) {
//     this->animationTypeEditMode = animationTypeEditMode;
// }
