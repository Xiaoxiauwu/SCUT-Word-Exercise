#include "UIInternal.h"

namespace scut_ui {
namespace {
const std::string AboutMarkdown = u8R"md(# 关于

**SCUT 学术英语词汇练习 · Version 1.0.3**

By XiaoXia

本项目基于 ImGui，用于通过题库练习巩固词汇与考试知识，支持自建题单、随机练习和错题复习。

[项目主页](https://github.com/Xiaoxiauwu/SCUT-Word-Exercise) · [问题反馈](https://github.com/Xiaoxiauwu/SCUT-Word-Exercise/issues)

---

## 最近更新

### V1.0.3

### 练习与错题

- 增加了对多选题的支持，以及抽题时选项随机排列。
- 支持查看、删除错题，手动收录或排除本题，以及答对后移出错题集。
- 修复提前结算时将最新未作答题误收录为错题的 bug。
- 增加了练习计时，结算显示总用时、答对题数和 p/s（答对题数 ÷ 总用时秒数）。

### 题单与编辑

- 支持拖拽排序题单顺序。
- 增加题单收藏夹。

### Markdown 与公式

- 增加了对常用 Markdown 语法的支持，如标题、强调、删除线、引用、列表、分隔线、代码块和表格。
- 支持本地图片及普通超链接。
- 支持 `$...$` 和 `$$...$$` 的公式编写。

### 窗口与界面

- 增加了对窗口大小调整的支持。

---

## 历史版本

### V1.0.2

1. 修复 Unit 7 第 52 题的答案错误（应选择 A 而不是 B）。
2. 将绘制帧率与屏幕刷新率锁定，降低 GPU 资源占用。
3. 增加“自动判题 + 自动下一题”功能。

### v1.0.1

1. 修复错题练习提前结算时错题数量异常减少的问题。
2. 改良“一键判题”的逻辑。
3. 更新题单管理功能，支持自建题单。
4. 更新学术英语 Unit 6–10 的 AWL 词汇题题单。

### v1.0.0beta

1. 附带学术英语 Unit 1–5 的 AWL 词汇题题单。
2. 支持任意组合题单进行随机抽题。
3. 增加错题练习。

学术英语题单资料来源：通用学术英语短语词汇教程，2019 年 9 月第 1 版第 1 次印刷。

---

欢迎通过项目的问题反馈页面报告 BUG 或提出建议。
)md";
}

void RenderAbout() {
    ImGui::BeginChild(u8"关于 ", {width - gap, height}, false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SetCursorPos({gap, gap});
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
    if (ImGui::Button(u8"返回", {buttonwidth * 2 / kbw, buttonwidth / kbw}) &&
        !ButtonLock && subWindow.empty()) {
        TabStack.pop();
        animeManager.addAnime(&switchFrameV, 255, 0, FrameDelay, OutSine, 0);
    }
    ImGui::PopStyleColor();

    // Keep the return control fixed while only the Markdown document scrolls.
    const float top = buttonwidth / kbw + 3 * gap;
    ImGui::SetCursorPos({width / 8, top});
    ImGui::BeginChild("about_markdown", {width * 0.75f, height - top - gap}, false,
        ImGuiWindowFlags_AlwaysVerticalScrollbar);
    const float contentWidth = (std::max)(1.0f, ImGui::GetContentRegionAvail().x - 2 * gap);
    const float fontSize = width / 50;
    const float contentHeight = MeasureMarkdown(AboutMarkdown, contentWidth, fontSize);
    RenderMarkdown(AboutMarkdown, ImVec2(gap, gap), contentWidth, fontSize);
    // Draw-list rendering needs an explicit item to establish the scroll extent.
    ImGui::SetCursorPos({gap, gap + contentHeight});
    ImGui::Dummy(ImVec2(contentWidth, 3 * gap));
    ImGui::EndChild();
    ImGui::EndChild();
}
} // namespace scut_ui
