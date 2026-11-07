#pragma once
#include "json/json.h"
#include <vector>
#include <random>
#include <chrono>
#include "QuestionModel.h"

namespace scut_ui {
enum Statu { NoChoose, Chosen, Accepted, WrongAnswer };

// Exercise data and grading are independent of ImGui and Win32.
struct ExerciseSession {
    using Clock = std::chrono::steady_clock;
    Json::Value questions = Json::arrayValue;
    int acceptedCount = 0;
    int currentIndex = 0;
    bool wrongAnswerPractice = false;
    bool allowRepeated = false;
    bool randomizeChoices = false;
    bool autoJudge = false;
    bool autoAdvance = false;
    bool collectWrongAnswers = true;
    bool removeCorrectWrongAnswers = false;
    std::vector<Json::Value> visited, unvisited;
    std::vector<std::vector<int>> choiceOrder;

    // skip != 0 leaves the last unanswered question ungraded.
    void judge(Json::Value& userInfo, int skip = 0);
    void gradeQuestion(int index, Json::Value& userInfo);
    void submitCurrent(Json::Value& userInfo);
    bool hasPendingAnswers() const;
    int submitSelected(Json::Value& userInfo);
    void setChoiceRandomization(bool enabled, std::mt19937& random);
    int displayedChoiceIndex(int questionIndex, int displayIndex) const;
    void finish(Json::Value& userInfo, Clock::time_point now = Clock::now());
    double elapsedSeconds(Clock::time_point now = Clock::now()) const;
    double correctPerSecond() const {
        const double seconds = elapsedSeconds();
        return seconds > 0.0 ? acceptedCount / seconds : 0.0;
    }
    int gradedCount() const {
        int count = 0;
        for (const auto& question : questions) if (question["Locked"].asBool()) ++count;
        return count;
    }
    bool start(std::vector<Json::Value> pool, bool wrongPractice, std::mt19937& random,
               Clock::time_point now = Clock::now());
    bool next(std::mt19937& random);
    void setWrongAnswerPolicy(int questionIndex, int policy, Json::Value& userInfo);
private:
    Clock::time_point startedAt{};
    double finishedSeconds = 0.0;
    bool timerRunning = false;
};
}
