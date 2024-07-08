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

void Demo_LinePlots();
void Demo_DigitalPlots();
void HandleCamera(Camera* camera);

CircularStewartPlatform circularPlatform;
Animation platformAnimation(&circularPlatform);
Camera camera = { 0 };
bool locked = false;
// utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};

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
	SetTargetFPS(60);
	rlImGuiSetup(true);
    ImPlot::CreateContext();

    // DisableCursor();                    // Limit cursor to relative movement inside the window
	// InitStewart(&stewartPlatform);
    // InitAnimation(&ANIM);
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

            IncrAnimationTime();

            DrawText("Stewart Platform Simulation", 10, 10, 20, BLACK);

            // start ImGui Content
            rlImGuiBegin();
                // show ImGui Content
                bool open = true;
                ImGui::ShowDemoWindow(&open);
                ShowStewartGui(&open);
                // Demo_LinePlots();
                // Demo_DigitalPlots();
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

void Demo_LinePlots() {
    static float xs1[1001], ys1[1001];
    for (int i = 0; i < 1001; ++i) {
        xs1[i] = i * 0.001f;
        ys1[i] = 0.5f + 0.5f * sinf(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
    }
    static double xs2[20], ys2[20];
    for (int i = 0; i < 20; ++i) {
        xs2[i] = i * 1/19.0f;
        ys2[i] = xs2[i] * xs2[i];
    }
    if (ImPlot::BeginPlot("Line Plots")) {
        ImPlot::SetupAxes("x","y");
        ImPlot::PlotLine("f(x)", xs1, ys1, 1001);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);
        ImPlot::EndPlot();
    }
}


void Demo_DigitalPlots() {
    ImGui::BulletText("Digital plots do not respond to Y drag and zoom, so that");
    ImGui::Indent();
    ImGui::Text("you can drag analog plots over the rising/falling digital edge.");
    ImGui::Unindent();

    static bool paused = false;
    static ScrollingBuffer dataDigital[2];
    static ScrollingBuffer dataAnalog[2];
    static bool showDigital[2] = {true, false};
    static bool showAnalog[2] = {true, false};

    char label[32];
    ImGui::Checkbox("digital_0", &showDigital[0]); ImGui::SameLine();
    ImGui::Checkbox("digital_1", &showDigital[1]); ImGui::SameLine();
    ImGui::Checkbox("analog_0",  &showAnalog[0]);  ImGui::SameLine();
    ImGui::Checkbox("analog_1",  &showAnalog[1]);

    static float t = 0;
    if (!paused) {
        t += ImGui::GetIO().DeltaTime;
        //digital signal values
        if (showDigital[0])
            dataDigital[0].AddPoint(t, sinf(2*t) > 0.45);
        if (showDigital[1])
            dataDigital[1].AddPoint(t, sinf(2*t) < 0.45);
        //Analog signal values
        if (showAnalog[0])
            dataAnalog[0].AddPoint(t, sinf(2*t));
        if (showAnalog[1])
            dataAnalog[1].AddPoint(t, cosf(2*t));
    }
    if (ImPlot::BeginPlot("##Digital")) {
        ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0, t, paused ? ImGuiCond_Once : ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1);
        for (int i = 0; i < 2; ++i) {
            if (showDigital[i] && dataDigital[i].Data.size() > 0) {
                snprintf(label, sizeof(label), "digital_%d", i);
                ImPlot::PlotDigital(label, &dataDigital[i].Data[0].x, &dataDigital[i].Data[0].y, dataDigital[i].Data.size(), 0, dataDigital[i].Offset, 2 * sizeof(float));
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (showAnalog[i]) {
                snprintf(label, sizeof(label), "analog_%d", i);
                if (dataAnalog[i].Data.size() > 0)
                    ImPlot::PlotLine(label, &dataAnalog[i].Data[0].x, &dataAnalog[i].Data[0].y, dataAnalog[i].Data.size(), 0, dataAnalog[i].Offset, 2 * sizeof(float));
            }
        }
        ImPlot::EndPlot();
    }
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