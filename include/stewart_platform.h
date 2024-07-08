#ifndef STEW
#define STEW
#include "raylib.h"
#include <cmath> // For sqrtf and powf functions
#include "rlgl.h"
#include "raymath.h"

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
    Vector3 pos;
    float radius;
    float shaftDistance;
};

struct Platform {
    Vector3 pos;
    float radius;
    float anchorDistance;
};

const Vector3 zero_vec = (Vector3){0.0f, 0.0f, 0.0f};
const Quaternion zero_qua = (Quaternion){0.0f, 0.0f, 0.0f, 1.0f};

class StewartPlatform {
public:
    Vector3 translation;
    Quaternion orientation;

    Vector3 pos;
    float legsLength;
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

    StewartPlatform()
        :   pos((Vector3){5,0,5}),
            translation(zero_vec),
            orientation(zero_qua),
            legsLength(3.0f),
            hornLength(5.0f),
            rodLength(13.0f),
            T0(zero_vec),
            sinBeta{0}, cosBeta{0},
            base({zero_vec, 8.0f, 2.0f / 8.0f}),
            plat({zero_vec, 5.0f, 2.0f / 8.0f})
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
        // base.pos = Vector3Add(base.pos, pos);
        // plat.pos = Vector3Add(plat.pos, pos);
    }
    virtual ~StewartPlatform() = default;

    void draw(){
        // code
        Color platColor = Fade(SKYBLUE, 0.7f); // Set the alpha channel to 0.5 (50% transparency)
        Color baseColor = Fade(YELLOW, 0.7f); // Set the alpha channel to 0.5 (50% transparency)
        float thick = 0.003f;
        Vector3 endposB = {base.pos.x, base.pos.y+thick, base.pos.z};
        Vector3 endposP = {plat.pos.x, plat.pos.y+thick, plat.pos.z};
        // Get the rotation matrix based on quaternion rotation
        Matrix rotationM = QuaternionToMatrix(orientation);
        // Generate translation matrix to platform position
        Matrix translationM = MatrixTranslate(translation.x, translation.y + T0.y, translation.z);
        // Multiply translation matrix with rotation matrix
        Matrix combinedM = MatrixMultiply(rotationM, translationM);
        // Draw base frame first before any translation, rotation is applied
        drawFrame(base.pos);
        // Draw base platform (circular / hexagonal / whatever)
        drawBasePlate(endposB, baseColor);
        for (size_t i = 0; i < 6; i++)
        {
            rlPushMatrix();
                // if (drawVectors)
                // {
                //     // Draw q vectors
                //     DrawCylinderEx(zero_vec, THIS.q[i], 0.01, 0.01, 20, DARKGRAY);
                //     // Draw l vectors
                //     DrawCylinderEx(THIS.B[i], THIS.q[i], 0.01, 0.01, 20, DARKGRAY);
                // }

                // Draw horns
                DrawCylinderEx(B[i], H[i] , 0.08, 0.08,20, PURPLE);
                // Draw rods
                DrawCylinderEx(H[i], q[i] , 0.08, 0.08,20, PURPLE);
                // DrawSphere(THIS.q[i], 0.2f, BLUE);
                DrawSphere(H[i], 0.2f, BLUE);
                // Draw joints
                DrawSphere(B[i], 0.2f, RED);

                // This rlPushMatrix doesn't work as expected in terms of rotation TODO: check why
                rlPushMatrix();
                    rlMultMatrixf(MatrixToFloat(combinedM));
                    DrawSphere(P[i], 0.2f, RED);
                    // Draw frame on platform also, refPos is 0,0,0 because we translated already
                    drawFrame(zero_vec);
                    // Draw plate
                    drawPlatformPlate(endposP, platColor);
                rlPopMatrix();
            rlPopMatrix();
        }


    }

    void update(const Vector3& tra, const Quaternion& ori){
        translation = tra;
        orientation = ori;
        Vector3 o;
        for (size_t i = 0; i < 6; i++)
        {
            o = Vector3RotateByQuaternion(P[i], orientation);

            q[i].x = translation.x + o.x;
            q[i].y = translation.y + o.y + T0.y;
            q[i].z = translation.z + o.z ;
            // TraceLog(LOG_INFO, " THIS.q[i] = %.2f %.2f %.2f",  THIS.q[i].x,  THIS.q[i].y,  THIS.q[i].z );
            l[i].x = q[i].x - B[i].x;
            l[i].y = q[i].y - B[i].y;
            l[i].z = q[i].z - B[i].z;

            float gk = (l[i].x * l[i].x) + (l[i].y * l[i].y) + (l[i].z * l[i].z) - ((rodLength * rodLength) + (hornLength * hornLength));
            float ek = 2 * hornLength * l[i].y ;
            float fk = 2 * hornLength * (cosBeta[i] * l[i].x + sinBeta[i] * l[i].z);

            float sqSum = ek * ek + fk * fk;
            float sq1 = sqrtf(1 - gk * gk / sqSum);
            float sq2 = sqrtf(sqSum);
            float sinAlpha = (gk * ek) / sqSum - (fk * sq1) / sq2;
            float cosAlpha = (gk * fk) / sqSum + (ek * sq1) / sq2;

            H[i].x = B[i].x + hornLength * cosAlpha * cosBeta[i];
            H[i].y = B[i].y + hornLength * sinAlpha;
            H[i].z = B[i].z + hornLength * cosAlpha * sinBeta[i];
        }

    }

protected:
    virtual void getLegs() = 0;
    virtual void drawBasePlate(Vector3 endB, Color c) = 0;
    virtual void drawPlatformPlate(Vector3 endP, Color c) = 0;
    virtual void init() = 0;

    void drawCone(float radius, float h){
        //
    }
    void drawFrame(Vector3 refPos){
        float radiusBottom = 0.2f;
        float radiusTop = 0.0f;
        float coneHeight = 1.0f;
        float frameHeight = 3.0f;
        int slices = 20;
        float frameThick = 0.1f;

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

    void init() override {
        // Initialization specific to circular platform
        getLegs();
        for (size_t i = 0; i < 6; i++)
        {
            B[i] = legs[i].baseJoint;
            P[i] = legs[i].platformJoint;
            sinBeta[i] = sinf(legs[i].motorRot);
            cosBeta[i] = cosf(legs[i].motorRot);
            q[i] = zero_vec;
            l[i] = zero_vec;
            H[i] = zero_vec;
        }

        calcT0();
        // TraceLog(LOG_INFO, "T0 = %.2f %.2f %.2f", T0.x, T0.y, T0.z);
    }

protected:
    typedef unsigned char uint8;
    typedef char sint8;
    void getLegs() override {
        uint8 hornDirection = 0u;
        for (size_t i = 0; i < 6; i++)
        {
            sint8 pm = pow(-1, i);
            float phiCut = (1 + i - i % 2) * PI / 3;
            float phiB = (i + i % 2) * PI / 3 + pm * base.shaftDistance / 2;
            float phiP = phiCut - pm * plat.anchorDistance / 2;

            legs[i].baseJoint = (Vector3){cosf(phiB) * base.radius, 0, sinf(phiB) * base.radius};
            legs[i].platformJoint = (Vector3){cosf(phiP) * plat.radius, 0, sinf(phiP) * plat.radius};
            legs[i].motorRot = phiB + ((i + hornDirection) % 2) * PI + PI / 2;

            TraceLog(LOG_INFO, "😁 baseJoint = %.3f %.3f %.3f platformJoint = %.3f %.3f %.3f motorRotation = %.3f",
                                legs[i].baseJoint.x, legs[i].baseJoint.y, legs[i].baseJoint.z,
                                legs[i].platformJoint.x, legs[i].platformJoint.y, legs[i].platformJoint.z,
                                legs[i].motorRot);
        }
    }

    void drawBasePlate(Vector3 endB, Color c) override {
        // Drawing code for circular base plate
        DrawCylinderEx(base.pos, endB, base.radius, base.radius, 30, c);
        DrawCylinderWiresEx(base.pos, endB, base.radius, base.radius, 30, c);
    }

    void drawPlatformPlate(Vector3 endP, Color c) override {
        // Drawing code for circular platform plate
        DrawCylinderEx(zero_vec, endP, plat.radius, plat.radius, 30, c);
        DrawCylinderWiresEx(zero_vec, endP, plat.radius, plat.radius, 30, c);
    }
};


typedef struct{
    Vector3 pos;
    float radius;
    float shaftDistance;
} Base1;

typedef struct {
    Vector3 pos;
    float radius;
    float anchorDistance;
} Platform1;

typedef struct
{
    Vector3 baseJoint;
    Vector3 platformJoint;
    float motorRot;
    float angle;
} Legs1;

typedef struct {
    Vector3 pos;
    Vector3 translation;
    Quaternion orientation;
    float legsLength;
    float hornLength;
    float rodLength;
    Legs1 legs[6];
    Vector3 B[6];
    Vector3 P[6];
    Vector3 q[6];
    Vector3 l[6];
    Vector3 H[6];
    Vector3 T0;
    float sinBeta[6];
    float cosBeta[6];
    Base1 base;
    Platform1 plat;
} cTewart;

// cTewart stewartPlatform;

typedef enum {
    WOBBLE,
    ROTATE,
    TILT
} AnimationType1;

typedef struct {
    float pct;
    float dtFactor;
    AnimationType1 animationTypeActive;
    bool animationTypeEditMode;
} Animation1;

// Animation animation;

extern cTewart stewartPlatform;
extern Animation1 animation;

#define THIS stewartPlatform
#define BASE (THIS.base)
#define PLAT (THIS.plat)
#define zero_vec (Vector3){0.0f, 0.0f, 0.0f}
#define zero_qua (Quaternion){0.0f, 0.0f, 0.0f, 0.0f}
#define ANIM animation


void InitStewart(cTewart *stewartPlatform);

void InitAnimation(Animation1 *a);

void RunAnimation();

void UpdateStewart(Vector3 translation, Quaternion orientation);

void _drawFrame(Vector3 refPos);

void DrawStewart();

void IncrAnimationTime();

#endif