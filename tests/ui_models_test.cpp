#include "ExerciseSession.h"
#include "ProblemSetEditor.h"
#include "UILayout.h"
#include <cmath>
#include <cassert>
#include <iostream>

using namespace scut_ui;

Json::Value question(int choice) {
    Json::Value q;
    q["problem"] = "Example " + std::to_string(choice);
    q["choices"].append("first");
    q["choices"].append("second");
    q["answer"] = "A";
    q["Choose"] = choice;
    q["Locked"] = 0;
    return q;
}

int main() {
    // Use explicit monotonic timestamps so timing checks never depend on sleeps.
    ExerciseSession timed;
    Json::Value timedUser;
    std::mt19937 timedRandom(123);
    const auto started = ExerciseSession::Clock::time_point{};
    assert(timed.elapsedSeconds() == 0.0 && timed.correctPerSecond() == 0.0);
    assert(timed.start({question(-1)}, false, timedRandom, started));
    assert(timed.elapsedSeconds(started + std::chrono::seconds(5)) == 5.0);
    ToggleChoice(timed.questions[0], 0);
    timed.finish(timedUser, started + std::chrono::seconds(8));
    assert(timed.acceptedCount == 1 && timed.elapsedSeconds() == 8.0);
    assert(std::abs(timed.correctPerSecond() - 0.125) < 0.000001);
    timed.finish(timedUser, started + std::chrono::seconds(80));
    assert(timed.elapsedSeconds() == 8.0 && timed.acceptedCount == 1);
    assert(timed.start({question(-1)}, true, timedRandom, started));
    assert(timed.elapsedSeconds(started) == 0.0 && timed.acceptedCount == 0);
    timed.finish(timedUser, started);
    assert(timed.correctPerSecond() == 0.0 && timed.gradedCount() == 0);
    assert(!timed.start({}, false, timedRandom, started));
    assert(timed.elapsedSeconds(started + std::chrono::seconds(8)) == 0.0);

    for (auto client : {std::pair<float, float>{640, 440}, {1024, 683},
                        {1920, 1080}, {3840, 2160}, {800, 1200}}) {
        auto layout = FitLayout(client.first, client.second);
        assert(layout.width <= client.first + 0.01f);
        assert(layout.height <= client.second + 0.01f);
        assert(std::abs(layout.width / layout.height - 1.5f) < 0.001f);
    }
    assert(FitLayout(3840, 2160).referenceWidth > 1561);
    const WindowSize current{960, 640};
    const auto horizontal = ConstrainWindowSize(current, {1440, 640});
    assert(horizontal.width == 1440 && horizontal.height == 960);
    const auto vertical = ConstrainWindowSize(current, {960, 960});
    assert(vertical.width == 1440 && vertical.height == 960);
    const auto minimum = ConstrainWindowSize(current, {100, 100});
    assert(minimum.width == 660 && minimum.height == 440);
    for (auto desired : {WindowSize{1001, 673}, {1920, 1080}, {800, 1200}}) {
        const auto constrained = ConstrainWindowSize(current, desired);
        assert(constrained.width * 2 == constrained.height * 3);
    }
    assert(WindowScale(960) == 1.0f);
    assert(WindowScale(1440) == 1.5f);
    assert(WindowScale(1920) == 2.0f);

    ExerciseSession session;
    Json::Value user;
    session.questions.append(question(0));
    session.questions.append(question(1));
    session.questions.append(question(-1));

    session.judge(user, 1);
    assert(session.acceptedCount == 1);
    assert(user["WA"].size() == 1);
    assert(!session.questions[2]["Locked"].asBool());
    assert(user["WA"][0]["Choose"].asInt() == -1);
    assert(!user["WA"][0]["Locked"].asBool());
    assert(user["WA"][0]["statu"].asInt() == NoChoose);

    ExerciseSession batch;
    Json::Value batchUser;
    batch.questions.append(question(0));
    batch.questions.append(question(1));
    batch.questions.append(question(-1));
    assert(batch.hasPendingAnswers());
    assert(batch.submitSelected(batchUser) == 2);
    assert(batch.questions[0]["statu"].asInt() == Accepted);
    assert(batch.questions[1]["statu"].asInt() == WrongAnswer);
    assert(!batch.questions[2]["Locked"].asBool());
    assert(batch.gradedCount() == 2 && batch.acceptedCount == 1);
    assert(!batch.hasPendingAnswers());
    assert(batch.submitSelected(batchUser) == 0);

    // Settlement grades unanswered questions, but never grades locked ones twice.
    session.judge(user);
    assert(session.acceptedCount == 1);
    assert(user["WA"].size() == 2);
    session.judge(user);
    assert(session.acceptedCount == 1);
    assert(user["WA"].size() == 2);

    // A wrong-answer retry can be graded independently of the original session.
    ExerciseSession retry;
    retry.questions.append(user["WA"][0]);
    ToggleChoice(retry.questions[0], 0);
    retry.judge(user);
    assert(retry.acceptedCount == 1);
    assert(session.acceptedCount == 1);

    ProblemSetEditor editor;
    editor.loadDefault("Custom");
    assert(editor.getSize() == 1);
    editor.changeTitle(0, "Updated question");
    editor.changeChoice(0, 1, "Updated option");
    editor.changeAnswer(0, 1);
    editor.addNewProblem();
    editor.delProblem(1);
    editor.work();
    assert(editor.getSize() == 1);
    auto saved = editor.exportProblems();
    assert(saved["name"].asString() == "Custom");
    assert(saved["problems"][0]["problem"].asString() == "Updated question");
    assert(saved["problems"][0]["choices"][1].asString() == "Updated option");
    assert(saved["problems"][0]["answer"].asString() == "B");
    editor.delProblem(0);
    editor.work();
    assert(editor.getSize() == 1);

    // Multi-select grading is set equality: missing and extra choices both fail.
    auto multi=question(-1);
    multi["type"]="multiple";
    multi["choices"].append("third");
    multi["answer"]="CA";
    assert(CanonicalAnswers("c, a;A") == "AC");
    assert(ValidQuestion(multi));
    assert(IsMultiple(multi));
    assert(!HasSelection(multi));
    ToggleChoice(multi,2); ToggleChoice(multi,0);
    assert(SelectedAnswers(multi)=="AC");
    ExerciseSession multiSession;
    Json::Value multiUser;
    multiSession.questions.append(multi);
    multiSession.judge(multiUser);
    assert(multiSession.acceptedCount==1);
    ResetAttempt(multi); ToggleChoice(multi,0);
    multiSession.questions.append(multi);
    multiSession.judge(multiUser);
    assert(multiSession.acceptedCount==1);
    assert(multiUser["WA"].size()==1);
    ResetAttempt(multi); ToggleChoice(multi,0); ToggleChoice(multi,1); ToggleChoice(multi,2);
    multiSession.questions.append(multi);
    multiSession.judge(multiUser);
    assert(multiSession.acceptedCount==1);
    assert(multiUser["WA"].size()==1); // Same question is not duplicated.
    ResetAttempt(multi); ToggleChoice(multi,0); ToggleChoice(multi,0);
    assert(!HasSelection(multi));
    auto invalid=multi; invalid["answer"]="Z";
    assert(!ValidQuestion(invalid));
    invalid["answer"]=""; assert(!ValidQuestion(invalid));
    invalid["type"]=Json::arrayValue; assert(!ValidQuestion(invalid));

    // Manual collection applies to correct answers; exclusion wins over automatic collection.
    ExerciseSession manual;
    Json::Value manualUser;
    manual.questions.append(question(0));
    manual.setWrongAnswerPolicy(0,1,manualUser);
    manual.judge(manualUser);
    assert(manual.acceptedCount==1 && manualUser["WA"].size()==1);
    manual.setWrongAnswerPolicy(0,2,manualUser);
    assert(manualUser["WA"].empty());
    manual.questions.append(question(1));
    manual.setWrongAnswerPolicy(1,2,manualUser);
    manual.judge(manualUser);
    assert(manualUser["WA"].empty());
    manual.collectWrongAnswers=false;
    manual.questions.append(question(-1));
    manual.judge(manualUser);
    assert(manualUser["WA"].empty());

    // Starting/retrying does not drain the persistent wrong-answer collection.
    std::mt19937 random(123);
    Json::Value practiceUser;
    AddWrongAnswer(practiceUser,question(1));
    AddWrongAnswer(practiceUser,question(-1));
    std::vector<Json::Value> pool;
    for(const auto& q:practiceUser["WA"]) pool.push_back(q);
    ExerciseSession practice;
    assert(practice.start(pool,true,random));
    assert(practiceUser["WA"].size()==2 && practice.unvisited.size()==1);
    ToggleChoice(practice.questions[0],0);
    practice.judge(practiceUser);
    assert(practiceUser["WA"].size()==2); // Correct by default still stays in the collection.
    assert(practice.next(random));
    practice.removeCorrectWrongAnswers=true;
    ToggleChoice(practice.questions[1],0);
    practice.judge(practiceUser);
    assert(practiceUser["WA"].size()==1);
    practice.allowRepeated=true;
    practice.setWrongAnswerPolicy(0,2,practiceUser);
    assert(practiceUser["WA"].empty());
    for(int i=0;i<8;++i) {
        assert(practice.next(random));
        auto& q=practice.questions[practice.currentIndex];
        assert(!q["Locked"].asBool() && !HasSelection(q));
        if(QuestionKey(q)==QuestionKey(practice.questions[0])) {
            assert(q["WrongAnswerPolicy"].asInt()==2);
            ToggleChoice(q,1); practice.judge(practiceUser);
            assert(!ContainsWrongAnswer(practiceUser,q));
        }
    }
    assert(!practice.start({},true,random));

    ExerciseSession randomized;
    randomized.randomizeChoices = true;
    auto randomQuestion = question(-1);
    randomQuestion["choices"].append("third");
    randomQuestion["choices"].append("fourth");
    randomized.start({randomQuestion}, false, random);
    auto order = randomized.choiceOrder[0];
    std::sort(order.begin(), order.end());
    assert((order == std::vector<int>{0,1,2,3}));
    int displayOfCorrectChoice = -1;
    for (int i = 0; i < 4; ++i) {
        const int sourceIndex = randomized.displayedChoiceIndex(0, i);
        assert(sourceIndex >= 0 && sourceIndex < 4);
        if (sourceIndex == 0) displayOfCorrectChoice = i;
    }
    assert(displayOfCorrectChoice >= 0);
    ToggleChoice(randomized.questions[0], randomized.displayedChoiceIndex(0, displayOfCorrectChoice));
    randomized.submitCurrent(practiceUser);
    assert(randomized.acceptedCount == 1);
    randomized.setChoiceRandomization(false, random);
    for (int i = 0; i < 4; ++i) assert(randomized.displayedChoiceIndex(0, i) == i);

    // Legacy duplicates migrate without losing distinct questions or answers.
    Json::Value legacy;
    legacy["WA"].append(question(1)); legacy["WA"].append(question(1));
    legacy["WA"].append(question(-1));
    NormalizeWrongAnswers(legacy);
    assert(legacy["WA"].size()==2);
    assert(!HasSelection(legacy["WA"][0]));
    RemoveWrongAnswer(legacy,question(1));
    assert(legacy["WA"].size()==1);

    // Editor stores multiple answers and remaps them when an option is deleted.
    editor.loadDefault("Multi"); editor.setMultiple(0,true);
    editor.changeAnswer(0,2);
    assert(CorrectAnswers(editor.getProblem(0))=="AC");
    editor.removeChoice(0,1);
    assert(CorrectAnswers(editor.getProblem(0))=="AB");
    assert(ValidQuestion(editor.getProblem(0)));
    editor.changeAnswer(0,0); editor.changeAnswer(0,1);
    assert(!ValidQuestion(editor.getProblem(0)));
    editor.setMultiple(0,false);
    assert(CorrectAnswers(editor.getProblem(0))=="A");
    editor.data()["order"]=7;
    ProblemSetEditor loaded;
    loaded.load(editor.exportProblems());
    assert(loaded.data()["order"].asInt()==7);

    // Natural numeric order and explicit sort indices are shared by all lists.
    Json::Value sets;
    for(const char* name:{"Unit 10","Unit 2","Unit 1"}) sets[name]["name"]=name;
    auto names=OrderedProblemSetNames(sets);
    assert((names==std::vector<std::string>{"Unit 1","Unit 2","Unit 10"}));
    sets["Unit 10"]["order"]=0;
    names=OrderedProblemSetNames(sets);
    assert(names.front()=="Unit 10");
    assert(NaturalLess("Unit 99999999999999999999","Unit 100000000000000000000"));
    assert(MoveProblemSet(sets,"Unit 10","Unit 2",true));
    assert((OrderedProblemSetNames(sets)==std::vector<std::string>{"Unit 1","Unit 2","Unit 10"}));
    assert(MoveProblemSet(sets,"Unit 10","Unit 1",false));
    assert(OrderedProblemSetNames(sets).front()=="Unit 10");
    assert(!MoveProblemSet(sets,"Unit 10","Unit 10",true));
    assert(!MoveProblemSet(sets,"missing","Unit 1",false));
    assert(!MoveProblemSet(sets,"Unit 10","Unit 1",false));

    Json::Value chronological;
    AddWrongAnswer(chronological,question(0));
    AddWrongAnswer(chronological,question(1));
    AddWrongAnswer(chronological,question(0));
    assert(chronological["WA"].size()==2);
    assert(QuestionKey(chronological["WA"][1])==QuestionKey(question(0)));
    assert(chronological["WA"][0]["LastWrongAt"].asInt64()<chronological["WA"][1]["LastWrongAt"].asInt64());
    auto first=chronological["WA"][0];
    RemoveWrongAnswer(chronological,first);
    RestoreWrongAnswer(chronological,first);
    assert(QuestionKey(chronological["WA"][0])==QuestionKey(first));
    NormalizeWrongAnswers(chronological);
    assert(QuestionKey(chronological["WA"][0])==QuestionKey(first));

    // Finishing ignores only the newest untouched question, including the denominator.
    ExerciseSession finishSession;
    Json::Value finishUser;
    finishSession.questions.append(question(0));
    finishSession.questions.append(question(-1));
    finishSession.currentIndex=0;
    finishSession.submitCurrent(finishUser); // Reviewing an older question must not grade the new one.
    assert(!finishSession.questions[1]["Locked"].asBool());
    finishSession.finish(finishUser);
    assert(finishSession.gradedCount()==1 && finishSession.acceptedCount==1);
    assert(finishUser["WA"].empty());
    finishSession.finish(finishUser);
    assert(finishSession.gradedCount()==1 && finishUser["WA"].empty());

    ExerciseSession untouched;
    untouched.questions.append(question(-1));
    untouched.finish(finishUser);
    assert(untouched.gradedCount()==0 && finishUser["WA"].empty());

    ExerciseSession skipped;
    skipped.questions.append(question(-1));
    skipped.questions.append(question(-1));
    Json::Value skippedUser;
    skipped.finish(skippedUser);
    assert(skipped.gradedCount()==1 && skippedUser["WA"].size()==1);
    assert(!skipped.questions[1]["Locked"].asBool());

    ExerciseSession partial;
    ResetAttempt(multi); ToggleChoice(multi,0);
    partial.questions.append(multi);
    Json::Value partialUser;
    partial.finish(partialUser);
    assert(partial.gradedCount()==1 && partial.acceptedCount==0 && partialUser["WA"].size()==1);

    std::cout << "UI model regression tests passed\n";
}
