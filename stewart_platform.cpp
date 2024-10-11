
#include "stewart_platform.h"
#include <iostream>
#include <fstream>
#include <filesystem>
/*==== Base Class Platform ====*/
void StewartPlatform::draw(){
        // Get the rotation matrix based on quaternion rotation
        Matrix rotationM = QuaternionToMatrix(orientation);
        // Generate translation matrix to platform position (consider T0 and platform position)
        Matrix translationM = MatrixTranslate(pos.x + translation.x, pos.y + translation.y + T0.y, pos.z + translation.z);
        // Multiply translation matrix with rotation matrix
        Matrix transformM = MatrixMultiply(rotationM, translationM);

        Matrix hexapodPos = MatrixTranslate(pos.x, pos.y, pos.z);
        rlPushMatrix();
             rlMultMatrixf(MatrixToFloat(hexapodPos));
            // Draw base frame first before any translation, rotation is applied
            drawFrame();
            // Draw base platform (circular / hexagonal / whatever)
            drawBasePlate(base.color);
        rlPopMatrix();
        for (size_t i = 0; i < 6; i++)
        {
            rlPushMatrix();
                // translate whole hexapod to position..doesnt work as expected or I am doing something wrong
                // rlMultMatrixf(MatrixToFloat(hexapodPos));

                rlPushMatrix();
                    rlMultMatrixf(MatrixToFloat(hexapodPos));
                    if (drawVectors)
                    {
                        // Draw q vectors
                        DrawCylinderEx(zero_vec, q[i], 0.01, 0.01, 20, DARKGRAY);
                        // Draw l vectors
                        DrawCylinderEx(B[i], q[i], 0.01, 0.01, 20, DARKGRAY);
                    }

                    // Draw horns
                    DrawCylinderEx(B[i], H[i] , 0.08, 0.08,20, PURPLE);
                    // Draw rods
                    DrawCylinderEx(H[i], q[i] , 0.08, 0.08,20, PURPLE);
                    DrawSphere(H[i], 0.2f, BLUE);
                    // Draw joints
                    DrawSphere(B[i], 0.2f, RED);
                rlPopMatrix();
                // This rlPushMatrix doesn't work as expected in terms of rotation TODO: check why
                rlPushMatrix();
                    rlMultMatrixf(MatrixToFloat(transformM));
                    DrawSphere(P[i], 0.2f, RED);
                    // Draw frame on platform also, refPos is 0,0,0 because we translated already
                    drawFrame();
                    // Draw plate
                    drawPlatformPlate(plat.color);
                rlPopMatrix();
            rlPopMatrix();
        }
    }

void StewartPlatform::update(const Vector3& tra, const Quaternion& ori){
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

        // calculate servo motor angle
        legs[i].angle = asinf((H[i].y - B[i].y) / hornLength) * RAD2DEG;
    }

}

void StewartPlatform::loadConfig(const std::string& configFilePath) {

    // Construct the full path to the config file using the preprocessor definition
    std::filesystem::path configPath = std::filesystem::path(PROJECT_BASE_DIR) / "include" / "config" / configFilePath;

    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Could not open config file at path: " + configPath.string());
    }

    json config;
    configFile >> config;

    pos = {config["pos"][0], config["pos"][1], config["pos"][2]};
    translation = {config["translation"][0], config["translation"][1], config["translation"][2]};
    orientation = {config["orientation"][0], config["orientation"][1], config["orientation"][2], config["orientation"][3]};
    hornLength = config["hornLength"];
    rodLength = config["rodLength"];
    base = {
        config["base"]["thickness"],
        config["base"]["radius"],
        config["base"]["shaftDistance"],
        {config["base"]["color"][0], config["base"]["color"][1], config["base"]["color"][2], config["base"]["color"][3]}
        };
    plat = {
         config["plat"]["thickness"],
         config["plat"]["radius"],
         config["plat"]["anchorDistance"],
        {config["plat"]["color"][0], config["plat"]["color"][1], config["plat"]["color"][2], config["plat"]["color"][3]}
        };
    drawVectors = config["drawVectors"];

    if (config.contains("platformOuterRadius") && config.contains("baseOuterRadius")){
        platformOuterRadius = config["platformOuterRadius"];
        baseOuterRadius = config["baseOuterRadius"];
    }else{
        platformOuterRadius = plat.radius + 2;
        baseOuterRadius = base.radius + 3;
    }

}

void StewartPlatform::saveConfig(const std::string& configFilePath) {
    // Construct the full path to the config file using the preprocessor definition
    std::filesystem::path configPath = std::filesystem::path(PROJECT_BASE_DIR) / "include" / "config" / configFilePath;

    // Create a JSON object and populate it with the current state
    nlohmann::json config;
    config["pos"] = {pos.x, pos.y, pos.z};
    config["translation"] = {translation.x, translation.y, translation.z};
    config["orientation"] = {orientation.x, orientation.y, orientation.z, orientation.w};
    config["hornLength"] = hornLength;
    config["rodLength"] = rodLength;
    config["base"]["thickness"] = base.thickness;
    config["base"]["radius"] = base.radius;
    config["base"]["shaftDistance"] = base.shaftDistance;
    config["base"]["color"] = {base.color.r, base.color.g, base.color.b, base.color.a};
    config["plat"]["thickness"] = plat.thickness;
    config["plat"]["radius"] = plat.radius;
    config["plat"]["anchorDistance"] = plat.anchorDistance;
    config["plat"]["color"] = {plat.color.r, plat.color.g, plat.color.b, plat.color.a};
    config["platformOuterRadius"] = platformOuterRadius;
    config["baseOuterRadius"] = baseOuterRadius;
    config["drawVectors"] = drawVectors;


    // Open the file for writing
    std::ofstream configFile(configPath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Could not open config file for writing at path: " + configPath.string());
    }

    // Write the JSON object to the file
    configFile << config.dump(4); // Pretty print with an indent of 4 spaces
    configFile.close();
}

/*==== Circular Platform ====*/
void CircularStewartPlatform::init() {
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
}

void CircularStewartPlatform::getLegs() {
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

        // TraceLog(LOG_INFO, "😁 baseJoint = %.3f %.3f %.3f platformJoint = %.3f %.3f %.3f motorRotation = %.3f",
        //                     legs[i].baseJoint.x, legs[i].baseJoint.y, legs[i].baseJoint.z,
        //                     legs[i].platformJoint.x, legs[i].platformJoint.y, legs[i].platformJoint.z,
        //                     legs[i].motorRot);
    }
}

void CircularStewartPlatform::drawBasePlate(Color c) {
    // Drawing code for circular base plate
    Vector3 endposB = {0.0f, base.thickness, 0.0f};
    DrawCylinderEx(zero_vec, endposB, base.radius, base.radius, 30, c);
    DrawCylinderWiresEx(zero_vec, endposB, base.radius, base.radius, 30, c);
}

void CircularStewartPlatform::drawPlatformPlate(Color c) {
    // Drawing code for circular platform plate
    Vector3 endposP = {0.0f, plat.thickness, 0.0f};
    DrawCylinderEx(zero_vec, endposP, plat.radius, plat.radius, 30, c);
    DrawCylinderWiresEx(zero_vec, endposP, plat.radius, plat.radius, 30, c);
}


/*==== Hexagonal Platform ====*/
void HexagonalStewartPlatform::init() {
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
}


void HexagonalStewartPlatform::getLegs() {
    getHexPlateInts(base.radius, baseOuterRadius, 0.0, baseInts);
    getHexPlateInts(plat.radius, platformOuterRadius, 0.0, platformInts);

    Vector3 basePoints[6];
    Vector3 platPoints[6];
    float motorAngle[6];
    uint8 hornDirection = 0u;
    for (size_t i = 0; i < 6; i++)
    {
        int midK = i | 1;
        float baseCx = baseInts[midK].x;
        float baseCy = baseInts[midK].y;
        float baseNx = baseInts[(midK + 1) % 6].x;
        float baseNY = baseInts[(midK + 1) % 6].y;

        float platCx = platformInts[midK].x;
        float platCy = platformInts[midK].y;
        float platNx = platformInts[(midK + 1) % 6].x;
        float platNY = platformInts[(midK + 1) % 6].y;

        float baseDX = baseNx - baseCx;
        float baseDY = baseNY - baseCy;
        float lenBaseSide = hypot(baseDX, baseDY);

        int pm = pow(-1, i);

        float baseMidX = (baseCx + baseNx) / 2;
        float baseMidY = (baseCy + baseNY) / 2;

        float platMidX = (platCx + platNx) / 2;
        float platMidY = (platCy + platNY) / 2;

        baseDX /= lenBaseSide;
        baseDY /= lenBaseSide;

        // calculate 3d points based on the 2d points calculated above. Note that the y value is 0 and in 3d space y axis is actually z axis
        basePoints[i].x = baseMidX + baseDX * base.shaftDistance * pm;
        basePoints[i].z = baseMidY + baseDY * base.shaftDistance * pm;
        basePoints[i].y = 0;

        platPoints[i].x = platMidX + baseDX * plat.anchorDistance * pm;
        platPoints[i].z = platMidY + baseDY * plat.anchorDistance * pm;
        platPoints[i].y = 0;

        motorAngle[i] = atan2(baseDY, baseDX) + ((i + hornDirection) % 2) * PI;

    }

    // TODO: add platformTurn == 0 uint8 platformIndex[6] = {4, 3, 0, 5, 2, 1}
    uint8 platformIndex[6] = {0, 1, 2, 3, 4, 5};
    for (size_t i = 0; i < 6; i++)
    {
        legs[i].baseJoint = basePoints[i];
        legs[i].platformJoint = platPoints[platformIndex[i]];
        legs[i].motorRot = motorAngle[i];
        // TraceLog(LOG_WARNING, "=====> baseJoint = %.3f %.3f %.3f platformJoint = %.3f %.3f %.3f motorRotation = %.3f",
        //             legs[i].baseJoint.x, legs[i].baseJoint.y, legs[i].baseJoint.z,
        //             legs[i].platformJoint.x, legs[i].platformJoint.y, legs[i].platformJoint.z,
        //             legs[i].motorRot);
    }


}
// Function to draw a custom hexagon in 3D
void DrawHexagon3D(Vector3 vertices[6], Color color, float thickness) {
    rlPushMatrix();  // Save the current transform matrix
    // Hexagon height
    float hexagonHeight = thickness;
    float h = hexagonHeight / 2.0f;
    // Top face hexagon vertices (centered on XY plane at Y = hexagonHeight / 2)
    Vector3 topHexagonVertices[6] = {
        {vertices[0].x, h, vertices[0].z},
        {vertices[1].x, h, vertices[1].z},
        {vertices[2].x, h, vertices[2].z},
        {vertices[3].x, h, vertices[3].z},
        {vertices[4].x, h, vertices[4].z},
        {vertices[5].x, h, vertices[5].z}
    };

    // Bottom face hexagon vertices (centered on XY plane at Y = -hexagonHeight / 2)
    Vector3 bottomHexagonVertices[6] = {
        {vertices[0].x, -h, vertices[0].z},
        {vertices[1].x, -h, vertices[1].z},
        {vertices[2].x, -h, vertices[2].z},
        {vertices[3].x, -h, vertices[3].z},
        {vertices[4].x, -h, vertices[4].z},
        {vertices[5].x, -h, vertices[5].z}
    };

    // Draw top face of the hexagon
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing upwards
        for (int i = 0; i < 6; i++) {
        rlVertex3f(0.0f, hexagonHeight / 2.0f, 0.0f);  // Center of top face
        rlVertex3f(topHexagonVertices[i].x, topHexagonVertices[i].y, topHexagonVertices[i].z);  // Current vertex
        rlVertex3f(topHexagonVertices[(i + 1) % 6].x, topHexagonVertices[(i + 1) % 6].y, topHexagonVertices[(i + 1) % 6].z);  // Next vertex
    }
    rlEnd();

    // Draw bottom face of the hexagon
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlNormal3f(0.0f, -1.0f, 0.0f); // Normal pointing downwards
    for (int i = 0; i < 6; i++) {
        rlVertex3f(0.0f, -hexagonHeight / 2.0f, 0.0f);  // Center of bottom face
        rlVertex3f(bottomHexagonVertices[i].x, bottomHexagonVertices[i].y, bottomHexagonVertices[i].z);  // Current vertex
        rlVertex3f(bottomHexagonVertices[(i + 1) % 6].x, bottomHexagonVertices[(i + 1) % 6].y, bottomHexagonVertices[(i + 1) % 6].z);  // Next vertex
    }
    rlEnd();

    // Draw vertical faces connecting top and bottom hexagon faces
    rlBegin(RL_QUADS);
    rlColor4ub(color.r, color.g, color.b, color.a);
    for (int i = 0; i < 6; i++) {
        int next = (i + 1) % 6;
        // Set normal for each vertical face
        Vector3 v1 = topHexagonVertices[i];
        Vector3 v2 = topHexagonVertices[next];
        Vector3 v3 = bottomHexagonVertices[next];
        Vector3 normal = Vector3CrossProduct(Vector3Subtract(v2, v1), Vector3Subtract(v3, v1));
        normal = Vector3Normalize(normal);

        rlNormal3f(normal.x, normal.y, normal.z);  // Normal for vertical face

        // First quad vertex: current top vertex
        rlVertex3f(topHexagonVertices[i].x, topHexagonVertices[i].y, topHexagonVertices[i].z);
        // Second quad vertex: next top vertex
        rlVertex3f(topHexagonVertices[next].x, topHexagonVertices[next].y, topHexagonVertices[next].z);
        // Third quad vertex: next bottom vertex
        rlVertex3f(bottomHexagonVertices[next].x, bottomHexagonVertices[next].y, bottomHexagonVertices[next].z);
        // Fourth quad vertex: current bottom vertex
        rlVertex3f(bottomHexagonVertices[i].x, bottomHexagonVertices[i].y, bottomHexagonVertices[i].z);
    }
    rlEnd();

    rlPopMatrix();  // Restore the matrix
}
void HexagonalStewartPlatform::drawBasePlate(Color c) {
    Color tint = c;
    Vector3 v[6]= {
        {baseInts[0].x, 0, baseInts[0].y},
        {baseInts[1].x, 0, baseInts[1].y},
        {baseInts[2].x, 0, baseInts[2].y},
        {baseInts[3].x, 0, baseInts[3].y},
        {baseInts[4].x, 0, baseInts[4].y},
        {baseInts[5].x, 0, baseInts[5].y}
    };

    DrawHexagon3D(v, tint, base.thickness);
}
void HexagonalStewartPlatform::drawPlatformPlate(Color c) {
    Color tint = c;
    Vector3 v[6]= {
        {platformInts[0].x, 0, platformInts[0].y},
        {platformInts[1].x, 0, platformInts[1].y},
        {platformInts[2].x, 0, platformInts[2].y},
        {platformInts[3].x, 0, platformInts[3].y},
        {platformInts[4].x, 0, platformInts[4].y},
        {platformInts[5].x, 0, platformInts[5].y}
    };

    DrawHexagon3D(v, tint, plat.thickness);
}


void getCurrentServoAngles(float* angles, int count){
    for (size_t i = 0; i < count; i++)
    {
        // TODO: maybe move angle somewhere else?
        // angles[i] = THIS.legs[i].angle;
        // TODO: limit the servo angle to a servo range
        // printf("[%d] %.2f\n",i, RAD2DEG * THIS.legs[i].angle );
    }
}
