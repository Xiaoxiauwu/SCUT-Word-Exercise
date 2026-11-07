#pragma once
#include "QuestionModel.h"
#include <map>
namespace scut_ui {
class ProblemSetEditor {
    Json::Value Problems;
    std::map<int,int> deleted;
public:
    int i_ = 0, j_ = 0;
    void loadDefault(std::string name) {
        Problems = Json::objectValue;
        Problems["name"] = name;
        Problems["order"] = 1000;
        Problems["problems"] = Json::arrayValue;
        deleted.clear();
        addNewProblem();
    }
    void load(const Json::Value& value) { Problems = value; deleted.clear(); }
    Json::Value exportProblems() const { return Problems; }
    Json::Value& data() { return Problems; }
    int getSize() const { return static_cast<int>(Problems["problems"].size()); }
    void rename(const std::string& name) { Problems["name"] = name; }
    void addNewProblem() {
        Json::Value q;
        q["problem"] = u8"请输入题目";
        q["type"] = "single";
        q["answer"] = "A";
        for (int i=0; i<4; ++i) q["choices"].append(u8"请输入选项");
        Problems["problems"].append(q);
    }
    Json::Value& getProblem(int i) { return Problems["problems"][i]; }
    void changeTitle(int i, const std::string& text) { getProblem(i)["problem"] = text; }
    void changeChoice_setCursor(int i,int j) { i_=i; j_=j; }
    void changeChoice(int i,int j,const std::string& text) { getProblem(i)["choices"][j] = text; }
    void changeAnswer(int i,int j) {
        auto& q=getProblem(i);
        if(j<0 || j>=static_cast<int>(q["choices"].size())) return;
        const char c=static_cast<char>('A'+j);
        std::string answer=CorrectAnswers(q);
        if(!IsMultiple(q)) answer=std::string(1,c);
        else if(answer.find(c)==std::string::npos) answer+=c;
        else answer.erase(answer.find(c),1);
        q["answer"]=CanonicalAnswers(answer);
    }
    void setMultiple(int i,bool multiple) {
        auto& q=getProblem(i);
        const auto answer=CorrectAnswers(q);
        q["type"]=multiple?"multiple":"single";
        if(!multiple) q["answer"]=answer.empty()?"A":answer.substr(0,1);
    }
    bool addChoice(int i) {
        auto& q=getProblem(i);
        if(q["choices"].size()>=26) return false;
        q["choices"].append(u8"请输入选项"); return true;
    }
    void removeChoice(int i,int j) {
        auto& q=getProblem(i);
        if(q["choices"].size()<=2 || j<0 || j>=static_cast<int>(q["choices"].size())) return;
        std::string answer;
        for(char c:CorrectAnswers(q)) {
            int index=c-'A';
            if(index!=j) answer+=static_cast<char>('A'+index-(index>j));
        }
        Json::Value removed;
        q["choices"].removeIndex(j,&removed);
        q["answer"]=answer;
    }
    void delProblem(int i) { deleted[i]=1; }
    void delProblem_choice(int i,int j) { removeChoice(i,j); }
    void work() {
        for(auto it=deleted.rbegin(); it!=deleted.rend(); ++it) {
            if(getSize()>1 && it->first>=0 && it->first<getSize()) {
                Json::Value removed;
                Problems["problems"].removeIndex(it->first,&removed);
            }
        }
        deleted.clear();
    }
};
}
