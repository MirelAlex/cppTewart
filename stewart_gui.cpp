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

        if (ImGui::TreeNode("Settings"))
        {
            ImGui::SeparatorText("Platform Settings");

            {
                // IMGUI_DEMO_MARKER("Configuration/Camera/SliderInt, SliderFloat");
                // static int i1 = 0;
                // ImGui::SliderInt("slider int", &i1, -1, 3);
                // ImGui::SameLine(); HelpMarker("CTRL+click to input value.");
                Animation* anim = GetCurrentAnimatedPlatform();
                StewartPlatform* plat = anim->getPlatform();
                ImVec4 Bcolor = ImVec4(plat->base.color.r / 255.0f,
                                      plat->base.color.g / 255.0f,
                                      plat->base.color.b / 255.0f,
                                      plat->base.color.a / 255.0f);
                ImVec4 Pcolor = ImVec4(plat->plat.color.r / 255.0f,
                                       plat->plat.color.g / 255.0f,
                                       plat->plat.color.b / 255.0f,
                                       plat->plat.color.a / 255.0f);

                ImGui::Checkbox("Show vectors", &plat->drawVectors);

                ImGui::SameLine();
                static bool check1 = true;
                ImGui::Checkbox("Show path", &check1);
                if (check1){
                    anim->bDrawPath = true;
                }else{
                    anim->bDrawPath = false;
                }

                static bool hexActive = true;
                ImGui::Checkbox("Hexagonal Platform", &hexActive);
                if (hexActive){
                    setCurrentAnimatedHexPlatform();
                }else{
                    setCurrentAnimatedCircularPlatform();
                }

                float vec_pos[] = {plat->pos.x, plat->pos.y, plat->pos.z};

                if(ImGui::SliderFloat3("Position", vec_pos, -100.0f, 100.0f)){
                    // update camera postion according to sliders value
                    plat->pos.x = vec_pos[0]; plat->pos.y = vec_pos[1]; plat->pos.z = vec_pos[2];
                }

                if(ImGui::SliderFloat("Rod Length", &plat->rodLength, 0.0f, 10.0f)){
                   plat->refreshPlatform();
                }
                if(ImGui::SliderFloat("Horn Length", &plat->hornLength, 0.0f, 10.0f)){
                   plat->refreshPlatform();
                }
                if(ImGui::SliderFloat("Platform Radius", &plat->plat.radius, 0.0f, 10.0f)){
                    plat->refreshPlatform();
                }
                if(ImGui::SliderFloat("Base Radius", &plat->base.radius, 0.0f, 10.0f)){
                    plat->refreshPlatform();
                }

                if (!hexActive) ImGui::BeginDisabled();
                    if(ImGui::SliderFloat("Outer plat rad", &plat->platformOuterRadius, 0.0f, 10.0f)){
                        plat->refreshPlatform();
                    }
                    if(ImGui::SliderFloat("Outer base rad", &plat->baseOuterRadius, 0.0f, 10.0f)){
                        plat->refreshPlatform();
                    }
                if (!hexActive) ImGui::EndDisabled();

                if(ImGui::SliderFloat("Anchor distance", &plat->plat.anchorDistance, 0.0f, 10.0f)){
                    plat->refreshPlatform();
                }
                if(ImGui::SliderFloat("Shaft distance", &plat->base.shaftDistance, 0.0f, 10.0f)){
                    plat->refreshPlatform();
                }
                if(ImGui::SliderFloat("Base thickness", &plat->base.thickness, 0.0f, 2.0f)){
                    plat->refreshPlatform();
                }
                if(ImGui::SliderFloat("Plat thickness", &plat->plat.thickness, 0.0f, 2.0f)){
                    plat->refreshPlatform();
                }

                if(ImGui::ColorEdit4("Base color", (float*)&Bcolor, ImGuiColorEditFlags_Uint8)){
                    uint8 r = Bcolor.x * 255;
                    uint8 g = Bcolor.y * 255;
                    uint8 b = Bcolor.z * 255;
                    uint8 a = Bcolor.w * 255;
                    Color cb = {r, g, b, a};
                    plat->base.color = cb;
                }
                if(ImGui::ColorEdit4("Plat color", (float*)&Pcolor, ImGuiColorEditFlags_Uint8)){
                    uint8 r = Pcolor.x * 255;
                    uint8 g = Pcolor.y * 255;
                    uint8 b = Pcolor.z * 255;
                    uint8 a = Pcolor.w * 255;
                    Color cp = {r, g, b, a};
                    plat->plat.color = cp;
                }

                if (ImGui::Button("Save Config")) {plat->saveConfig("stewart_config.json");}


            }
            ImGui::TreePop();
        }



    }

    ImGui::PopItemWidth();
    ImGui::End();
}

// static ColorPicker(Animation *anim){

//     // Generate a default palette. The palette will persist and can be edited.
//     static bool saved_palette_init = true;
//     static ImVec4 saved_palette[32] = {};
//     if (saved_palette_init)
//     {
//         for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
//         {
//             ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
//                 saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
//             saved_palette[n].w = 1.0f; // Alpha
//         }
//         saved_palette_init = false;
//     }

//     static ImVec4 backup_color;
//     bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
//     ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
//     open_popup |= ImGui::Button("Palette");
//     if (open_popup)
//     {
//         ImGui::OpenPopup("mypicker");
//         backup_color = color;
//     }
//     if (ImGui::BeginPopup("mypicker"))
//     {
//         ImGui::Text("MY CUSTOM COLOR PICKER WITH AN AMAZING PALETTE!");
//         ImGui::Separator();
//         ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
//         ImGui::SameLine();

//         ImGui::BeginGroup(); // Lock X position
//         ImGui::Text("Current");
//         ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
//         ImGui::Text("Previous");
//         if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
//             color = backup_color;
//         ImGui::Separator();
//         ImGui::Text("Palette");
//         for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
//         {
//             ImGui::PushID(n);
//             if ((n % 8) != 0)
//                 ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

//             ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
//             if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
//                 color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

//             // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
//             // drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
//             if (ImGui::BeginDragDropTarget())
//             {
//                 if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
//                     memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
//                 if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
//                     memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
//                 ImGui::EndDragDropTarget();
//             }

//             ImGui::PopID();
//         }
//         ImGui::EndGroup();
//         ImGui::EndPopup();
//     }
// }


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
