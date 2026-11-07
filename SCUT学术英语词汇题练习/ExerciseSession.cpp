#include "ExerciseSession.h"
namespace scut_ui {
double ExerciseSession::elapsedSeconds(Clock::time_point now) const {
    return timerRunning ? (std::max)(0.0, std::chrono::duration<double>(now - startedAt).count())
                        : finishedSeconds;
}
void ExerciseSession::finish(Json::Value& user, Clock::time_point now) {
    finishedSeconds = elapsedSeconds(now);
    timerRunning = false;
    judge(user, 1);
}
void ExerciseSession::gradeQuestion(int index, Json::Value& user) {
    if (index < 0 || index >= static_cast<int>(questions.size())) return;
    auto& q = questions[index];
    if (q["Locked"].asBool()) return;
    q["Locked"] = true;
    const bool correct = ValidQuestion(q) && HasSelection(q) && SelectedAnswers(q) == CorrectAnswers(q);
    q["statu"] = correct ? Accepted : WrongAnswer;
    if (correct) ++acceptedCount;
    const int policy = q.get("WrongAnswerPolicy", 0).asInt();
    if (policy == 2) RemoveWrongAnswer(user, q);
    else if (policy == 1) AddWrongAnswer(user, q);
    else if (correct && wrongAnswerPractice && removeCorrectWrongAnswers) RemoveWrongAnswer(user, q);
    else if (!correct && collectWrongAnswers) AddWrongAnswer(user, q);
}
void ExerciseSession::judge(Json::Value& user, int skip) {
    for (int i = 0; i < static_cast<int>(questions.size()); ++i) {
        if (skip && i + 1 == static_cast<int>(questions.size()) && !HasSelection(questions[i])) continue;
        gradeQuestion(i, user);
    }
}
void ExerciseSession::submitCurrent(Json::Value& user) {
    if (currentIndex >= 0 && currentIndex < static_cast<int>(questions.size()) && HasSelection(questions[currentIndex]))
        gradeQuestion(currentIndex, user);
}
bool ExerciseSession::hasPendingAnswers() const {
    for (const auto& q : questions)
        if (!q["Locked"].asBool() && HasSelection(q)) return true;
    return false;
}
int ExerciseSession::submitSelected(Json::Value& user) {
    int submitted = 0;
    for (int i = 0; i < static_cast<int>(questions.size()); ++i) {
        if (!questions[i]["Locked"].asBool() && HasSelection(questions[i])) {
            gradeQuestion(i, user);
            ++submitted;
        }
    }
    return submitted;
}
void ExerciseSession::setChoiceRandomization(bool enabled, std::mt19937& random) {
    randomizeChoices = enabled;
    if (currentIndex < 0 || currentIndex >= static_cast<int>(questions.size())) return;
    auto& order = choiceOrder[currentIndex];
    for (int i = 0; i < static_cast<int>(order.size()); ++i) order[i] = i;
    if (enabled) std::shuffle(order.begin(), order.end(), random);
}
int ExerciseSession::displayedChoiceIndex(int questionIndex, int displayIndex) const {
    if (questionIndex < 0 || questionIndex >= static_cast<int>(choiceOrder.size())
        || displayIndex < 0 || displayIndex >= static_cast<int>(choiceOrder[questionIndex].size())) return displayIndex;
    return choiceOrder[questionIndex][displayIndex];
}
bool ExerciseSession::start(std::vector<Json::Value> pool, bool wrongPractice, std::mt19937& random,
                            Clock::time_point now) {
    startedAt = now;
    finishedSeconds = 0.0;
    timerRunning = false;
    questions = Json::arrayValue;
    visited.clear();
    unvisited.clear();
    choiceOrder.clear();
    currentIndex = -1;
    acceptedCount = 0;
    wrongAnswerPractice = wrongPractice;
    for (auto q : pool) if (ValidQuestion(q)) {
        ResetAttempt(q);
        q.removeMember("WrongAnswerPolicy");
        unvisited.push_back(q);
    }
    std::shuffle(unvisited.begin(), unvisited.end(), random);
    timerRunning = next(random);
    return timerRunning;
}
bool ExerciseSession::next(std::mt19937& random) {
    if (currentIndex + 1 < static_cast<int>(questions.size())) { ++currentIndex; return true; }
    if (unvisited.empty() && (!allowRepeated || visited.empty())) return false;
    Json::Value q;
    if (allowRepeated) {
        std::uniform_int_distribution<size_t> pick(0, visited.size() + unvisited.size() - 1);
        const size_t index = pick(random);
        if (index < visited.size()) q = visited[index];
        else {
            const size_t i = index - visited.size();
            q = unvisited[i];
            visited.push_back(q);
            unvisited.erase(unvisited.begin() + i);
        }
    } else {
        q = unvisited.back();
        unvisited.pop_back();
        visited.push_back(q);
    }
    ResetAttempt(q);
    questions.append(q);
    std::vector<int> order(q["choices"].size());
    for (int i = 0; i < static_cast<int>(order.size()); ++i) order[i] = i;
    if (randomizeChoices) std::shuffle(order.begin(), order.end(), random);
    choiceOrder.push_back(std::move(order));
    currentIndex = static_cast<int>(questions.size()) - 1;
    return true;
}
void ExerciseSession::setWrongAnswerPolicy(int index, int policy, Json::Value& user) {
    if (index < 0 || index >= static_cast<int>(questions.size())) return;
    const auto key = QuestionKey(questions[index]);
    for (auto& q : questions) if (QuestionKey(q) == key) q["WrongAnswerPolicy"] = policy;
    for (auto& q : visited) if (QuestionKey(q) == key) q["WrongAnswerPolicy"] = policy;
    for (auto& q : unvisited) if (QuestionKey(q) == key) q["WrongAnswerPolicy"] = policy;
    if (policy == 1) AddWrongAnswer(user, questions[index]);
    else if (policy == 2) RemoveWrongAnswer(user, questions[index]);
    else if (questions[index]["Locked"].asBool() && questions[index]["statu"].asInt() == WrongAnswer
             && collectWrongAnswers) AddWrongAnswer(user, questions[index]);
}
}
