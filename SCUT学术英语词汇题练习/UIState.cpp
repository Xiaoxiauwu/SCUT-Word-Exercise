#include "UIInternal.h"

namespace scut_ui {

float Screen_Width = 0, Screen_Height = 0, width = 0, height = 0;
float buttonwidth = 0, gap = 0;
int baseoffset = 0;
ImFont *Font = nullptr, *Font_Big = nullptr;
bool ButtonLock = false;
namespace { ImageLoader images; }
ImageLoader* imageLoader = &images;
std::stack<int> TabStack;
ProblemSetState problemSets;
ExerciseSession exercise;
ProblemSetEditor editor;
Json::Value userInfo;
std::string pre_name, name;
ImVec4 TinyButton[5];
int themeIndex = Red;
ImVec4* Color = nullptr;
float kbw = 6.0f;
std::function<void(bool)> SettingWindow;
std::vector<SubWindow> subWindow;
float animeTimeL = 0, animeTimeR = 0, switchFrameV = 0;
int FrameDelay = 300;
std::vector<float> dx;
bool rearrange = false;

unsigned long long HASH(std::string text) {
    unsigned long long hash = 0;
    for (auto c : text) hash = ((hash + c) * 131) % 1000000000000001267ULL;
    return hash;
}

void judge(int skip) { exercise.judge(userInfo, skip); SaveUserData(); }

} // namespace scut_ui

