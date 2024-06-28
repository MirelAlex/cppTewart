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

#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"
#include <cstdio>
void Demo_LinePlots();
void Demo_DigitalPlots();


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
	int screenWidth = 1280;
	int screenHeight = 800;
    // int   bar_data[11] = {0,1,2,3,4,5,6,7,8,9,10};
    // float x_data[10] =  {0,1,2,3,4,5,6,7,8,9};
    // float y_data[10] =  {0,1,2,3,4,5,6,7,8,9};
    // FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT |
	SetConfigFlags( FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "raylib-Extras [ImGui] example - simple ImGui Demo");
	SetTargetFPS(60);
	rlImGuiSetup(true);
    ImPlot::CreateContext();
	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);

		// start ImGui Conent
		rlImGuiBegin();

            // show ImGui Content
            bool open = true;
            ImGui::ShowDemoWindow(&open);
            Demo_LinePlots();
            Demo_DigitalPlots();
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
