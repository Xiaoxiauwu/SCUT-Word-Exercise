#include "UIInternal.h"

namespace scut_ui {
void RenderHome() {
    const float menuOffset = buttonwidth / kbw * 0.5f;
    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f });
    drawText(width / 2, height / 4, height / 10, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), encode::string_To_UTF8("SCUT学术英语词汇练习"));

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) * (-1) - menuOffset });
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
    if (ImGui::Button(u8"开始练习", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size()==0)   //  注册按钮类
    {
        //ButtonLock = 1;
        for (int i = 0; i < dx.size(); i++) {
            animeManager.unBind(&dx[i]);//解绑，防止清空vector的时候，anime还在更新不存在的值（虽然不太可能有这种情况出现）
        }
        dx.clear();
        rearrange = 1;
        TabStack.push(TAB::ProblemsSetSelect);

        animeManager.addAnime(&switchFrameV, 255, 0, FrameDelay, OutSine, 0);
        //animeManager.addAnime(&animeTimeL, 0, 1, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::ProblemsSetSelect;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, 1, FrameDelay, InSine);
        //});
    }
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) * 0 - menuOffset });
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
    if (ImGui::Button(u8"错题练习", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        if (userInfo["WA"].size() == 0) {
            ButtonLock = 1;
            function<void(bool)> window = [&](bool alive)->void {
                drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
                float w, h, x, y;//长宽，中心坐标
                w = width / 2.8;
                h = height / 4;
                x = width / 2;
                y = height / 2;
                ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
                ImGui::BeginChild(
                    u8"空",
                    ImVec2(w, h),
                    1,
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoSavedSettings
                );
                drawText(
                    w / 2, Screen_Width / 1024 * 24.0f / 2,
                    Screen_Width / 1024 * 24.0f / 2,
                    ImVec4(255, 255, 255, 255),
                    u8"Time Limit Exceeded"
                );
                ImGui::Indent(width / 30);
                ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
                ImGui::Text(u8"你还没有错题，或你已经补过题了");

                //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

                ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
                ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
                if (ImGui::Button(u8"了解", { buttonwidth / 2,buttonwidth / kbw }) && alive)
                {
                    subWindow.pop_back();
                    ButtonLock = 0;
                }
                ImGui::PopStyleColor();
                ImGui::EndChild();
                };
            subWindow.push_back({ window,0,0 });
        }
        else {
            std::vector<Json::Value> pool;
            for (const auto& q : userInfo["WA"]) pool.push_back(q);
            if (exercise.start(pool, true, myrand)) TabStack.push(TAB::Exercise);

        }
        //animeManager.addAnime(&animeTimeL, 0, 1, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::ProblemsSetSelect;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, 1, FrameDelay, InSine);
        //});
    }
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) - menuOffset});
    if (ImGui::Button(u8"错题管理", {buttonwidth, buttonwidth / kbw}) && !ButtonLock && subWindow.empty()) {
        TabStack.push(WrongAnswers);
        animeManager.addAnime(&switchFrameV,255,0,FrameDelay,OutSine,0);
    }

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) * 2 - menuOffset });
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
    if (ImGui::Button(u8"题库管理", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        TabStack.push(TAB::EditorProblemSetSelect);
        animeManager.addAnime(&switchFrameV, 255, 0, FrameDelay, OutSine, 0);
        //ButtonLock = 1;
        //function<void(bool)> window = [&](bool alive)->void {
        //    drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
        //    float w, h, x, y;//长宽，中心坐标
        //    w = width / 2.8;
        //    h = height / 4;
        //    x = width / 2;
        //    y = height / 2;
        //    ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
        //    ImGui::BeginChild(
        //        u8"咕咕",
        //        ImVec2(w, h),
        //        1,
        //        ImGuiWindowFlags_NoResize |
        //        ImGuiWindowFlags_NoSavedSettings
        //    );
        //    drawText(
        //        w / 2, Screen_Width / 1024 * 24.0f / 2,
        //        Screen_Width / 1024 * 24.0f / 2,
        //        ImVec4(255, 255, 255, 255),
        //        u8"Not Found"
        //    );
        //    ImGui::Indent(width / 30);
        //    ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
        //    ImGui::Text(u8"暂时没写");

        //    //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

        //    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
        //    ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
        //    if (ImGui::Button(u8"知道了", { buttonwidth / 2,buttonwidth / kbw }) && alive)
        //    {
        //        subWindow.pop_back();
        //        ButtonLock = 0;
        //    }
        //    ImGui::PopStyleColor();
        //    ImGui::EndChild();
        //    };
        //subWindow.push_back({ window,0,0 });
    }
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) * 3 - menuOffset });
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
    if (ImGui::Button(u8"关于", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        TabStack.push(About);
        animeManager.addAnime(&switchFrameV, 255, 0, FrameDelay, OutSine, 0);
    }
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) * 4 - menuOffset });
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
    if (ImGui::Button(u8"退出", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        //ButtonLock = 1;
        TabStack.pop();
        //animeManager.addAnime(&animeTimeL, 0, 1, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::ProblemsSetSelect;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, 1, FrameDelay, InSine);
        //});
    }
    ImGui::PopStyleColor();

    drawText(width / 2, height - width / 120 - Screen_Width / 1024 * 12.0f, Screen_Width / 1024 * 12.0f, ImVec4(0.6, 0.6, 0.6, 1), u8"Version 1.0.3  By XiaoXia");
}


}
