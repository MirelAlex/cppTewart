#include "animation.h"
#include <iostream>
std::vector<Vector3> buffer;
Animation::Animation(StewartPlatform* platform):
    platform(platform),
    translation((Vector3){0.0f,0.0f,0.0f}),
    orientation((Quaternion){0.0f,0.0f,0.0f,1.0f}),
    pct(0.0f),
    pathBuffer(300),
    dtFactor(1.0f),
    duration(5.0f),
    animationTypeActive(AnimationType::LISSAJOUS),
    animationTypeEditMode(false),
    animationShouldCycle(true)
    {}

void Animation::runAnimation() {
    float b = 0;
    float amplitude = 0.98;

    switch (animationTypeActive)
    {
        case AnimationType::WOBBLE:
            setAnimationDuration(2);
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
    // addPathPoint(translation);
    buffer.push_back(translation);

    incrAnimationTime();
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
    // if (pathBuffer.size() > 1) {
        // Calculate current position along the path based on pct
        // int currentPointIndex = static_cast<int>(pct * (pathPoints.size() - 1));
        // Vector3 currentPos = pathPoints[currentPointIndex].position;
        // Draw path lines
            rlPushMatrix();
                rlTranslatef(platform->T0.x, platform->T0.y + 0.05, platform->T0.z);
                for (size_t i = 0; i < buffer.size()-1; i++) {
                    DrawLine3D(buffer[i], buffer[i+1], RED);
                }
            rlPopMatrix();
        if (buffer.size() > 1000)
        {
            buffer.erase(buffer.begin());
        }

        // Draw sphere at the last vertex
        DrawSphere(Vector3Add(buffer[0],platform->T0), 0.1f, GREEN);
        // DrawSphere(Vector3Add(pathBuffer.get_last_point().position,platform->T0), 0.4f, BLUE);

        // Draw a sphere or marker at the current position
        // DrawSphere(currentPos, 0.2f, BLUE);
    // }
}

void Animation::addPathPoint(const Vector3& position) {
    pathBuffer.push_back(position);
}

void Animation::clearPath() {
    pathBuffer.empty();
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
