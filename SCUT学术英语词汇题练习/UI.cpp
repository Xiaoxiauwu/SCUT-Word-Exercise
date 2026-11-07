#include "UIInternal.h"

#include <cfloat>
namespace {
void ConstrainAspectRatio(ImGuiSizeCallbackData* data) {
    const auto size = scut_ui::ConstrainWindowSize(
        {data->CurrentSize.x, data->CurrentSize.y},
        {data->DesiredSize.x, data->DesiredSize.y});
    data->DesiredSize = ImVec2(size.width, size.height);
}
}
// Public entry points retained for GUI.cpp. UI state is private to scut_ui.
void init(ID3D11Device* device) { scut_ui::Initialize(device); }
void getScreenRect() { scut_ui::UpdateScreenRect(); }
void saveUserData() { scut_ui::SaveUserData(); }

void MainUI() {
    using namespace scut_ui;
    if (TabStack.size() == 0) EXIT();

    ApplyTheme();

    ImVec2 initialSize(960, 640), initialPos(100, 100);
    const auto& monitors = ImGui::GetPlatformIO().Monitors;
    if (!monitors.empty()) {
        const auto& monitor = monitors[0];
        initialSize.x = MIN(initialSize.x, monitor.WorkSize.x * 0.85f);
        initialSize.y = MIN(initialSize.y, monitor.WorkSize.y * 0.85f);
        const auto fitted = FitLayout(initialSize.x, initialSize.y);
        initialSize = ImVec2(fitted.width, fitted.height);
        initialPos = monitor.WorkPos + (monitor.WorkSize - initialSize) * 0.5f;
    }
    ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(660, 440), ImVec2(FLT_MAX, FLT_MAX), ConstrainAspectRatio);
    ImGui::Begin(
        u8"SCUT学术英语词汇练习",
        NULL,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoSavedSettings |//|
        //ImGuiWindowFlags_NoMove//请添加移动条谢谢
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );

    UpdateScreenRect();

    switch (TabStack.top()) {
        case TAB::Main:
            RenderHome();
            break;
        case TAB::ProblemsSetSelect:
            RenderProblemSetSelection();
            break;
        case TAB::Exercise:
            RenderExercise();
            break;
        case TAB::EditorProblemSetSelect:
            RenderProblemSetManager();
            break;
        case TAB::Editor:
            RenderEditor();
            break;
        case TAB::About:
            RenderAbout();
            break;
        case TAB::WrongAnswers:
            RenderWrongAnswers();
            break;
    }

    RenderDialogs();

    // The viewport foreground is submitted after child windows, so the transition
    // mask stays above every page and dialog without inheriting child clipping.
    const ImVec2 windowPos = ImGui::GetWindowPos();
    ImGui::GetForegroundDrawList(ImGui::GetWindowViewport())->AddRectFilled(
        windowPos, windowPos + ImGui::GetWindowSize(),
        IM_COL32(15, 15, 15, static_cast<int>((std::max)(0.0f, (std::min)(255.0f, switchFrameV)))));
    ImGui::End();
    animeManager.updateAnime();
}
