#pragma once
#include "json/json.h"
#include <algorithm>
#include <cctype>
#include <set>
#include <string>
#include <vector>
#include <chrono>

namespace scut_ui {
inline std::string CanonicalAnswers(const std::string& text) {
    std::set<char> letters;
    for (unsigned char c : text) {
        if (c == ',' || c == ';' || c == '|' || std::isspace(c)) continue;
        c = static_cast<unsigned char>(std::toupper(c));
        if (c < 'A' || c > 'Z') return "";
        letters.insert(static_cast<char>(c));
    }
    return {letters.begin(), letters.end()};
}
inline std::string CorrectAnswers(const Json::Value& q) {
    return q["answer"].isString() ? CanonicalAnswers(q["answer"].asString()) : "";
}
inline bool IsMultiple(const Json::Value& q) {
    return (q["type"].isString() && q["type"].asString() == "multiple") || CorrectAnswers(q).size() > 1;
}
inline bool ValidQuestion(const Json::Value& q) {
    if (!q.isObject() || !q["problem"].isString() || q["problem"].asString().empty()
        || !q["choices"].isArray() || q["choices"].size() < 2 || q["choices"].size() > 26) return false;
    if (q.isMember("type") && (!q["type"].isString()
        || (q["type"].asString() != "single" && q["type"].asString() != "multiple"))) return false;
    if (q.get("type", "").asString() == "single" && CorrectAnswers(q).size() > 1) return false;
    for (const auto& choice : q["choices"]) if (!choice.isString()) return false;
    const auto answers = CorrectAnswers(q);
    return !answers.empty() && answers.back() - 'A' < static_cast<int>(q["choices"].size());
}
inline std::string SelectedAnswers(const Json::Value& q) {
    if (q.isMember("Selected")) return CanonicalAnswers(q["Selected"].asString());
    const int choice = q.get("Choose", -1).asInt();
    return choice >= 0 && choice < static_cast<int>(q["choices"].size())
        ? std::string(1, static_cast<char>('A' + choice)) : "";
}
inline bool HasSelection(const Json::Value& q) { return !SelectedAnswers(q).empty(); }
inline bool ChoiceSelected(const Json::Value& q, int i) {
    return SelectedAnswers(q).find(static_cast<char>('A' + i)) != std::string::npos;
}
inline bool ChoiceCorrect(const Json::Value& q, int i) {
    return CorrectAnswers(q).find(static_cast<char>('A' + i)) != std::string::npos;
}
inline void ResetAttempt(Json::Value& q) {
    q["Selected"] = "";
    q["Choose"] = -1;
    q["Locked"] = false;
    q["statu"] = 0;
    q.removeMember("Statu");
}
inline void ToggleChoice(Json::Value& q, int i) {
    if (q.get("Locked", false).asBool() || i < 0 || i >= static_cast<int>(q["choices"].size())) return;
    std::string selected = SelectedAnswers(q);
    const char letter = static_cast<char>('A' + i);
    if (!IsMultiple(q)) selected = std::string(1, letter);
    else if (selected.find(letter) == std::string::npos) selected += letter;
    else selected.erase(selected.find(letter), 1);
    q["Selected"] = CanonicalAnswers(selected);
    q["Choose"] = selected.empty() ? -1 : selected.front() - 'A';
    q["statu"] = selected.empty() ? 0 : 1;
}
inline std::string QuestionKey(const Json::Value& q) {
    Json::Value key;
    key["problem"] = q["problem"];
    key["choices"] = q["choices"];
    key["answer"] = CorrectAnswers(q);
    key["multiple"] = IsMultiple(q);
    return Json::writeString(Json::StreamWriterBuilder(), key);
}
inline bool ContainsWrongAnswer(const Json::Value& user, const Json::Value& q) {
    const auto key = QuestionKey(q);
    if (!user["WA"].isArray()) return false;
    for (const auto& saved : user["WA"]) if (QuestionKey(saved) == key) return true;
    return false;
}
inline void RemoveWrongAnswer(Json::Value& user, const Json::Value& q) {
    const auto key = QuestionKey(q);
    Json::Value kept(Json::arrayValue);
    if (user["WA"].isArray()) for (const auto& saved : user["WA"])
        if (QuestionKey(saved) != key) kept.append(saved);
    user["WA"] = kept;
}
inline void SortWrongAnswers(Json::Value& user) {
    std::vector<Json::Value> sorted;
    for (const auto& q : user["WA"]) sorted.push_back(q);
    std::stable_sort(sorted.begin(), sorted.end(), [](const Json::Value& a, const Json::Value& b) {
        return a.get("LastWrongAt", Json::Int64(0)).asInt64() < b.get("LastWrongAt", Json::Int64(0)).asInt64();
    });
    user["WA"] = Json::arrayValue;
    for (const auto& q : sorted) user["WA"].append(q);
}
inline void StoreWrongAnswer(Json::Value& user, const Json::Value& q, bool updateTime) {
    auto saved = q;
    RemoveWrongAnswer(user, saved);
    if (!user["WA"].isArray()) user["WA"] = Json::arrayValue;
    if (updateTime) {
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        const Json::Int64 time = (std::max)(static_cast<Json::Int64>(now), user.get("WrongAnswerClock",Json::Int64(0)).asInt64()+1);
        saved["LastWrongAt"] = time;
        user["WrongAnswerClock"] = time;
    }
    ResetAttempt(saved);
    saved.removeMember("WrongAnswerPolicy");
    user["WA"].append(saved);
    SortWrongAnswers(user);
}
inline void AddWrongAnswer(Json::Value& user, const Json::Value& q) { StoreWrongAnswer(user,q,true); }
inline void RestoreWrongAnswer(Json::Value& user, const Json::Value& q) { StoreWrongAnswer(user,q,false); }
inline void NormalizeWrongAnswers(Json::Value& user) {
    auto old = user["WA"];
    user["WA"] = Json::arrayValue;
    Json::Int64 legacyOrder=0;
    if (old.isArray()) for (auto q : old) if (q.isObject()) {
        if (!q["LastWrongAt"].isInt64()) q["LastWrongAt"]=++legacyOrder;
        user["WrongAnswerClock"]=(std::max)(user.get("WrongAnswerClock",Json::Int64(0)).asInt64(),q["LastWrongAt"].asInt64());
        RestoreWrongAnswer(user,q);
    }
}

// Compare numeric runs without integer conversion (also supports very long indices).
inline bool NaturalLess(const std::string& a, const std::string& b) {
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (std::isdigit(static_cast<unsigned char>(a[i])) && std::isdigit(static_cast<unsigned char>(b[j]))) {
            size_t ai = i, bj = j;
            while (i < a.size() && std::isdigit(static_cast<unsigned char>(a[i]))) ++i;
            while (j < b.size() && std::isdigit(static_cast<unsigned char>(b[j]))) ++j;
            auto x = a.substr(ai, i - ai), y = b.substr(bj, j - bj);
            auto nx = x.find_first_not_of('0'), ny = y.find_first_not_of('0');
            auto vx = nx == std::string::npos ? "0" : x.substr(nx);
            auto vy = ny == std::string::npos ? "0" : y.substr(ny);
            if (vx.size() != vy.size()) return vx.size() < vy.size();
            if (vx != vy) return vx < vy;
            if (x.size() != y.size()) return x.size() < y.size();
        } else {
            auto x = static_cast<unsigned char>(a[i++]), y = static_cast<unsigned char>(b[j++]);
            if (x != y) return x < y;
        }
    }
    return a.size() < b.size();
}
inline std::vector<std::string> OrderedProblemSetNames(const Json::Value& sets) {
    auto names = sets.getMemberNames();
    std::sort(names.begin(), names.end(), [&](const std::string& a, const std::string& b) {
        const int x = sets[a].get("order", 1000).asInt(), y = sets[b].get("order", 1000).asInt();
        return x != y ? x < y : NaturalLess(a, b);
    });
    return names;
}
inline bool MoveProblemSet(Json::Value& sets, const std::string& source, const std::string& target, bool after) {
    auto names=OrderedProblemSetNames(sets);
    const auto original=names;
    auto from=std::find(names.begin(),names.end(),source);
    if (from==names.end() || source==target || !sets.isMember(target)) return false;
    names.erase(from);
    auto to=std::find(names.begin(),names.end(),target);
    names.insert(after ? to+1 : to,source);
    if (names==original) return false;
    for (size_t i=0;i<names.size();++i) sets[names[i]]["order"]=static_cast<int>(i);
    return true;
}
}
