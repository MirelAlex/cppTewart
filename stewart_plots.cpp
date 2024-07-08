#include "implot.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stewart_plots.h"


void StewartPlots::Stewart_RealtimePlots() {
    ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
    static ScrollingBuffer anglesBuff[6];
    static float angles[6];
    getCurrentServoAngles(angles, 6);
    static float t = 0;
    t += ImGui::GetIO().DeltaTime;
    for (size_t i = 0; i < 6; i++)
    {
        anglesBuff[i].AddPoint(t, angles[i]);
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

