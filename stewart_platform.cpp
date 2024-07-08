
// int main() {
//     CircularStewartPlatform circularPlatform;
//     circularPlatform.init();
//     circularPlatform.update();

//     HexagonalStewartPlatform hexagonalPlatform;
//     hexagonalPlatform.init();
//     hexagonalPlatform.update();

//     return 0;
// }
#include "stewart_platform.h"
#include <cmath>
#include "raymath.h"
#include "rlgl.h"

cTewart stewartPlatform;
Animation1 animation;

// TODO: to be added in a structure
float amplitude = 0.98f;
bool drawVectors = 0;


void getLegs(){
    uint8 hornDirection = 0u;
    for (size_t i = 0; i < 6; i++)
    {
        sint8 pm = pow(-1, i);
        float phiCut = (1 + i - i % 2) * PI / 3;
        float phiB = (i + i % 2) * PI / 3 + pm * BASE.shaftDistance / 2;
        float phiP = phiCut - pm * PLAT.anchorDistance / 2;

        THIS.legs[i].baseJoint = (Vector3){cos(phiB) * BASE.radius, 0, sin(phiB) * BASE.radius};
        THIS.legs[i].platformJoint = (Vector3){cos(phiP) * PLAT.radius, 0, sin(phiP) * PLAT.radius};
        THIS.legs[i].motorRot = phiB + ((i + hornDirection) % 2) * PI + PI / 2;

        TraceLog(LOG_INFO, "baseJoint = %.3f %.3f %.3f platformJoint = %.3f %.3f %.3f motorRotation = %.3f",
                            THIS.legs[i].baseJoint.x, THIS.legs[i].baseJoint.y, THIS.legs[i].baseJoint.z,
                            THIS.legs[i].platformJoint.x, THIS.legs[i].platformJoint.y, THIS.legs[i].platformJoint.z,
                            THIS.legs[i].motorRot);
    }

}

void calcT0(){
    THIS.T0 = (Vector3){
        0.0f,
        sqrtf(THIS.rodLength * THIS.rodLength + THIS.hornLength * THIS.hornLength
        - powf(THIS.P[0].x - THIS.B[0].x, 2)
        - powf(THIS.P[0].z - THIS.B[0].z, 2)),
        0.0f
    };
}

void _initParameters(cTewart *sp){

    // Initialization
   *sp = (cTewart){
        .pos = zero_vec,
        .translation = zero_vec,
        .orientation = zero_qua,
        .legsLength = 3.0f,
        .hornLength = 5.0f,
        .rodLength = 13.0f,
        .legs={ {zero_vec, zero_vec, 0.0f, 0.0f},
                {zero_vec, zero_vec, 0.0f, 0.0f},
                {zero_vec, zero_vec, 0.0f, 0.0f},
                {zero_vec, zero_vec, 0.0f, 0.0f},
                {zero_vec, zero_vec, 0.0f, 0.0f},
                {zero_vec, zero_vec, 0.0f, 0.0f},
        },
        .B={zero_vec},
        .P={zero_vec},
        .q={zero_vec},
        .l={zero_vec},
        .H={zero_vec},
        .T0=zero_vec,
        .sinBeta={0.0f},
        .cosBeta={0.0f},
        .base = {
                    .pos = {0.0f, 0.0f, 0.0f},
                    .radius = 8.0f,
                    .shaftDistance = 2.0f / 8.0f
                },
        .plat = {
                    .pos = {0.0f, 0.0f, 0.0f},
                    .radius = 5.0f,
                    .anchorDistance = 2.0f / 8.0f,
                    // .orientation_matrix_f = (float*)0
                },
    };
}

void InitAnimation(Animation1 *a){
    *a = (Animation1){
        .pct = 0.0f,
        .dtFactor = 0.5f, // animation speed factor
        .animationTypeActive = ROTATE,
        .animationTypeEditMode = false
    };
}

void InitStewart(cTewart *sp){

    _initParameters(sp);
    getLegs();
    for (size_t i = 0; i < 6; i++)
    {
        THIS.B[i] = THIS.legs[i].baseJoint;
        THIS.P[i] = THIS.legs[i].platformJoint;
        THIS.sinBeta[i] = sin(THIS.legs[i].motorRot);
        THIS.cosBeta[i] = cos(THIS.legs[i].motorRot);
        THIS.q[i] = zero_vec;
        THIS.l[i] = zero_vec;
        THIS.H[i] = zero_vec;
    }

    calcT0();
    // TraceLog(LOG_INFO, "THIS.T0 = %.2f %.2f %.2f", THIS.T0.x, THIS.T0.y, THIS.T0.z );
    // TODO: remove this and try to use matrices to translate and rotate
    // or maybe not, I can still keep this ..idk
    // PLAT.pos = THIS.T0;
}

void UpdateStewart(Vector3 translation, Quaternion orientation){
    THIS.translation = translation;
    THIS.orientation = orientation;
    Vector3 o;
    for (size_t i = 0; i < 6; i++)
    {
        o = Vector3RotateByQuaternion(THIS.P[i], orientation);

        THIS.q[i].x = translation.x + o.x;
        THIS.q[i].y = translation.y + o.y + THIS.T0.y;
        THIS.q[i].z = translation.z + o.z ;
        // TraceLog(LOG_INFO, " THIS.q[i] = %.2f %.2f %.2f",  THIS.q[i].x,  THIS.q[i].y,  THIS.q[i].z );
        THIS.l[i].x = THIS.q[i].x - THIS.B[i].x;
        THIS.l[i].y = THIS.q[i].y - THIS.B[i].y;
        THIS.l[i].z = THIS.q[i].z - THIS.B[i].z;

        float gk = (THIS.l[i].x * THIS.l[i].x) + (THIS.l[i].y * THIS.l[i].y) + (THIS.l[i].z * THIS.l[i].z) - ((THIS.rodLength * THIS.rodLength) + (THIS.hornLength * THIS.hornLength));
        float ek = 2 * THIS.hornLength * THIS.l[i].y ;
        float fk = 2 * THIS.hornLength * (THIS.cosBeta[i] * THIS.l[i].x + THIS.sinBeta[i] * THIS.l[i].z);

        float sqSum = ek * ek + fk * fk;
        float sq1 = sqrtf(1 - gk * gk / sqSum);
        float sq2 = sqrtf(sqSum);
        float sinAlpha = (gk * ek) / sqSum - (fk * sq1) / sq2;
        float cosAlpha = (gk * fk) / sqSum + (ek * sq1) / sq2;

        THIS.H[i].x = THIS.B[i].x + THIS.hornLength * cosAlpha * THIS.cosBeta[i];
        THIS.H[i].y = THIS.B[i].y + THIS.hornLength * sinAlpha;
        THIS.H[i].z = THIS.B[i].z + THIS.hornLength * cosAlpha * THIS.sinBeta[i];
    }

}

void _drawFrame(Vector3 refPos){
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

void DrawStewart(){
    Color platColor = Fade(SKYBLUE, 0.7f); // Set the alpha channel to 0.5 (50% transparency)
    Color baseColor = Fade(YELLOW, 0.7f); // Set the alpha channel to 0.5 (50% transparency)
    float thick = 0.003f;
    Vector3 endposB = {BASE.pos.x, BASE.pos.y+thick, BASE.pos.z};
    Vector3 endposP = {PLAT.pos.x, PLAT.pos.y+thick, PLAT.pos.z};
    // Get the rotation matrix based on quaternion rotation
    Matrix rotationM = QuaternionToMatrix(THIS.orientation);
    // Generate translation matrix to platform position
    Matrix translationM = MatrixTranslate(THIS.translation.x, THIS.translation.y + THIS.T0.y, THIS.translation.z);
    // Multiply translation matrix with rotation matrix
    Matrix combinedM = MatrixMultiply(rotationM, translationM);
    for (size_t i = 0; i < 6; i++)
    {
        rlPushMatrix();
            if (drawVectors)
            {
                // Draw q vectors
                DrawCylinderEx(zero_vec, THIS.q[i], 0.01, 0.01, 20, DARKGRAY);
                // Draw l vectors
                DrawCylinderEx(THIS.B[i], THIS.q[i], 0.01, 0.01, 20, DARKGRAY);
            }

            // Draw horns
            DrawCylinderEx(THIS.B[i], THIS.H[i] , 0.08, 0.08,20, PURPLE);
            // Draw rods
            DrawCylinderEx(THIS.H[i], THIS.q[i] , 0.08, 0.08,20, PURPLE);
            // DrawSphere(THIS.q[i], 0.2f, BLUE);
            DrawSphere(THIS.H[i], 0.2f, BLUE);
            // Draw joints
            DrawSphere(THIS.B[i], 0.2f, RED);

            // This rlPushMatrix doesn't work as expected in terms of rotation TODO: check why
            rlPushMatrix();
                rlMultMatrixf(MatrixToFloat(combinedM));
                DrawSphere(THIS.P[i], 0.2f, RED);
                // Draw frame on platform also
                _drawFrame(zero_vec);
                // Draw plate
                DrawCylinderEx(zero_vec, endposP, PLAT.radius, PLAT.radius, 30, platColor);
                DrawCylinderWiresEx(zero_vec, endposP, PLAT.radius, PLAT.radius, 30, platColor);
            rlPopMatrix();
        rlPopMatrix();
    }

    DrawCylinderEx(BASE.pos, endposB, BASE.radius, BASE.radius, 30, baseColor);
    DrawCylinderWiresEx(BASE.pos, endposB, BASE.radius, BASE.radius, 30, baseColor);

}

void RunAnimation(){
    float b = 0;
    switch (ANIM.animationTypeActive)
    {
        // case WOBBLE:
        //     // wobble effect
        //     b = ANIM.pct * 2.0f * PI;
        //     THIS.translation.x = cos(-b) * 2.0f * amplitude;
        //     THIS.translation.y = 0.0f;
        //     THIS.translation.z = sin(-b) * 2.0f * amplitude;
        //     Quaternion o = (Quaternion){-cos(b), 0 , sin(b), amplitude * 10};
        //     THIS.orientation = QuaternionNormalize(o);
        //     break;
        case ROTATE:
            // rotate
            b = powf(sin(ANIM.pct * PI * 2 - PI * 8), 5) / amplitude;
            THIS.translation.x = 0.0f;
            THIS.translation.y = 0.0f;
            THIS.translation.z = 0.0f;
            THIS.orientation = QuaternionFromAxisAngle((Vector3){0,1,0}, b);
            break;
        // case TILT:
        //     // tilt on 2 axis
        //     Vector3 v;
        //     b = powf(sin(ANIM.pct * PI * 2 - PI * 8), 5) / (amplitude+1);
        //     THIS.translation.x = 0.0f;
        //     THIS.translation.y = 0.0f;
        //     THIS.translation.z = 0.0f;
        //     // printf("1/2: %f\n",(float)(1.0/2.0));
        //     // printf("under: %f\n",ANIM.pct);
        //     // printf("over: %f\n",ANIM.pct);
        //     if (ANIM.pct < (float)(1.0/2.0))
        //     {
        //         v = (Vector3){1,0,0};
        //     }else{
        //         v = (Vector3){0,0,1};
        //     }
        //     THIS.orientation = QuaternionFromAxisAngle(v, b);
        //     break;
        default:
            break;
    }
}

// void DrawSliders()
// {
//     float _rodL = THIS.rodLength;
//     float _hornL = THIS.hornLength;
//     float _sD = BASE.shaftDistance;
//     float _aD = PLAT.anchorDistance;
//     // Check all possible UI states that require controls lock
//     if (ANIM.animationTypeEditMode) GuiLock();

//     THIS.hornLength = GuiSliderBar((Rectangle){ leftBoundary, 40, 250, 20 },
//                                     "horn length", TextFormat("%.2f", THIS.hornLength),
//                                     THIS.hornLength,
//                                     0, 20);
//     THIS.rodLength = GuiSliderBar((Rectangle){ leftBoundary, 70, 250, 20 },
//                                     "rod length", TextFormat("%.2f", THIS.rodLength),
//                                     THIS.rodLength,
//                                     0, 20);
//     // TODO: rewrite this / move it
//     if ((THIS.rodLength != _rodL) || (THIS.hornLength != _hornL))
//     {
//         calcT0();
//     }

//     BASE.shaftDistance = GuiSliderBar((Rectangle){ leftBoundary, 100, 250, 20 },
//                                     "shaftDistance", TextFormat("%.2f", BASE.shaftDistance),
//                                     BASE.shaftDistance,
//                                     0, 1); // 1 is perfectly symetrical
//     PLAT.anchorDistance = GuiSliderBar((Rectangle){ leftBoundary, 130, 250, 20 },
//                                     "anchorDsitance", TextFormat("%.2f", PLAT.anchorDistance),
//                                     PLAT.anchorDistance,
//                                     0, 1); // 1 is perfectly symetrical
//     // TODO: rewrite this / move
//     if ((BASE.shaftDistance!= _sD) || (PLAT.anchorDistance != _aD))
//     {
//         getLegs();
//         for (size_t i = 0; i < 6; i++)
//         {
//             THIS.B[i] = THIS.legs[i].baseJoint;
//             THIS.P[i] = THIS.legs[i].platformJoint;
//             THIS.sinBeta[i] = sin(THIS.legs[i].motorRot);
//             THIS.cosBeta[i] = cos(THIS.legs[i].motorRot);
//             THIS.q[i] = zero_vec;
//             THIS.l[i] = zero_vec;
//             THIS.H[i] = zero_vec;
//         }
//     }
//     amplitude = GuiSliderBar((Rectangle){ leftBoundary, 160, 250, 20 },
//                                     "amplitude", TextFormat("%.2f", amplitude),
//                                     amplitude,
//                                     -1, 1);
//     ANIM.dtFactor = GuiSliderBar((Rectangle){ leftBoundary, 190, 250, 20 },
//                                     "ANIM.dtFactor", TextFormat("%.2f", ANIM.dtFactor),
//                                     ANIM.dtFactor,
//                                     0, 1);
//     drawVectors = GuiCheckBox((Rectangle){ leftBoundary, 220, 20, 20 }, "show q/l vectors?", drawVectors);

//     GuiUnlock();

//     GuiLabel((Rectangle){ leftBoundary, 250, 20, 20 }, "Animation type:");
//     if (GuiDropdownBox((Rectangle){ leftBoundary, 20 + 250, 140, 28 }, "WOBBLE;ROTATE;TILT", (int*)&ANIM.animationTypeActive, ANIM.animationTypeEditMode)){
//         ANIM.animationTypeEditMode = !ANIM.animationTypeEditMode;
//     }

// }

void getServoAngles(){
    for (size_t i = 0; i < 6; i++)
    {
        // TODO: maybe move angle somewhere else?
        THIS.legs[i].angle = asinf((THIS.H[i].y - THIS.B[i].y) / THIS.hornLength) * RAD2DEG;
        // TODO: limit the servo angle to a servo range
        // printf("[%d] %.2f\n",i, RAD2DEG * THIS.legs[i].angle );
    }
}

void IncrAnimationTime(){
    // increment ANIM.pct
    ANIM.pct += (GetFrameTime() * ANIM.dtFactor);
    if (ANIM.pct > 1.0f) {ANIM.pct=0.0f;};
}