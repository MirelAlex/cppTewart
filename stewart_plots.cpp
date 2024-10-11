#include "implot.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stewart_plots.h"
#include "main.h"

void StewartPlots::Stewart_RealtimePlots() {
    ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
    Animation* animation = GetCurrentAnimatedPlatform();
    assert(animation != nullptr);
    static ScrollingBuffer anglesBuff[6];
    static ScrollingBuffer transl[3];
    static ScrollingBuffer orient[3];

    static float t = 0;
    if (!animation->isAnimationPaused()) {
        StewartPlatform* plat = animation->getPlatform();
        t += ImGui::GetIO().DeltaTime;
        for (size_t i = 0; i < 6; i++)
        {
            anglesBuff[i].AddPoint(t, plat->legs[i].angle);
        }
        transl[0].AddPoint(t, plat->translation.x);
        transl[1].AddPoint(t, plat->translation.y);
        transl[2].AddPoint(t, plat->translation.z);
        Vector3 rotation = Vector3Scale(QuaternionToEuler(plat->orientation), RAD2DEG);
        orient[0].AddPoint(t, rotation.x);
        orient[1].AddPoint(t, rotation.y);
        orient[2].AddPoint(t, rotation.z);
    }

    static float history = 10.0f;
    ImGui::SliderFloat("History",&history,1,30,"%.1f s");
    // rdata1.Span = history;
    // rdata2.Span = history;

    static ImPlotAxisFlags flagsY = ImPlotAxisFlags_AutoFit;
    static ImPlotAxisFlags flagsX = ImPlotAxisFlags_NoTickLabels;

    if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1,150))) {
        ImPlot::SetupAxes("time", "degree", flagsX, flagsY);
        ImPlot::SetupAxisLimits(ImAxis_X1,t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
        // ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL,0.5f);
        for (size_t i = 0; i < 6; i++)
        {
            char label[20];
            snprintf(label, sizeof(label), "Angle[%d]", i);
            ImPlot::PlotLine(label, &anglesBuff[i].Data[0].x, &anglesBuff[i].Data[0].y, anglesBuff[i].Data.size(), 0, anglesBuff[i].Offset, 2*sizeof(float));
        }

        ImPlot::EndPlot();
    }
    const char* label[3]= { "x", "y", "z" };
    if (ImPlot::BeginPlot("##Scrolling1", ImVec2(-1,150))) {
        ImPlot::SetupAxes("time", "translation", flagsX, flagsY);
        ImPlot::SetupAxisLimits(ImAxis_X1,t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
        // ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL,0.5f);
        for (size_t i = 0; i < 3; i++)
        {
            // char axis[10] = "xyz";
            // char label[50];
            // snprintf(label, sizeof(label), "translation.%s", axis[i]);
            ImPlot::PlotLine(label[i], &transl[i].Data[0].x, &transl[i].Data[0].y, transl[i].Data.size(), 0, transl[i].Offset, 2*sizeof(float));

        }
        ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("##Scrolling2", ImVec2(-1,150))) {
        ImPlot::SetupAxes("time", "orientation", flagsX, flagsY);
        ImPlot::SetupAxisLimits(ImAxis_X1,t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
        // ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL,0.5f);
        for (size_t i = 0; i < 3; i++)
        {
            // char axis[10] = "xyz";
            // char label[50];
            // snprintf(label, sizeof(label), "translation.%s", axis[i]);
            ImPlot::PlotLine(label[i], &orient[i].Data[0].x, &orient[i].Data[0].y, orient[i].Data.size(), 0, orient[i].Offset, 2*sizeof(float));

        }
        ImPlot::EndPlot();
    }
}

void StewartPlots::ShowStewartPlots(bool *p_open){

    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
    ImGui::Begin("Stewart Plots", p_open, ImGuiWindowFlags_MenuBar);

    ImGui::Text("Realtime plots for servo motor angles");
    if (ImGui::TreeNodeEx("Realtime angles")) {
        Stewart_RealtimePlots();
        ImGui::TreePop();
    }
    ImGui::End();
}

