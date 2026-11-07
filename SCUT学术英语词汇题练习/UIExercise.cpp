#include "UIInternal.h"

namespace scut_ui {
namespace {
std::string FormatPracticeTime(double seconds, bool fractions = false) {
    const auto centiseconds = static_cast<long long>(seconds * 100.0);
    const auto totalSeconds = centiseconds / 100;
    std::ostringstream out;
    out << std::setfill('0') << std::setw(2) << totalSeconds / 3600 << ':'
        << std::setw(2) << totalSeconds / 60 % 60 << ':' << std::setw(2) << totalSeconds % 60;
    if (fractions) out << '.' << std::setw(2) << centiseconds % 100;
    return out.str();
}
}
void RenderExercise() {

    if (exercise.questions.empty()) { TabStack.pop(); return; }
    int cnt = exercise.questions.size();
    bool advance = false;
    const auto& question = exercise.questions[exercise.currentIndex];
    float WIDTH = width * 2 / 3 - gap;
    const float buttonHeight = buttonwidth / kbw;
    const float textSize = width / 50;
    drawText(width / 3, gap + buttonHeight / 2, width / 60, ImVec4(1, 1, 1, 1),
        std::string(u8"用时 ") + FormatPracticeTime(exercise.elapsedSeconds()));
    std::string resourceDirectory = ".";
    const auto sourceName = encode::UTF8_To_String(question["SourceSet"].asString());
    if (const auto source = problemSets.nameToPath.find(sourceName); source != problemSets.nameToPath.end())
        resourceDirectory = std::filesystem::path(source->second).parent_path().string();
    const std::string problemText = std::to_string(exercise.currentIndex + 1)
        + (IsMultiple(question) ? u8". [多选] " : ". ") + question["problem"].asString();
    const float normalOptionWidth = buttonwidth;
    const float horizontalPadding = ImGui::GetStyle().FramePadding.x;
    std::vector<std::string> choiceLabels;
    bool singleColumn = false;
    std::vector<int> sourceChoices;
    for (int i = 0; i < static_cast<int>(question["choices"].size()); ++i) {
        const int sourceIndex = exercise.displayedChoiceIndex(exercise.currentIndex, i);
        sourceChoices.push_back(sourceIndex);
        choiceLabels.push_back(std::string(1, static_cast<char>('A' + i)) + ". " + question["choices"][sourceIndex].asString());
        if (ImGui::CalcTextSize(choiceLabels.back().c_str()).x > normalOptionWidth - 2 * horizontalPadding)
            singleColumn = true;
    }
    const float optionWidth = singleColumn ? WIDTH - 2 * gap : normalOptionWidth;
    const float wrappedTextWidth = (std::max)(1.0f, optionWidth - 2 * horizontalPadding);
    std::vector<float> optionHeights;
    std::vector<float> optionTextHeights;
    for (const auto& label : choiceLabels) {
        const float textHeight = MeasureMarkdown(label, wrappedTextWidth, textSize, resourceDirectory);
        optionTextHeights.push_back(textHeight);
        const float optionHeight = (std::max)(buttonHeight, textHeight + 2 * ImGui::GetStyle().FramePadding.y + 2 * gap);
        optionHeights.push_back(optionHeight);
    }
    std::vector<float> columnHeights(singleColumn ? 1 : 2, 0.0f);
    for (int i = 0; i < static_cast<int>(optionHeights.size()); ++i)
        columnHeights[singleColumn ? 0 : i % 2] += optionHeights[i] + gap;
    const float optionsHeight = *std::max_element(columnHeights.begin(), columnHeights.end());
    const float problemHeight = MeasureMarkdown(problemText, WIDTH - 3 * gap, textSize, resourceDirectory);
    const float contentHeight = problemHeight + 2 * gap + optionsHeight;
    const float maxCardHeight = height * 0.68f;
    float HEIGHT = (std::min)(contentHeight, maxCardHeight);
    const auto drawOptionLabel = [&](float x, float y, float buttonH, float contentHeight, const std::string& label) {
        return RenderMarkdown(label, ImVec2(x + horizontalPadding, y + (buttonH - contentHeight) / 2),
            wrappedTextWidth, textSize, resourceDirectory);
    };
    ImGui::SetCursorPos({ gap, height / 2 - HEIGHT / 2 });
    ImGui::BeginChild(
        u8"题",
        ImVec2(WIDTH, HEIGHT),//height - gap - buttonwidth / kbw - 2 * gap),
        1,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_AlwaysVerticalScrollbar// |
        //ImGuiWindowFlags_NoMove
    ); {
        
        RenderMarkdown(problemText, ImVec2(gap, gap), WIDTH - 3 * gap, textSize, resourceDirectory);

        
        std::function<void(int)> setColor = [&](int i) -> void {
            if (exercise.questions[exercise.currentIndex]["Locked"].asBool()) {//已有结果
                ImVec4 tmp = ImVec4(44 / 255.0f, 44 / 255.0f, 44 / 255.0f, 150 / 255.0f);
                const bool selected = ChoiceSelected(question, i);
                const bool correct = ChoiceCorrect(question, i);
                if (selected && !correct)
                    tmp = ImVec4(0.85f, 0.18f, 0.18f, 0.75f);
                else if (correct && selected)
                    tmp = ImVec4(0.12f, 0.72f, 0.24f, 0.75f);
                else if (correct && IsMultiple(question))
                    tmp = ImVec4(0.95f, 0.62f, 0.12f, 0.85f);
                else if (correct)
                    tmp = ImVec4(0.12f, 0.72f, 0.24f, 0.75f);
                ImGui::PushStyleColor(ImGuiCol_Button, tmp);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, tmp);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, tmp);                
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, 
                    !ChoiceSelected(exercise.questions[exercise.currentIndex], i) ? ImVec4(44 / 255.0f, 44 / 255.0f, 44 / 255.0f, 150 / 255.0f) : Color[ImGuiCol_Button]
                );
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 
                    !ChoiceSelected(exercise.questions[exercise.currentIndex], i) ? ImVec4(54 / 255.0f, 54 / 255.0f, 54 / 255.0f, 150 / 255.0f) : Color[ImGuiCol_ButtonHovered]
                );
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, 
                    !ChoiceSelected(exercise.questions[exercise.currentIndex], i) ? ImVec4(46 / 255.0f, 46 / 255.0f, 46 / 255.0f, 150 / 255.0f) : Color[ImGuiCol_ButtonActive]
                );
            }
        };

        std::vector<float> columnOffsets(singleColumn ? 1 : 2, 0.0f);
        for (int i = 0; i < static_cast<int>(choiceLabels.size()); ++i) {
            const int column = singleColumn ? 0 : i % 2;
            const float optionY = gap + problemHeight + gap + columnOffsets[column];
            const float x = singleColumn ? gap : (i & 1 ? WIDTH - gap - optionWidth : gap);
            const float optionHeight = optionHeights[i];
            ImGui::SetCursorPos({x, optionY});
            const int sourceIndex = sourceChoices[i];
            setColor(sourceIndex);
            const bool pressed = ImGui::Button(("##choice_" + std::to_string(i)).c_str(), {optionWidth, optionHeight});
            ImGui::SetItemAllowOverlap();
            ImGui::PopStyleColor(3);
            const bool linkActivated = drawOptionLabel(x, optionY, optionHeight, optionTextHeights[i], choiceLabels[i]);
            if (pressed && !linkActivated && !ButtonLock && subWindow.empty() && !question["Locked"].asBool()) {
                ToggleChoice(exercise.questions[exercise.currentIndex],sourceIndex);
                if (exercise.autoJudge && !IsMultiple(exercise.questions[exercise.currentIndex])) {
                    exercise.submitCurrent(userInfo); SaveUserData();
                    advance = exercise.autoAdvance && exercise.questions[exercise.currentIndex]["statu"].asInt()==Accepted;
                }
            }
            columnOffsets[column] += optionHeight + gap;
        }
        if (ButtonLock) drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
    }
    ImGui::EndChild();

    ImVec2 label_size = ImGui::CalcTextSize(u8"允许题目重复", NULL, true);
    ImGui::SetCursorPos({gap, height - (gap + label_size.y) * 4 - gap});
    if (ImGui::Checkbox(u8"选项随机", &exercise.randomizeChoices))
        exercise.setChoiceRandomization(exercise.randomizeChoices, myrand);

    ImGui::SetCursorPos({ gap , height - (gap + label_size.y) * 3 - gap });
    if (ImGui::Checkbox(u8"允许题目重复", &exercise.allowRepeated)) {
        //如果点了会怎么样
        //测试用接口
        //std::cout << exercise.questions[exercise.currentIndex]["problem"].asString() << "\n";
    }

    ImGui::SetCursorPos({ gap , height - (gap + label_size.y) * 2 - gap });
    if (ImGui::Checkbox(u8"自动判题", &exercise.autoJudge)) {
        if (!exercise.autoJudge) exercise.autoAdvance=false;
        //什么事都没有感觉
    }

    ImGui::SetCursorPos({gap, height-(gap+label_size.y)-gap});
    if (ImGui::Checkbox(u8"自动判题 + 自动下一题", &exercise.autoAdvance) && exercise.autoAdvance) exercise.autoJudge=true;

    //exercise.questions
    const bool currentPending = !exercise.questions[exercise.currentIndex]["Locked"].asBool()
        && HasSelection(exercise.questions[exercise.currentIndex]);
    if (exercise.hasPendingAnswers()) {
        ImGui::SetCursorPos({ width / 3 - buttonwidth / 2, height / 2 + HEIGHT / 2 + gap + gap + buttonwidth / kbw });
        ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
        if (ImGui::Button(u8"判题", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)
        {
            exercise.submitSelected(userInfo); SaveUserData();
            advance=exercise.autoAdvance && currentPending
                && exercise.questions[exercise.currentIndex]["statu"].asInt()==Accepted;
        }
        ImGui::PopStyleColor();
    }

    if (HasSelection(exercise.questions[exercise.currentIndex]) || exercise.currentIndex < cnt - 1) {//如果不准备开新题，或者已经选了最后一题，那么允许“下一题”
        ImGui::SetCursorPos({ width / 3 - buttonwidth / 2, height / 2 + HEIGHT / 2 + gap });
        ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
        if (ImGui::Button(u8"下一题", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)
        {
            advance = true;
        }
        ImGui::PopStyleColor();
    }

    ImGui::SetCursorPos({width-buttonwidth/2-gap, gap});
    if (ImGui::Button(u8"错题设置", {buttonwidth/2, buttonwidth/kbw})) ImGui::OpenPopup("wrong_settings");
    if (ImGui::BeginPopup("wrong_settings")) {
        int policy=exercise.questions[exercise.currentIndex].get("WrongAnswerPolicy",0).asInt();
        if (ImGui::Combo(u8"本题", &policy, u8"自动收录\0收录本题\0不收录本题\0")) {
            exercise.setWrongAnswerPolicy(exercise.currentIndex,policy,userInfo); SaveUserData();
        }
        if (ImGui::Checkbox(u8"自动收录答错题", &exercise.collectWrongAnswers)) SavePracticeSettings();
        if (exercise.wrongAnswerPractice && ImGui::Checkbox(u8"答对后移出错题集", &exercise.removeCorrectWrongAnswers)) SavePracticeSettings();
        ImGui::TextUnformatted(IsMultiple(exercise.questions[exercise.currentIndex]) ? u8"多选题：选完点击判题。" : u8"单选题");
        ImGui::EndPopup();
    }

    WIDTH = width / 3 - 2 * gap;
    HEIGHT = height - gap - buttonwidth / kbw - 2 * gap;
    ImGui::SetCursorPos({ width * 2 / 3 + gap , gap + buttonwidth / kbw + gap });
    ImGui::BeginChild(
        u8"列表",
        ImVec2(WIDTH, HEIGHT),
        1,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings// |
        //ImGuiWindowFlags_NoMove
    ); {
        float siz = (WIDTH - 3 * gap) / 11;
        for (int i = 0; i < cnt; i++) {
            drawRect(gap / 2 + (siz + gap / 2) * (i % 10), gap + (siz + gap / 2) * (i / 10), siz, siz, TinyButton[exercise.questions[i]["statu"].asInt()]);
            drawText(gap / 2 + (siz + gap / 2) * (i % 10) + siz / 2, gap + (siz + gap / 2) * (i / 10) + siz / 2, width / 60, ImVec4(0, 0, 0, 255.0f), std::to_string(i + 1));
            ImGui::SetCursorPos({ gap / 2 + (siz + gap / 2) * (i % 10), gap + (siz + gap / 2) * (i / 10) });
            if (PaintBoard((std::string("_") + std::to_string(i + 1)).c_str(), {siz, siz}) && !ButtonLock && subWindow.size() == 0)
            {
                //std::cout << i << "]\n";
                exercise.currentIndex = i;
            }
        }
        if (ButtonLock) drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
    }
    ImGui::EndChild();

    ImGui::SetCursorPos({ gap, gap });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"结算", { buttonwidth / 4, buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        exercise.finish(userInfo); SaveUserData();



        ButtonLock = 1;
        const double totalSeconds = exercise.elapsedSeconds();
        const double pps = exercise.correctPerSecond();
        function<void(bool)> window = [totalSeconds, pps](bool alive)->void {
            drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
            float w, h, x, y;//长宽，中心坐标
            w = width / 2.8;
            h = height * 0.4f;
            x = width / 2;
            y = height / 2;
            ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
            ImGui::BeginChild(
                u8"无",
                ImVec2(w, h),
                1,
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings
            );
            drawText(
                w / 2, Screen_Width / 1024 * 24.0f / 2,
                Screen_Width / 1024 * 24.0f / 2,
                ImVec4(255, 255, 255, 255),
                u8"Training Complished"
            );
            ImGui::Indent(width / 30);
            ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
            std::stringstream ss;
            ss << std::setprecision(2) << std::fixed << (exercise.gradedCount() ? 100.0 * exercise.acceptedCount / exercise.gradedCount() : 0.0);//   显示图像缩放进程
            ImGui::Text(encode::string_To_UTF8(std::string("您的正确率为: ") + ss.str() + std::string("%%")).c_str());
            ImGui::Text(u8"答对题数：%d / %d", exercise.acceptedCount, exercise.gradedCount());
            ImGui::Text(u8"总用时：%s", FormatPracticeTime(totalSeconds, true).c_str());
            ImGui::Text(u8"p/s：%.3f", pps);

            //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

            ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
            ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
            if (ImGui::Button(u8"稳了", { buttonwidth / 2,buttonwidth / kbw }) && alive)
            {
                subWindow.pop_back();
                TabStack.pop();
                ButtonLock = 0;

                exercise.wrongAnswerPractice = false;
                exercise.questions.clear();
                exercise.visited.clear();
                exercise.unvisited.clear();
            }
            ImGui::PopStyleColor();
            ImGui::EndChild();
            };
        subWindow.push_back({ window,0,0 });
    }
    ImGui::PopStyleColor();
    if (advance && !exercise.next(myrand)) popWindow(u8"Undersupply", u8"这些题单你已经全刷完了");

}


}
