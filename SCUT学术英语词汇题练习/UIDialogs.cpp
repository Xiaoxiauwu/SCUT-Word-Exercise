#include "UIInternal.h"

namespace scut_ui {

void popWindow(std::string title, std::string content, ImVec4 color) {
    ButtonLock = 1;
    function<void(bool)> window = [title, content, color](bool alive)->void {//如何保留一些临时变量
        drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
        float w, h, x, y;//长宽，中心坐标
        w = width / 2.8;
        h = height / 4;
        x = width / 2;
        y = height / 2;
        ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
        ImGui::BeginChild(
            (title + '_').c_str(),
            ImVec2(w, h),
            1,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings
        );
        drawText(
            w / 2, Screen_Width / 1024 * 24.0f / 2,
            Screen_Width / 1024 * 24.0f / 2,
            ImVec4(255, 255, 255, 255),
            title.c_str()
        );
        ImGui::Indent(width / 30);
        ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
        drawText_autoNewLine(width / 30, Screen_Width / 1024 * 24.0f, width / 50, color, content.c_str(), width / 2.8 - width / 30, u8" ", 0);

        ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
        ImGui::SetCursorPos({ w / 2 - buttonwidth / 4, h - buttonwidth / kbw * 1.3f });
        if (ImGui::Button(u8"确定", { buttonwidth / 2,buttonwidth / kbw }) && alive)
        {
            subWindow.pop_back();
            ButtonLock = 0;
        }
        ImGui::PopStyleColor();

        ImGui::EndChild();
        };
    subWindow.push_back({ window,0,0 });
}


void RenderDialogs() {
    const auto count = subWindow.size();
    for (size_t i = 0; i < count && i < subWindow.size(); ++i) {
        auto dialog = subWindow[i];
        if (i == count - 1 || dialog.isDraw)
            dialog.window(i == count - 1 || dialog.isAct);
    }
}

} // namespace scut_ui

namespace scut_ui {
namespace {
int ResizeTextBuffer(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto& text = *static_cast<std::string*>(data->UserData);
        text.resize(data->BufTextLen);
        data->Buf = text.data();
    }
    return 0;
}
}
bool EditText(const char* label, std::string& text, bool multiline, ImVec2 size) {
    if (text.capacity() < 256) text.reserve(256);
    return multiline
        ? ImGui::InputTextMultiline(label, text.data(), text.capacity()+1, size,
             ImGuiInputTextFlags_CallbackResize, ResizeTextBuffer, &text)
        : ImGui::InputText(label, text.data(), text.capacity()+1,
             ImGuiInputTextFlags_CallbackResize, ResizeTextBuffer, &text);
}
bool PageHeader(const char* title) {
    ImGui::SetCursorPos(ImVec2(gap, gap));
    if (ImGui::Button(u8"返回", ImVec2(buttonwidth/2, buttonwidth/kbw))) {
        TabStack.pop();
        return false;
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(title);
    ImGui::Separator();
    return true;
}
void SavePracticeSettings() {
    userInfo["preferences"]["collectWrongAnswers"] = exercise.collectWrongAnswers;
    userInfo["preferences"]["removeCorrectWrongAnswers"] = exercise.removeCorrectWrongAnswers;
    SaveUserData();
}
}
