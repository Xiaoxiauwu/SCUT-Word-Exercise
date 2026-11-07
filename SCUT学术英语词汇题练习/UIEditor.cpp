#include "UIInternal.h"
namespace scut_ui {
void RenderEditor() {
    ImGui::BeginChild(u8"题单编辑", {width-gap,height},false,ImGuiWindowFlags_AlwaysVerticalScrollbar);
    static std::string content, notice;
    static int editQuestion=0, editChoice=-1;
    float offsetY=gap+buttonwidth/kbw+gap;
    const float WIDTH=width*2/3-gap;
    std::string resourceDirectory=".";
    const auto setName=encode::UTF8_To_String(editor.data()["name"].asString());
    if (const auto source=problemSets.nameToPath.find(setName); source!=problemSets.nameToPath.end())
        resourceDirectory=std::filesystem::path(source->second).parent_path().string();
    bool openTextEditor=false;
    for (int i=0;i<editor.getSize();++i) {
        auto& problem=editor.getProblem(i);
        const std::string questionText=std::to_string(i+1)+". "+problem["problem"].asString();
        const float questionHeight=MeasureMarkdown(questionText,WIDTH-2*gap,width/50,resourceDirectory);
        std::vector<std::string> labels;
        bool singleColumn=false;
        const float iconSize=buttonwidth/kbw*0.7f;
        const float groupWidth=buttonwidth;
        const float buttonTextWidth=groupWidth-iconSize-gap-2*ImGui::GetStyle().FramePadding.x;
        for (int j=0;j<static_cast<int>(problem["choices"].size());++j) {
            labels.push_back(std::string(1,static_cast<char>('A'+j))+". "+problem["choices"][j].asString());
            if (ImGui::CalcTextSize(labels.back().c_str()).x>buttonTextWidth) singleColumn=true;
        }
        const float optionGroupWidth=singleColumn ? WIDTH-2*gap : groupWidth;
        const float optionButtonWidth=optionGroupWidth-iconSize-gap;
        const float wrappedTextWidth=(std::max)(1.0f,optionButtonWidth-2*ImGui::GetStyle().FramePadding.x);
        const float buttonHeight=buttonwidth/kbw;
        std::vector<float> optionHeights;
        std::vector<float> optionTextHeights;
        std::vector<float> columnHeights(singleColumn?1:2,0.0f);
        for (int j=0;j<static_cast<int>(labels.size());++j) {
            const float textHeight=MeasureMarkdown(labels[j],wrappedTextWidth,width/50,resourceDirectory);
            optionTextHeights.push_back(textHeight);
            const float optionHeight=(std::max)(buttonHeight,textHeight+2*ImGui::GetStyle().FramePadding.y+2*gap);
            optionHeights.push_back(optionHeight);
            columnHeights[singleColumn?0:j%2]+=optionHeight+gap;
        }
        const float contentHeight=*std::max_element(columnHeights.begin(),columnHeights.end());
        const float HEIGHT=questionHeight+3*gap+contentHeight;
        ImGui::SetCursorPos({width/2-WIDTH/2,offsetY});
        ImGui::BeginChild(("question_"+std::to_string(i)).c_str(),{WIDTH,HEIGHT},true);
        RenderMarkdown(questionText,ImVec2(gap,gap),WIDTH-2*gap,width/50,resourceDirectory);
        std::vector<float> columnOffsets(singleColumn?1:2,0.0f);
        for (int j=0;j<static_cast<int>(problem["choices"].size());++j) {
            const int column=singleColumn?0:j%2;
            const float x=singleColumn?gap:(j&1?WIDTH-gap-optionGroupWidth:gap);
            const float y=gap+questionHeight+gap+columnOffsets[column];
            const float optionHeight=optionHeights[j];
            const ImVec4 color=ChoiceCorrect(problem,j) ? ImVec4(0,0.8f,0,0.6f) : ImVec4(44/255.0f,44/255.0f,44/255.0f,150/255.0f);
            ImGui::PushStyleColor(ImGuiCol_Button,color);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,color);
            ImGui::SetCursorPos({x,y});
            const bool answerPressed=ImGui::Button(("##answer_"+std::to_string(j)).c_str(),{optionButtonWidth,optionHeight});
            ImGui::SetItemAllowOverlap();
            ImGui::PopStyleColor(3);
            const bool linkActivated=RenderMarkdown(labels[j],ImVec2(x+ImGui::GetStyle().FramePadding.x,
                y+(optionHeight-optionTextHeights[j])/2),wrappedTextWidth,width/50,resourceDirectory);
            if (answerPressed && !linkActivated) editor.changeAnswer(i,j);
            ImGui::SetCursorPos({x+optionButtonWidth+gap,y+(optionHeight-iconSize)/2});
            if (ImageButton(u8"编辑选项",{iconSize,iconSize},imageLoader->IMG["edit"].image,true,
                {iconSize*0.85f,iconSize*0.85f},static_cast<int>(HASH("edit_"+std::to_string(i)+"_"+std::to_string(j))))) {
                editQuestion=i; editChoice=j; content=problem["choices"][j].asString(); openTextEditor=true;
            }
            columnOffsets[column]+=optionHeight+gap;
        }
        ImGui::EndChild();
        offsetY+=HEIGHT+gap;
        ImGui::SetCursorPos({width/2-WIDTH/2,offsetY});
        ImGui::PushID(i);
        if (ImGui::Button(u8"编辑题目",{buttonwidth/2,buttonwidth/kbw})) {
            editQuestion=i; editChoice=-1; content=problem["problem"].asString(); openTextEditor=true;
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"删除题目",{buttonwidth/2,buttonwidth/kbw})) editor.delProblem(i);
        ImGui::SameLine();
        if (ImGui::Button(u8"题目设置",{buttonwidth/2,buttonwidth/kbw})) ImGui::OpenPopup("question_settings");
        if (ImGui::BeginPopup("question_settings")) {
            bool multiple=IsMultiple(problem);
            if (ImGui::Checkbox(u8"多选题",&multiple)) editor.setMultiple(i,multiple);
            ImGui::TextUnformatted(u8"点击卡片中的选项设置正确答案。多选可选多个。 ");
            if (ImGui::Button(u8"新增选项")) editor.addChoice(i);
            for (int j=0;j<static_cast<int>(problem["choices"].size());++j) {
                ImGui::PushID(j);
                if (ImGui::Button((u8"删除选项 "+std::string(1,static_cast<char>('A'+j))).c_str())) {
                    editor.removeChoice(i,j); ImGui::PopID(); break;
                }
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();
        offsetY+=buttonwidth/kbw+gap*3;
    }
    editor.work();
    ImGui::SetCursorPos({width/2-buttonwidth/2,offsetY});
    if (ImGui::Button(u8"新建题目",{buttonwidth,buttonwidth/kbw})) editor.addNewProblem();
    // Preserve the original fixed Save corner and the scrollable question cards.
    ImGui::SetCursorPos({gap,gap+ImGui::GetScrollY()});
    if (ImGui::Button(u8"Save",{buttonwidth/4,buttonwidth/kbw})) {
        bool valid=true;
        for (int i=0;i<editor.getSize();++i) if (!ValidQuestion(editor.getProblem(i))) {
            valid=false; notice=u8"第 "+std::to_string(i+1)+u8" 题缺少有效题面、选项或正确答案。"; break;
        }
        if (valid) {
            auto data=editor.exportProblems();
            auto key=encode::UTF8_To_String(data["name"].asString());
            SaveJsonToFile(data,problemSets.nameToPath[key]); problemSets.sets[key]=data;
            notice.clear(); TabStack.pop();
        } else ImGui::OpenPopup(u8"保存提示");
    }
    ImGui::SetCursorPos({width-buttonwidth/2-gap,gap+ImGui::GetScrollY()});
    if (ImGui::Button(u8"Back",{buttonwidth/2,buttonwidth/kbw})) TabStack.pop();
    if (openTextEditor) ImGui::OpenPopup(u8"编辑内容");
    const ImVec2 dialogSize(width * 0.7f, height * 0.65f);
    const ImVec2 parentPosition = ImGui::GetWindowPos();
    ImGui::SetNextWindowPos(ImVec2(parentPosition.x + (width - dialogSize.x) / 2,
        parentPosition.y + (height - dialogSize.y) / 2));
    ImGui::SetNextWindowSize(dialogSize);
    if (ImGui::BeginPopupModal(u8"编辑内容",nullptr,ImGuiWindowFlags_NoTitleBar |
                              ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {
        const float padding = width / 30;
        const float footerY = dialogSize.y - buttonwidth / kbw - 2 * gap;
        const float fieldY = width / 35 + 3 * gap;
        drawText(dialogSize.x / 2, gap + width / 70, width / 35, ImVec4(1,1,1,1),
            editChoice < 0 ? u8"编辑题面" : u8"编辑选项");
        ImGui::SetCursorPos({padding,fieldY});
        EditText("##content",content,true,{dialogSize.x - 2 * padding, footerY - fieldY - 2 * gap});
        ImGui::PushStyleColor(ImGuiCol_Button,Color[ImGuiCol_Button]);
        ImGui::SetCursorPos({dialogSize.x / 2 - buttonwidth / 2 - gap,footerY});
        if (ImGui::Button(u8"确定",{buttonwidth/2,buttonwidth/kbw})) {
            if (editChoice<0) editor.changeTitle(editQuestion,content); else editor.changeChoice(editQuestion,editChoice,content);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetCursorPos({dialogSize.x / 2 + gap,footerY});
        if (ImGui::Button(u8"算了",{buttonwidth/2,buttonwidth/kbw})) ImGui::CloseCurrentPopup();
        ImGui::PopStyleColor();
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal(u8"保存提示",nullptr,ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextWrapped("%s",notice.c_str());
        if (ImGui::Button(u8"了解")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    ImGui::EndChild();
}
}
