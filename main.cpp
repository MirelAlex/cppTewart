/*******************************************************************************************
*
*   raylib-extras [ImGui] example - Simple Integration
*
*	This is a simple ImGui Integration
*	It is done using C++ but with C style code
*	It can be done in C as well if you use the C ImGui wrapper
*	https://github.com/cimgui/cimgui
*
*   Copyright (c) 2021 Jeffery Myers
*
********************************************************************************************/
#include "main.h"

#define SCREEN_WIDTH    (1920)
#define SCREEN_HEIGHT   ( 800)

void HandleCamera(Camera* camera);

CircularStewartPlatform circularPlatform;
Animation platformAnimation(&circularPlatform);
Camera camera = { 0 };
bool locked = false;

int main(int argc, char* argv[])
{
    // Initialization
	//--------------------------------------------------------------------------------------

    // Define the camera to look into our 3d world
    camera.position = (Vector3){ 30.0f, 30.0f, 30.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    // FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT |
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "c++Tewart Platform Simulation");
	SetTargetFPS(144);
	rlImGuiSetup(true);
    ImPlot::CreateContext();

    circularPlatform.init();
    // Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
        HandleCamera(&camera);



        // animation runs inside the method and since it is bound to a StewartPlatform object it will call its update method
        platformAnimation.runAnimation();

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                circularPlatform.draw();
                platformAnimation.drawPath();
                DrawGrid(70, 5.0f);
            EndMode3D();

            DrawText("Stewart Platform Simulation", 10, 10, 20, BLACK);

            // start ImGui Content
            rlImGuiBegin();
                // show ImGui Content
                bool open = true;
                ImGui::ShowDemoWindow(&open);
                ImPlot::ShowDemoWindow(&open);
                StewartPlots::ShowStewartPlots(&open);
                ShowStewartGui(&open);
                // end ImGui Content
            rlImGuiEnd();
		EndDrawing();
		//----------------------------------------------------------------------------------
	}
    ImPlot::DestroyContext();
	rlImGuiShutdown();

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}

void HandleCamera(Camera* camera){
    if (IsKeyPressed('L')) locked = !locked;
    if (!locked){
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) UpdateCamera(camera, CAMERA_THIRD_PERSON);
    }else{
        DrawText("Camera Locked!", 10, 30, 15, RED);
    }
    if (IsKeyPressed('Z')) camera->target = circularPlatform.pos;
}


Animation* GetPlatformAnimation(){
    return &platformAnimation;
}

Camera* GetCamera(){
    return &camera;
}