#include "UIInternal.h"

namespace scut_ui {

void ApplyTheme() {
    ImGuiStyle& Style = ImGui::GetStyle();
    Color = Style.Colors;

    switch (themeIndex)
    {
    case Color_::Red:
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(172, 31, 54, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(192, 51, 74, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(172, 31, 54, 255);

        Color[ImGuiCol_Header] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(172, 31, 54, 255);

        Color[ImGuiCol_TitleBg] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_TitleBgActive] = ImColor(172, 31, 54, 255);
        Color[ImGuiCol_ResizeGrip] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_ResizeGripHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_ResizeGripActive] = ImColor(172, 31, 54, 255);

        //ImGuiCol_Separator,
        Color[ImGuiCol_SeparatorHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_SeparatorActive] = ImColor(172, 31, 54, 255);
        break;
    case Color_::Green:
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(10, 105, 56, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(30, 125, 76, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(0, 95, 46, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(10, 105, 56, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(10, 105, 56, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(0, 95, 46, 255);

        Color[ImGuiCol_Header] = ImColor(10, 105, 56, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(30, 125, 76, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(0, 95, 46, 255);

        break;
    case Color_::Blue:
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(51, 120, 255, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(71, 140, 255, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(31, 100, 225, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(51, 120, 255, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(51, 120, 255, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(31, 100, 225, 255);

        Color[ImGuiCol_Header] = ImColor(51, 120, 255, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(71, 140, 255, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(31, 100, 225, 255);

        break;
    case Color_::Orange://233,87,33
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(233, 87, 33, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(253, 107, 53, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(213, 67, 13, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(233, 87, 33, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(233, 87, 33, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(213, 67, 13, 255);

        Color[ImGuiCol_Header] = ImColor(233, 87, 33, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(253, 107, 53, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(213, 67, 13, 255);

        break;
    }

}

} // namespace scut_ui
