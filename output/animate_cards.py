from pathlib import Path
p=Path('SCUT学术英语词汇题练习/UIProblemSetManager.cpp'); s=p.read_text(encoding='utf-8-sig'); a=s.index('    int row = 0;'); b=s.index('    ImGui::SetCursorPos({width/2-buttonwidth/2,gap+row*(HEIGHT+gap)});',a)
s=s[:a]+r'''    auto names=OrderedProblemSetNames(problemSets.sets);
    const int row=static_cast<int>(names.size());
    const float pitch=HEIGHT+gap;
    static std::string dragged;
    static ImVec2 grabOffset, draggedPosition;
    static int insertion=0;
    static std::map<std::string,ImVec2> positions;
    static float previousPitch=0;
    const ImVec2 origin=ImGui::GetWindowPos();
    auto& io=ImGui::GetIO();
    bool dragging=!dragged.empty();
    if (dragging && !problemSets.sets.isMember(dragged)) { dragged.clear(); dragging=false; }
    bool cancelled=false;
    if (dragging && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        dragged.clear(); dragging=false; cancelled=true;
    }
    if (dragging) {
        // Keep the exact mouse-to-card offset, including when the list scrolls.
        draggedPosition=io.MousePos-grabOffset;
        const float centerY=draggedPosition.y+HEIGHT*0.5f-origin.y+ImGui::GetScrollY();
        insertion=static_cast<int>((centerY-gap)/pitch+0.5f);
        insertion=(std::max)(0,(std::min)(row-1,insertion));
        const bool overList=io.MousePos.x>=origin.x && io.MousePos.x<=origin.x+width
            && io.MousePos.y>=origin.y && io.MousePos.y<=origin.y+height;
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || !io.MouseDown[0]) {
            auto preview=names;
            preview.erase(std::find(preview.begin(),preview.end(),dragged));
            preview.insert(preview.begin()+insertion,dragged);
            if (overList && preview!=names) {
                for (int i=0;i<row;++i) {
                    problemSets.sets[preview[i]]["order"]=i;
                    SaveJsonToFile(problemSets.sets[preview[i]],problemSets.nameToPath[preview[i]]);
                }
                names=preview;
            }
            // Ease the released card from its final mouse position into its slot.
            positions[dragged]=ImVec2(draggedPosition.x-origin.x,
                draggedPosition.y-origin.y+ImGui::GetScrollY());
            dragged.clear(); dragging=false;
        } else if (overList) {
            const float localY=io.MousePos.y-origin.y;
            const float speed=height*io.DeltaTime;
            if (localY<height*0.12f) ImGui::SetScrollY(ImGui::GetScrollY()-speed);
            else if (localY>height*0.88f) ImGui::SetScrollY(ImGui::GetScrollY()+speed);
        }
    }
    auto preview=names;
    if (dragging) {
        preview.erase(std::find(preview.begin(),preview.end(),dragged));
        preview.insert(preview.begin()+insertion,dragged);
        const ImVec2 slot(origin.x+width/6,origin.y+gap+insertion*pitch-ImGui::GetScrollY());
        ImGui::GetWindowDrawList()->AddRect(slot,slot+ImVec2(WIDTH,HEIGHT),IM_COL32(192,100,110,160),8,0,2);
    }
    // Time-based exponential ease-out: no frame-rate dependence and no overshoot.
    const float easing=1.0f-std::exp(-18.0f*(std::min)(io.DeltaTime,0.1f));
    for (int i=0;i<row;++i) {
        const auto& key=preview[i];
        const ImVec2 target(width/6,gap+i*pitch);
        auto found=positions.find(key);
        if (found==positions.end()) positions[key]=target;
        else if (key!=dragged) {
            if (previousPitch!=pitch) found->second=target;
            else {
                found->second+=(target-found->second)*easing;
                if (ImLengthSqr(found->second-target)<0.25f) found->second=target;
            }
        }
    }
    previousPitch=pitch;
    for (auto it=positions.begin();it!=positions.end();) {
        if (!problemSets.sets.isMember(it->first)) it=positions.erase(it); else ++it;
    }
    auto renderOrder=names;
    if (dragging) {
        renderOrder.erase(std::find(renderOrder.begin(),renderOrder.end(),dragged));
        renderOrder.push_back(dragged); // Last child renders above the other cards.
    }
    for (const auto& key:renderOrder) {
        auto& set=problemSets.sets[key];
        const bool floating=dragging && key==dragged;
        if (floating) ImGui::SetCursorScreenPos(draggedPosition);
        else ImGui::SetCursorPos(positions[key]);
        if (floating) {
            const auto pos=ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(pos+ImVec2(5,7),pos+ImVec2(WIDTH+5,HEIGHT+7),IM_COL32(0,0,0,110),8);
            ImGui::PushStyleColor(ImGuiCol_Border,ImVec4(0.9f,0.6f,0.65f,1));
        }
        ImGui::BeginChild(key.c_str(), {WIDTH,HEIGHT}, true,ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoScrollbar);
        drawText(gap,gap,width/35,ImVec4(1,1,1,1),set["name"].asString(),false);
        drawText(gap,gap+width/35+gap,width/40,ImVec4(1,1,1,1),
            u8"题数："+std::to_string(set["problems"].size()),false);
        ImGui::SetCursorPos({gap,gap});
        ImGui::InvisibleButton("card_drag",{WIDTH-2*gap,HEIGHT-buttonwidth/kbw-3*gap});
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !dragging && !cancelled) {
            dragged=key;
            // MouseClickedPos preserves displacement from the initial press, not the drag threshold.
            grabOffset=io.MouseClickedPos[0]-ImGui::GetWindowPos();
            draggedPosition=io.MousePos-grabOffset;
            insertion=static_cast<int>(std::find(names.begin(),names.end(),key)-names.begin());
        }
        if (ImGui::IsItemHovered() && !dragging) ImGui::SetTooltip(u8"按住卡片拖动，其他卡片会自动让位；Esc 取消");
        ImGui::BeginDisabled(!dragged.empty());
        ImGui::SetCursorPos({WIDTH-gap-buttonwidth-gap,HEIGHT-gap-buttonwidth/kbw-gap/2});
        if (ImGui::Button(u8"编辑", {buttonwidth/2,buttonwidth/kbw})) { editor.load(set); TabStack.push(Editor); }
        ImGui::SetCursorPos({WIDTH-gap-buttonwidth/2,HEIGHT-gap-buttonwidth/kbw-gap/2});
        if (ImGui::Button(u8"删除", {buttonwidth/2,buttonwidth/kbw})) pendingDelete=key;
        ImGui::EndDisabled();
        ImGui::EndChild();
        if (floating) ImGui::PopStyleColor();
    }
'''+s[b:]
s=s.replace('#include "UIInternal.h"','#include "UIInternal.h"\n#include <cmath>'); p.write_text(s,encoding='utf-8-sig')
