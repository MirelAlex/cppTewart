#pragma once
#include "raylib.h"
#include <cmath> // For sqrtf and powf functions
#include "rlgl.h"
#include "raymath.h"
#include "json.hpp"
#include <string>
#include <fstream>
using json = nlohmann::json;

typedef unsigned char uint8;
typedef char sint8;
typedef unsigned short uint16;
typedef short sint16;
typedef unsigned int uint32;
typedef int sint32;
typedef bool boolean;

struct Legs {
    Vector3 baseJoint;
    Vector3 platformJoint;
    float motorRot;
    float angle;
};

struct Base {
    float thickness;
    float radius;
    float shaftDistance;
    Color color;
};

struct Platform {
    float thickness;
    float radius;
    float anchorDistance;
    Color color;
};

const Vector3 zero_vec = (Vector3){0.0f, 0.0f, 0.0f};
const Quaternion zero_qua = (Quaternion){0.0f, 0.0f, 0.0f, 1.0f};

class StewartPlatform {
public:
    Vector3 translation;
    Quaternion orientation;

    Vector3 pos;
    float hornLength;
    float rodLength;
    Vector3 B[6];
    Vector3 P[6];
    Vector3 q[6];
    Vector3 l[6];
    Vector3 H[6];
    Vector3 T0;
    float sinBeta[6];
    float cosBeta[6];
    Legs legs[6];
    Base base;
    Platform plat;

    // for hexagonal platform
    float platformOuterRadius = plat.radius + 2;
    float baseOuterRadius = base.radius + 3;

    boolean drawVectors;

    StewartPlatform()
        :   pos((Vector3){0,0,0}),
            translation(zero_vec),
            orientation(zero_qua),
            hornLength(5.0f),
            rodLength(13.0f),
            T0(zero_vec),
            sinBeta{0}, cosBeta{0},
            base({0.003, 8.0f, 3.0f, Fade(YELLOW, 0.7f)}),
            plat({0.003, 5.0f, 1.0f, Fade(SKYBLUE, 0.2f)}),
            drawVectors(true)

            // platformOuterRadius(plat.radius + 2),
            // baseOuterRadius(base.radius + 3)
            {
                for (size_t i = 0; i < 6; ++i) {
                    legs[i] = {zero_vec, zero_vec, 0.0f, 0.0f};
                    B[i] = zero_vec;
                    P[i] = zero_vec;
                    q[i] = zero_vec;
                    l[i] = zero_vec;
                    H[i] = zero_vec;
                }
                calcT0();
                loadConfig("stewart_config.json");
            }
    virtual ~StewartPlatform() = default;

    void draw();
    void update(const Vector3& tra, const Quaternion& ori);
    virtual void init() = 0;
    virtual void refreshPlatform() = 0;

    void saveConfig(const std::string& configFilePath);
private:
    void loadConfig(const std::string& configFilePath);
protected:
    virtual void getLegs() = 0;
    virtual void drawBasePlate(Color c) = 0;
    virtual void drawPlatformPlate(Color c) = 0;


    void drawFrame(){
        float radiusBottom = 0.2f;
        float radiusTop = 0.0f;
        float coneHeight = 1.0f;
        float frameHeight = 3.0f;
        int slices = 20;
        float frameThick = 0.1f;
        Vector3 refPos = {0.0f, 0.0f, 0.0f};

        Vector3 refConeStart = Vector3Add(refPos, (Vector3){frameHeight, 0.0f, 0.0f});
        // Draw cone and frame X
        Vector3 startPosition = refConeStart;
        Vector3 endPosition = startPosition;
        endPosition.x += coneHeight;
        DrawCylinderEx(refPos, startPosition, frameThick, frameThick, slices, RED);
        DrawCylinderEx(startPosition, endPosition, radiusBottom, radiusTop,slices, RED);

        // Draw cone and frame Y
        refConeStart = Vector3Add(refPos, (Vector3){0.0f, frameHeight, 0.0f});
        startPosition = refConeStart;
        endPosition = startPosition;
        endPosition.y += coneHeight;
        DrawCylinderEx(refPos, startPosition, frameThick, frameThick, slices, GREEN);
        DrawCylinderEx(startPosition, endPosition, radiusBottom, radiusTop,slices, GREEN);

        // Draw cone and frame Z
        refConeStart = Vector3Add(refPos, (Vector3){0.0f, 0.0f, frameHeight});
        startPosition = refConeStart;
        endPosition = startPosition;
        endPosition.z += coneHeight;
        DrawCylinderEx(refPos, startPosition, frameThick, frameThick, slices, BLUE);
        DrawCylinderEx(startPosition, endPosition, radiusBottom, radiusTop,slices, BLUE);
    }

    void calcT0() {
        T0 = (Vector3){
            0.0f,
            sqrtf(rodLength * rodLength + hornLength * hornLength
                - powf(P[0].x - B[0].x, 2)
                - powf(P[0].z - B[0].z, 2)),
            0.0f
        };
    }
};


// CircularStewartPlatform class
class CircularStewartPlatform : public StewartPlatform {
public:
    CircularStewartPlatform() : StewartPlatform() {}
    void init() override;
    void refreshPlatform() override {
        init();
    }
protected:
    void getLegs() override;
    void drawBasePlate(Color c) override;
    void drawPlatformPlate(Color c) override;
};

class HexagonalStewartPlatform : public StewartPlatform {
public:
    HexagonalStewartPlatform() : StewartPlatform() {}
    void init() override;
    void refreshPlatform() override {
        init();
    }
private:
    Vector2 baseInts[6];
    Vector2 platformInts[6];
    // float platformOuterRadius;
    // float baseOuterRadius;

    void getHexPlateInts(float r_i, float r_o, float rot, Vector2* ints){
        float a_2 = (2 * r_i - r_o) / sqrt(3);
        for (size_t i = 0; i < 6; i++)
        {
            float odd = i % 2;
            float phi = (i - odd) / 3 * PI + rot;
            float ap = a_2 * pow(-1, i);

            ints[i].x = (r_o * cos(phi)) + (ap * sin(phi));
            ints[i].y = (r_o * sin(phi)) - (ap * cos(phi));
        }
    }
protected:
    void getLegs() override;
    void drawBasePlate(Color c) override;
    void drawPlatformPlate(Color c) override;
};


void getCurrentServoAngles(float *angles, int count);
