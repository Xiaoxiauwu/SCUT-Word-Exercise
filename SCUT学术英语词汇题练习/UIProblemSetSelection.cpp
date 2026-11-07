#include "UIInternal.h"

namespace scut_ui {
void RenderProblemSetSelection() {
    static int list = 0;//0: 学术英语，1: 收藏夹，2：所有题单
    static float dy = gap + buttonwidth / kbw + gap;

    static ImVec2 previousSize;
    if (previousSize.x != width || previousSize.y != height) {
        animeManager.unBind(&dy);
        dy = gap + buttonwidth / kbw + gap + (height - 3*gap - buttonwidth/kbw)/3*list;
        for (auto& offset : dx) { animeManager.unBind(&offset); offset = 0; }
        previousSize = ImVec2(width,height);
    }
    float WIDTH = width / 3;
    float HEIGHT = height - gap - buttonwidth / kbw - 2 * gap;

    drawRect(gap + buttonwidth / kbw, dy, gap / 2, HEIGHT / 3, Color[ImGuiCol_Button]);

    drawText(gap + buttonwidth / kbw / 2, gap + buttonwidth / kbw + gap + HEIGHT / 3 / 2, width / 40, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"学\n术\n英\n语", 1);
    ImGui::SetCursorPos({ gap ,gap + buttonwidth / kbw + gap });
    if (PaintBoard(u8"学术英语", ImVec2(buttonwidth / kbw, HEIGHT / 3))) {
        list = 0;
        animeManager.addAnime(&dy, dy, gap + buttonwidth / kbw + gap, 300, InOutSine);

        for (int i = 0; i < dx.size(); i++) {
            animeManager.unBind(&dx[i]);//解绑，防止清空vector的时候，anime还在更新不存在的值
        }
        dx.clear();
        rearrange = 1;

        problemSets.selected.clear();
    }

    drawText(gap + buttonwidth / kbw / 2, gap + buttonwidth / kbw + gap + HEIGHT / 3 / 2 + HEIGHT / 3, width / 40, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"收\n藏\n夹", 1);
    ImGui::SetCursorPos({ gap ,gap + buttonwidth / kbw + gap + HEIGHT / 3 });
    if (PaintBoard(u8"收藏夹", ImVec2(buttonwidth / kbw, HEIGHT / 3))) {
        list = 1;
        animeManager.addAnime(&dy, dy, gap + buttonwidth / kbw + gap + HEIGHT / 3, 300, InOutSine);

        for (int i = 0; i < dx.size(); i++) {
            animeManager.unBind(&dx[i]);
        }
        dx.clear();
        rearrange = 1;

        problemSets.selected.clear();
    }

    drawText(gap + buttonwidth / kbw / 2, gap + buttonwidth / kbw + gap + HEIGHT / 3 / 2 + HEIGHT / 3 * 2, width / 40, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), u8"所\n有\n题\n单", 1);
    ImGui::SetCursorPos({ gap ,gap + buttonwidth / kbw + gap + HEIGHT / 3 * 2 });
    if (PaintBoard(u8"所有题单", ImVec2(buttonwidth / kbw, HEIGHT / 3))) {
        list = 2;
        animeManager.addAnime(&dy, dy, gap + buttonwidth / kbw + gap + HEIGHT / 3 * 2, 300, InOutSine);

        for (int i = 0; i < dx.size(); i++) {
            animeManager.unBind(&dx[i]);
        }
        dx.clear();
        rearrange = 1;

        problemSets.selected.clear();
    }

    bool favoritesChanged = false;
    ImGui::SetCursorPos({ gap + buttonwidth / kbw + gap, gap + buttonwidth / kbw + gap});
    ImGui::BeginChild(
        u8"题单选择",
        ImVec2(WIDTH, HEIGHT),
        1,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings// |
        //ImGuiWindowFlags_NoMove
    );{
        int cnt = 0;//万一有人把题库删了
            for (const auto& key : OrderedProblemSetNames(problemSets.sets)) {
                auto& name_ = problemSets.sets[key];
                const auto title = name_["name"].asString();
                auto& favorites = userInfo["preferences"]["favoriteProblemSets"];
                bool favorite = false;
                if (favorites.isArray()) for (const auto& item : favorites)
                    if (item.isString() && item.asString() == title) { favorite = true; break; }
                if ((list == 0 && title.find(u8"学术英语") == std::string::npos) ||
                    (list == 1 && !favorite)) continue;
                if (dx.size() <= cnt) {
                    dx.push_back(-(width / 3 - 2 * gap) - gap);
                }
                std::string name = encode::UTF8_To_String(name_["name"].asString());
                float WIDTH = width / 3 - 2 * gap;
                float HEIGHT = buttonwidth / kbw * 1.33;
                ImGui::SetCursorPos({ gap + dx[cnt], gap + (HEIGHT + gap) * cnt});//    设置绘制指针

                ImGui::PushStyleColor(ImGuiCol_Button, problemSets.selected.count(name) ? ImVec4(162 / 255.0, 21 / 255.0, 44 / 255.0, 1.0f) : Color[ImGuiCol_Button]);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, problemSets.selected.count(name) ? ImVec4(162 / 255.0, 11 / 255.0, 44 / 255.0, 1.0f) : Color[ImGuiCol_ButtonHovered]);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, problemSets.selected.count(name) ? ImVec4(162 / 255.0, 16 / 255.0, 44 / 255.0, 1.0f) : Color[ImGuiCol_ButtonActive]);
                if (ImGui::Button(encode::string_To_UTF8(name).c_str(), { WIDTH ,HEIGHT }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
                {
                    if (problemSets.selected.count(name)) {
                        problemSets.selected.erase(name);
                    }
                    else {
                        problemSets.selected.insert(name);
                    }
                    //Tab = TAB::Main;
                }
                ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
                if (!ButtonLock && subWindow.empty() && ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem(list == 1 ? u8"移出收藏夹" : u8"加入收藏夹", nullptr, false,
                                        list == 1 || !favorite)) {
                        Json::Value updated(Json::arrayValue);
                        if (favorites.isArray()) for (const auto& item : favorites)
                            if (item.isString() && item.asString() != title) updated.append(item);
                        if (list != 1) updated.append(title);
                        favorites = updated;
                        SaveUserData();
                        if (list == 1) problemSets.selected.erase(name);
                        favoritesChanged = true;
                    }
                    ImGui::EndPopup();
                }
                cnt++;
            }
        if (rearrange) {
            rearrange = 0;
            for (int i = 0; i < cnt; i++) {
                animeManager.addAnime(&dx[i], dx[i], 0, 500, OutBack, 70 * i);
            }
        }

        if (ButtonLock) drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
    }
    ImGui::EndChild();
    if (favoritesChanged) {
        for (auto& offset : dx) animeManager.unBind(&offset);
        dx.clear();
        rearrange = true;
    }

    //随机跳题，可重复，不可重复，结算界面，错题记录

    //随机抽题
    ImGui::SetCursorPos({ width * 2 / 3 - buttonwidth / 2, height / 2 });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"开始抽题", { buttonwidth, buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        if (problemSets.selected.size() == 0) {
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
                    u8"Unaccepted"
                );
                ImGui::Indent(width / 30);
                ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
                ImGui::Text(u8"请至少选择一组题单");

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
            for (const auto& key : problemSets.selected) {
                const auto& set = problemSets.sets[key];
                for (Json::ArrayIndex i=0; i<set["problems"].size(); ++i) {
                    auto q = set["problems"][i];
                    q["SourceSet"] = set["name"]; q["SourceIndex"] = static_cast<int>(i+1);
                    pool.push_back(q);
                }
            }
            if (exercise.start(pool, false, myrand)) TabStack.push(TAB::Exercise);
            else popWindow(u8"提示", u8"所选题单中没有有效题目。");

        }
    }
    ImGui::PopStyleColor();
    //是否允许题目重复
    //启用判题器
    

    //抽一组题目做
    //

    //drawText_autoNewLine(
    //    0.1 * width, 0.5 * height, width / 50, 
    //    ImVec4(255, 255, 255, 255), 
    //    "The topic of dynamics is based on the ___ that forces create movements which can be described as spatial changes within an all-controlling time domain.", 
    //    width * 0.9, //这个参数实际上是“右边界”
    //    " ", 
    //    0
    //);

    ImGui::SetCursorPos({ gap, gap });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"Back", { buttonwidth / 4, buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        problemSets.selected.clear();
        //ButtonLock = 1;
        TabStack.pop();
        //animeManager.addAnime(&animeTimeL, 0, width, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::Main;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, width, FrameDelay, InSine);
        //    });
    }
    ImGui::PopStyleColor();
}


}
