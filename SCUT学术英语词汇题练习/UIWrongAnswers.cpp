#include "UIInternal.h"

namespace scut_ui {

void RenderWrongAnswers() {
    if (!PageHeader(u8"错题管理")) return;
    static Json::Value removed;
    if (ImGui::Button(u8"练习全部错题")) {
        std::vector<Json::Value> pool;
        for (const auto& q:userInfo["WA"]) pool.push_back(q);
        if (exercise.start(pool,true,myrand)) TabStack.push(Exercise);
    }
    if (!removed.isNull()) {
        ImGui::SameLine();
        if (ImGui::Button(u8"撤销上次删除")) { RestoreWrongAnswer(userInfo,removed); removed=Json::nullValue; SaveUserData(); }
    }
    ImGui::Text(u8"当前共 %u 道错题",userInfo["WA"].size());
    ImGui::BeginChild("wrong_list",ImVec2(0,height-ImGui::GetCursorPosY()-gap*2),true);
    for (Json::ArrayIndex i=0;i<userInfo["WA"].size();++i) {
        const auto q=userInfo["WA"][i];
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::Button(u8"删除此题")) {
            removed=q; RemoveWrongAnswer(userInfo,q); SaveUserData();
            ImGui::PopID(); break;
        }
        ImGui::SameLine(); ImGui::Text(u8"第 %u 题 / %s",i+1,IsMultiple(q)?u8"多选":u8"单选");
        if (q.isMember("SourceSet")) ImGui::TextWrapped(u8"来源：%s / %d",q["SourceSet"].asCString(),q["SourceIndex"].asInt());
        ImGui::TextWrapped("%s",q.get("problem","").asCString());
        for (Json::ArrayIndex j=0;j<q["choices"].size();++j)
            ImGui::TextWrapped("%c. %s",'A'+j,q["choices"][j].asCString());
        ImGui::Text(u8"正确答案：%s",CorrectAnswers(q).c_str());
        if (!ValidQuestion(q)) ImGui::TextColored(ImVec4(1,0.4f,0.4f,1),u8"此旧记录格式无效，无法进入练习。");
        ImGui::Separator(); ImGui::PopID();
    }
    ImGui::EndChild();
}

}
