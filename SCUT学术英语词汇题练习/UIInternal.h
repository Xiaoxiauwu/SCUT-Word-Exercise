#pragma once
// Private UI contract. Application callers should include UI.h only.
#include "UI.h"
#include "AnimeManager.h"
#include "ExerciseSession.h"
#include "ProblemSetEditor.h"
#include "UILayout.h"
#include "UIMarkdown.h"

namespace scut_ui {
enum TAB { Main, ProblemsSetSelect, Exercise, About, EditorProblemSetSelect, Editor, WrongAnswers };
enum Color_ { Red, Green, Blue, Orange };
struct SubWindow {
    std::function<void(bool)> window;
    bool isDraw;
    bool isAct;
};
struct ProblemSetState {
    Json::Value sets;
    std::set<std::string> selected;
    std::map<std::string, std::string> nameToPath, pathToName;
};

extern float Screen_Width, Screen_Height, width, height, buttonwidth, gap;
extern int baseoffset;
extern ImFont *Font, *Font_Big;
extern bool ButtonLock;
extern ImageLoader* imageLoader;
extern std::stack<int> TabStack;
extern ProblemSetState problemSets;
extern ExerciseSession exercise;
extern ProblemSetEditor editor;
extern Json::Value userInfo;
extern std::string pre_name, name;
extern ImVec4 TinyButton[5];
extern ImVec4* Color;
extern int themeIndex;
extern float kbw, animeTimeL, animeTimeR, switchFrameV;
extern int FrameDelay;
extern std::function<void(bool)> SettingWindow;
extern std::vector<SubWindow> subWindow;
extern std::vector<float> dx;
extern bool rearrange;

unsigned long long HASH(std::string text);
void judge(int skip = 0);
void SaveJsonToFile(Json::Value value, std::string path);
void EXIT();
void SaveUserData();
std::vector<std::wstring> GetAllFiles(std::wstring path, std::wstring fileType);
void refresh();
void Initialize(ID3D11Device* device);
void UpdateScreenRect();
void ApplyTheme();
void RenderDialogs();
void popWindow(std::string title, std::string content,
               ImVec4 color = ImVec4(1, 1, 1, 1));
void RenderHome();
void RenderProblemSetSelection();
void RenderExercise();
void RenderProblemSetManager();
void RenderEditor();
void RenderAbout();
void RenderWrongAnswers();
bool EditText(const char* label, std::string& text, bool multiline = false, ImVec2 size = ImVec2(0, 0));
bool PageHeader(const char* title);
void SavePracticeSettings();
}

