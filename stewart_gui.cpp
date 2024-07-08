#include "stewart_gui.h"

//-----------------------------------------------------------------------------
// [SECTION] Helpers
//-----------------------------------------------------------------------------

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}
// Helper to wire demo markers located in code to an interactive browser
typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      SGImGuiDemoMarkerCallback;
extern void*                        SGImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback             SGImGuiDemoMarkerCallback = NULL;
void*                               SGImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (SGImGuiDemoMarkerCallback != NULL) SGImGuiDemoMarkerCallback(__FILE__, __LINE__, section, SGImGuiDemoMarkerCallbackUserData); } while (0)
static void ShowExampleMenuFile();

void ShowStewartGui(bool* p_open)
{
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");
    IMGUI_CHECKVERSION();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Stewart Gui", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            IMGUI_DEMO_MARKER("Menu/File");
                ShowExampleMenuFile();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("Stewart Gui! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGui::Spacing();

    IMGUI_DEMO_MARKER("Help");
    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::SeparatorText("ABOUT THIS DEMO:");
        ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
        ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
        ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
                          "and Metrics/Debugger (general purpose Dear ImGui debugging tool).");

        ImGui::SeparatorText("PROGRAMMER GUIDE:");
        ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
        ImGui::BulletText("See comments in imgui.cpp.");
        ImGui::BulletText("See example applications in the examples/ folder.");
        ImGui::BulletText("Read the FAQ at https://www.dearimgui.com/faq/");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
        ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");

        ImGui::SeparatorText("USER GUIDE:");
        ImGui::ShowUserGuide();
    }

    IMGUI_DEMO_MARKER("Configuration");
    if (ImGui::CollapsingHeader("Configuration"))
    {

        static bool disable_all = false; // The Checkbox for that is inside the "Disabled" section at the bottom
        if (disable_all)
            ImGui::BeginDisabled();

        IMGUI_DEMO_MARKER("Configuration/Camera");
        if (ImGui::TreeNode("Camera"))
        {
            ImGui::SeparatorText("Camera Settings");

            {
                IMGUI_DEMO_MARKER("Configuration/Camera/SliderInt, SliderFloat");
                // static int i1 = 0;
                // ImGui::SliderInt("slider int", &i1, -1, 3);
                // ImGui::SameLine(); HelpMarker("CTRL+click to input value.");
                static Camera* cam = GetCamera();
                // update vec_pos with current camera position; don't use static as we need to update sliders based on current camera everytime
                float vec_pos[3] = { cam->position.x, cam->position.y, cam->position.z};
                static float f1 = 45.0f;

                ImGui::SliderFloat("Fovy", &f1, 20.0f, 90.0f, "fovy = %.2f");
                ImGui::SameLine();
                if (ImGui::Button("Reset")) {f1 = 45.0f;}
                cam->fovy = f1;

                // ImGui::InputFloat3("input float3", vec_pos);
                // ImGui::DragFloat3("drag float3", vec_pos, 0.01f, 0.0f, 1.0f);
                if(ImGui::SliderFloat3("Position", vec_pos, -100.0f, 100.0f)){
                    // update camera postion according to sliders value
                    cam->position.x = vec_pos[0]; cam->position.y = vec_pos[1]; cam->position.z = vec_pos[2];
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset1")) {
                    vec_pos[0]= 30.0f; vec_pos[1]= 30.0f; vec_pos[2] = 30.0f;
                    cam->position.x = vec_pos[0]; cam->position.y = vec_pos[1]; cam->position.z = vec_pos[2];
                }
                // ImGui::InputInt3("input int3", vec4i);
                // ImGui::DragInt3("drag int3", vec4i, 1, 0, 255);
                // ImGui::SliderInt3("slider int3", vec4i, 0, 255);
                // IMGUI_DEMO_MARKER("Configuration/Camera/SliderAngle");
                // static float angle = 0.0f;
                // ImGui::SliderAngle("slider angle", &angle);

                IMGUI_DEMO_MARKER("Configuration/Camera/Camera Projection");
                enum Element { CAMERA_PERSPECTIVE, CAMERA_ORTHOGRAPHIC, Proj_COUNT };
                static int elem = CAMERA_PERSPECTIVE;
                const char* elems_names[Proj_COUNT] = { "CAMERA_PERSPECTIVE", "CAMERA_ORTHOGRAPHIC" };
                const char* elem_name = (elem >= 0 && elem < Proj_COUNT) ? elems_names[elem] : "Unknown";
                ImGui::SliderInt("Projection", &elem, 0, Proj_COUNT - 1, elem_name); // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
                ImGui::SameLine(); HelpMarker("Using the format string parameter to display a name instead of the underlying integer.");
                cam->projection = elem;
            }
            ImGui::TreePop();
        }

        if (disable_all)
            ImGui::EndDisabled();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

static void ShowExampleMenuFile()
{
    IMGUI_DEMO_MARKER("Examples/Menu");
    ImGui::MenuItem("(demo menu)", NULL, false, false);
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    // if (ImGui::BeginMenu("Open Recent"))
    // {
    //     ImGui::MenuItem("fish_hat.c");
    //     ImGui::MenuItem("fish_hat.inl");
    //     ImGui::MenuItem("fish_hat.h");
    //     if (ImGui::BeginMenu("More.."))
    //     {
    //         ImGui::MenuItem("Hello");
    //         ImGui::MenuItem("Sailor");
    //         if (ImGui::BeginMenu("Recurse.."))
    //         {
    //             ShowExampleMenuFile();
    //             ImGui::EndMenu();
    //         }
    //         ImGui::EndMenu();
    //     }
    //     ImGui::EndMenu();
    // }
    // if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    // if (ImGui::MenuItem("Save As..")) {}

    // ImGui::Separator();
    // // IMGUI_DEMO_MARKER("Examples/Menu/Options");
    // if (ImGui::BeginMenu("Options"))
    // {
    //     static bool enabled = true;
    //     ImGui::MenuItem("Enabled", "", &enabled);
    //     ImGui::BeginChild("child", ImVec2(0, 60), ImGuiChildFlags_Border);
    //     for (int i = 0; i < 10; i++)
    //         ImGui::Text("Scrolling Text %d", i);
    //     ImGui::EndChild();
    //     static float f = 0.5f;
    //     static int n = 0;
    //     ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
    //     ImGui::InputFloat("Input", &f, 0.1f);
    //     ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
    //     ImGui::EndMenu();
    // }

    // // IMGUI_DEMO_MARKER("Examples/Menu/Colors");
    // if (ImGui::BeginMenu("Colors"))
    // {
    //     float sz = ImGui::GetTextLineHeight();
    //     for (int i = 0; i < ImGuiCol_COUNT; i++)
    //     {
    //         const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
    //         ImVec2 p = ImGui::GetCursorScreenPos();
    //         ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
    //         ImGui::Dummy(ImVec2(sz, sz));
    //         ImGui::SameLine();
    //         ImGui::MenuItem(name);
    //     }
    //     ImGui::EndMenu();
    // }

    // // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // // In a real code-base using it would make senses to use this feature from very different code locations.
    // if (ImGui::BeginMenu("Options")) // <-- Append!
    // {
    //     // IMGUI_DEMO_MARKER("Examples/Menu/Append to an existing menu");
    //     static bool b = true;
    //     ImGui::Checkbox("SomeOption", &b);
    //     ImGui::EndMenu();
    // }

    // if (ImGui::BeginMenu("Disabled", false)) // Disabled
    // {
    //     IM_ASSERT(0);
    // }
    // if (ImGui::MenuItem("Checked", NULL, true)) {}
    // ImGui::Separator();
    // if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}
