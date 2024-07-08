#pragma once

#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "implot.h"
#include "rlImGui.h"
#include <cstdio>

#include "stewart_platform.h"
#include "animation.h"
#include "stewart_gui.h"
#include "stewart_plots.h"

// debug interfaces
Animation* GetPlatformAnimation();
Camera* GetCamera();