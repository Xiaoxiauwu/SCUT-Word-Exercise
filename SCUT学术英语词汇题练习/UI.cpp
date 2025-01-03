#include "UI.h"
#include "AnimeManager.h"

/////////////////////////////////////GLOBAL VARIABLES////////////////////////////////////////
float Screen_Width;
float Screen_Height;
float width;
float height;
float buttonwidth;
int baseoffset;
float gap;

ImFont* Font, * Font_Big;

bool ButtonLock = 0;//  播放过场动画（粒子）时，用于锁定全局按钮的“线程锁”

ImageLoader* imageLoader = new ImageLoader();

std::stack<int> TabStack;//状态栈
enum TAB {
    Main,
    ProblemsSetSelect,
    Exercise,
};

Json::Value ProblemSets;//按名字建立键值映射对
std::set<std::string> selected;//选择了哪些题集

//=============================处理判题==============================/
Json::Value userInfo;
const unsigned long long M = 1000000000000001267;
const unsigned long long P = 131;
unsigned long long HASH(std::string s) {
    unsigned long long h = 0;
    for (auto& c : s) {
        h += c;
        h *= P;
        h %= M;
    }
    return h;
}

int ACcount = 0;
Json::Value ExerciseList = Json::arrayValue;
void judge(int skip = 0) {//负责处理List中所有未judge的题
    int cnt = ExerciseList.size();
    for (int i = 0; i < cnt; i++) {
        if (ExerciseList[i]["Locked"].asBool() || (skip && (i == cnt - 1) && (ExerciseList[i]["Choose"].asInt() == 4))) continue;
        //std::cout << (ExerciseList[i]["Choose"].asInt() == 4) << ", " << (i == cnt - 1) << skip << "]\n";
        ExerciseList[i]["Locked"] = 1;

        if (ExerciseList[i]["Choose"].asInt() != ExerciseList[i]["answer"].asString()[0] - 'A') {//没来得及选也算错
            ExerciseList[i]["statu"] = 3;

            //std::cout << "Bad : " << ExerciseList[i]["choose"].asInt() << "<->" << (ExerciseList[i]["answer"].asString()[0] - 'A') << "]\n";

            if (!userInfo.isMember("WA")) {
                userInfo["WA"] = Json::arrayValue;
            }

            userInfo["WA"].append(ExerciseList[i]);//直接加入错题本，不管有没有重
            userInfo["WA"].back()["statu"] = 0;
            userInfo["WA"].back()["Choose"] = 4;
            userInfo["WA"].back()["Locked"] = 0;
        }
        else {
            //std::cout << "ok\n";
            ACcount++;
            ExerciseList[i]["statu"] = 2;
        }
    }
}

void uniqueWA() {//错题本按时间顺序去重//干脆不去重了，还能反映出这题你错的频率高
    return;
    std::set<unsigned long long> appear;
    std::vector<Json::Value> tmp;

    for (int i = userInfo["WA"].size() - 1; i >= 0; i--) {
        auto H = HASH(
            userInfo["WA"][i]["problem"].asString() +
            userInfo["WA"][i]["choices"][0].asString() +
            userInfo["WA"][i]["choices"][1].asString() +
            userInfo["WA"][i]["choices"][2].asString() +
            userInfo["WA"][i]["choices"][3].asString() +
            userInfo["WA"][i]["answer"].asString());
        if (appear.count(H)) continue;

        appear.insert(H);
        tmp.push_back(userInfo["WA"][i]);
    }

    userInfo["WA"] = Json::arrayValue;//清空
    for (int i = tmp.size() - 1; i >= 0; i--) {
        userInfo["WA"].append(tmp[i]);
    }
}

/*
怎么更新错题本？hard，因为要去重，感觉有点麻烦，每次结算的时候线性扫描一遍去重吧
*/

int index = 0;
//int choose = 4;//4是不选
bool isWApractise = 0;
ImVec4 TinyButton[5];
std::vector<Json::Value> Exercise_VisitedProblems, Exercise_UnvisitedProblems;
//支持随机访问，删除的数据结构，vector开两个应该就行
/////////////////////////////////////MAIN FUNCTION////////////////////////////////////////
void EXIT() {
    Json::StreamWriterBuilder writebuild;
    writebuild["emitUTF8"] = true;  //utf8支持,加这句,utf8的中文字符会编程\uxxx

    string document = Json::writeString(writebuild, userInfo);
    //cout << R;
    //cout << "保存用户信息成功";
    writeFileFromString("userinfo.dat", ENCODE(document));

    exit(0);
}

std::vector<std::wstring> GetAllFiles(std::wstring path, std::wstring fileType) {
    std::vector<std::wstring> files;
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    LARGE_INTEGER size;
    if (FindFirstFile(path.c_str(), &findData) == INVALID_HANDLE_VALUE) {// 查找文件夹，如果找不到目的文件夹的操作
        //  CreateDirectoryA(path.c_str(), NULL);
        //std::cout << "???\n";
        return files;// 空
    }
    hFind = FindFirstFile((path + fileType).c_str(), &findData); //搜索第一个文件，创建并返回搜索句柄，有则返回TRUE
    if (hFind == INVALID_HANDLE_VALUE) {
        //std::cout << "Failed to find first file!\n";DEBUG
        return files;
    }

    do {
        // 忽略"."和".."两个结果 
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)   //strcmp 比较字符，相同则返回0
            continue;
        files.push_back(findData.cFileName);
    } while (FindNextFile(hFind, &findData));
    return files;
}

void refresh() {//刷新新题集
    auto problems = GetAllFiles(L"problemSet", L"\\*.json");

    for (auto& pro : problems) {
        string rawInfo = readFileIntoString(encode::wideToOme(std::wstring(L"problemSet\\") + pro).c_str());
        //std::cout << rawInfo << "[";
        Json::Value R = readJsonFromString(rawInfo);
        
        //std::cout << R["name"].asString() << "]\n";
        ProblemSets[R["name"].asString()] = R;//重名会有覆盖，之后再改
    }
}

void init(ID3D11Device* g_pd3dDevice) {
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = nullptr;
    ImFontConfig Font_cfg;
    Font_cfg.FontDataOwnedByAtlas = false;

    //ImFont* Font = io.Fonts->AddFontFromFileTTF("..\\ImGui Tool\\Font.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    //Font = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, Screen_Width / 1024 * 18.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    //Font_Big = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, Screen_Width / 1024 * 24.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    if (checkFile(L"res\\Source Han Sans & Saira Hybrid-Regular.ttf")) {//  优先选择res里的这个字体
        Font = io.Fonts->AddFontFromFileTTF("res\\Source Han Sans & Saira Hybrid-Regular.ttf", Screen_Width / 1024 * 18.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    }
    else {
        //  防止用户误删字体文件，用户换别的字体文件的话，不一定保证字库包含中文，所以用自己的
        Font = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, Screen_Width / 1024 * 18.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    }
    
    //Font_Big = io.Fonts->AddFontFromFileTTF("Source Han Sans & Saira Hybrid-Regular.ttf", Screen_Width / 1024 * 24.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());

    setFont(Font);

    //  初始化图片资源
    imageLoader->Init(g_pd3dDevice);
    
    //注册并绑定图片
    //imageLoader->LoadImageToTexture("drawpix", "res/drawpix.png");
    //imageLoader->LoadImageToTexture("eraser", "res/eraser.png");

    //  文件管理器需要的一个初始化
    ifd::FileDialog::Instance().Init(g_pd3dDevice);
    ifd::FileDialog::Instance().CreateTexture = [](uint8_t* image_data, int width, int height, char fmt) -> void* {
        if (!image_data) {
            //  图像加载失败的处理
            //  不说话，生闷气，传个空指针回去让调用者自己猜
            return nullptr;
        }
        // 创建纹理
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = fmt == 0 ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = image_data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        HRESULT hr = ifd::FileDialog::Instance().device->CreateTexture2D(&desc, &subResource, &pTexture);
        if (FAILED(hr)) {
            //  创建纹理失败的处理
            //  不说话，生闷气，传个空指针回去让调用者自己猜
            return nullptr;
        }

        // Create texture view
        ID3D11ShaderResourceView* out_srv = NULL;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        hr = ifd::FileDialog::Instance().device->CreateShaderResourceView(pTexture, &srvDesc, &out_srv);
        pTexture->Release();
        if (FAILED(hr)) {
            //  创建SRV失败的处理
            //  不说话，生闷气，传个空指针回去让调用者自己猜
            return nullptr;
        }

        return (void*)out_srv;
    };

    ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
        ID3D11ShaderResourceView* texV = (ID3D11ShaderResourceView*)tex;
        if (texV != nullptr)texV->Release();
        //GLuint texID = (GLuint)tex;
        //glDeleteTextures(1, &texID);
    };
    
    TinyButton[0] = ImVec4(210.0f / 255.0f, 210.0f / 255.0f, 210.0f / 255.0f, 1.0f);
    TinyButton[1] = ImVec4(77.0f / 255.0f, 129.0f / 255.0f, 251.0f / 255.0f, 1.0f);
    TinyButton[2] = ImVec4(0.0f / 255.0f, 183.0f / 255.0f, 18.0f / 255.0f, 1.0f);
    TinyButton[3] = ImVec4(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
    TinyButton[4] = ImVec4(230.0f / 255.0f, 236.0f / 255.0f, 0.0f / 255.0f, 1.0f);

    getScreenRect();

    refresh();

    std::string rawInfo = readFileIntoString("userinfo.dat");
    Json::Value R = readJsonFromString(DECODE(rawInfo));

    if (R == Json::nullValue) {
        R["WA"] = Json::arrayValue;
    }
    
    userInfo = R;
    
    while (TabStack.size()) TabStack.pop();
    TabStack.push(Main);

    //  注册播放用音频
    //AudioLoader::get().addButton(L"res\\Tap1.mp3", "TAP");
    //AudioLoader::get().addButton(L"res\\Tap5.mp3", "SELECT");
    //AudioLoader::get().addBGM(L"res\\baba.mp3", "GAME");
}

void getScreenRect() {
    Screen_Width = MIN(1561, MAX(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)));//  获取显示器的宽
    Screen_Height = MIN(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));//    获取显示器的高     //
    width = Screen_Width * 0.6f;
    height = Screen_Width * 0.4f;
    buttonwidth = width / 4;
    baseoffset = Screen_Width / 1024 * 24.0f + width / 25;

    gap = height / 80;
    //cout << 12345;
}

////////////////////////////////////GLOBAL VARIABLE FOR FRAME


static int Color_ = 0;
enum Color_ {
    Red,
    Green,
    Blue,
    Orange
};
ImVec4* Color;

//  有时间就把这些整理进单独的FRAME
float kbw = 6.0;//  按钮长宽比例 1 : 6

std::function<void(bool)> SettingWindow;
struct SubWindow {
    std::function<void(bool)> window;//  布尔值：就算我绘制了控件，我该响应吗？
    bool isDraw;//  即便我不在栈顶，你也会绘制吗？
    bool isAct;//   即便我不在栈顶，你也会相应我的点击事件吗？
};
std::vector<SubWindow> subWindow;//子窗口栈

//  动画时间戳
float animeTimeL = 0;
float animeTimeR = 0;
int FrameDelay = 300;

////////////////////////////FRAME
void MAIN() {
    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f });//    设置绘制指针
    drawText(width / 2, height / 4, height / 10, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), encode::string_To_UTF8("SCUT学术英语词汇练习"));

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f});//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"开始练习", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size()==0)   //  注册按钮类
    {
        //ButtonLock = 1;
        TabStack.push(TAB::ProblemsSetSelect);
        //animeManager.addAnime(&animeTimeL, 0, 1, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::ProblemsSetSelect;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, 1, FrameDelay, InSine);
        //});
    }
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + buttonwidth / kbw + gap});
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"题库管理与编辑", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        ButtonLock = 1;
        function<void(bool)> window = [&](bool alive)->void {
            drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
            float w, h, x, y;//长宽，中心坐标
            w = width / 2.8;
            h = height / 4;
            x = width / 2;
            y = height / 2;
            ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
            ImGui::BeginChild(
                u8"咕咕",
                ImVec2(w, h),
                1,
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings
            );
            drawText(
                w / 2, Screen_Width / 1024 * 24.0f / 2,
                Screen_Width / 1024 * 24.0f / 2,
                ImVec4(255, 255, 255, 255),
                u8"Not Found"
            );
            ImGui::Indent(width / 30);
            ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
            ImGui::Text(u8"暂时没写");

            //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

            ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
            ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
            if (ImGui::Button(u8"知道了", { buttonwidth / 2,buttonwidth / kbw }) && alive)
            {
                subWindow.pop_back();
                ButtonLock = 0;
            }
            ImGui::PopStyleColor();
            ImGui::EndChild();
            };
        subWindow.push_back({ window,0,0 });
    }
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) * 2 });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"错题练习", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        if (userInfo["WA"].size() == 0) {
            ButtonLock = 1;
            function<void(bool)> window = [&](bool alive)->void {
                drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
                float w, h, x, y;//长宽，中心坐标
                w = width / 2.8;
                h = height / 4;
                x = width / 2;
                y = height / 2;
                ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
                ImGui::BeginChild(
                    u8"空",
                    ImVec2(w, h),
                    1,
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoSavedSettings
                );
                drawText(
                    w / 2, Screen_Width / 1024 * 24.0f / 2,
                    Screen_Width / 1024 * 24.0f / 2,
                    ImVec4(255, 255, 255, 255),
                    u8"Time Limit Exceeded"
                );
                ImGui::Indent(width / 30);
                ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
                ImGui::Text(u8"你还没有错题，或你已经补过题了");

                //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

                ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
                ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
                if (ImGui::Button(u8"了解", { buttonwidth / 2,buttonwidth / kbw }) && alive)
                {
                    subWindow.pop_back();
                    ButtonLock = 0;
                }
                ImGui::PopStyleColor();
                ImGui::EndChild();
                };
            subWindow.push_back({ window,0,0 });
        }
        else {
            Exercise_VisitedProblems.clear();
            Exercise_UnvisitedProblems.clear();
            for (auto& problem : userInfo["WA"]) {
                Exercise_UnvisitedProblems.push_back(problem);
            }
            selected.clear();

            std::shuffle(Exercise_UnvisitedProblems.begin(), Exercise_UnvisitedProblems.end(), myrand);

            ExerciseList.append(Exercise_UnvisitedProblems.back());
            Exercise_VisitedProblems.push_back(Exercise_UnvisitedProblems.back());
            Exercise_UnvisitedProblems.pop_back();

            userInfo["WA"] = Json::arrayValue;

            index = 0;
            //choose = 4;
            isWApractise = 1;
            ACcount = 0;
            TabStack.push(TAB::Exercise);
        }
        //animeManager.addAnime(&animeTimeL, 0, 1, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::ProblemsSetSelect;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, 1, FrameDelay, InSine);
        //});
    }
    ImGui::PopStyleColor();

    ImGui::SetCursorPos({ width * 0.5f - buttonwidth / 2, height * 0.6f + (buttonwidth / kbw + gap) * 3 });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"退出", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        //ButtonLock = 1;
        TabStack.pop();
        //animeManager.addAnime(&animeTimeL, 0, 1, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::ProblemsSetSelect;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, 1, FrameDelay, InSine);
        //});
    }
    ImGui::PopStyleColor();

    drawText(width / 2, height - width / 120 - Screen_Width / 1024 * 12.0f, Screen_Width / 1024 * 12.0f, ImVec4(0.6, 0.6, 0.6, 1), u8"Version 1.0.0 beta  By XiaoXia");
}

void PROBLEMSET() {
    ImGui::SetCursorPos({ gap, gap + buttonwidth / kbw + gap});
    ImGui::BeginChild(
        u8"题集选择",
        ImVec2(width / 3, height - gap - buttonwidth / kbw - 2 * gap),
        1,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings// |
        //ImGuiWindowFlags_NoMove
    );{
        int cnt = 0;//万一有人把基础题库删了
        for (int i = 0; i < 5; i++) {
            std::string name = std::string("Unit ") + char('1' + i);

            if (ProblemSets.isMember(name)) {
                float WIDTH = width / 3 - 2 * gap;
                float HEIGHT = buttonwidth / kbw * 1.33;
                ImGui::SetCursorPos({ gap, gap + (HEIGHT + gap) * cnt });//    设置绘制指针

                ImGui::PushStyleColor(ImGuiCol_Button, selected.count(name) ? ImVec4(162 / 255.0, 21 / 255.0, 44 / 255.0, 1.0f) : Color[ImGuiCol_Button]);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, selected.count(name) ? ImVec4(162 / 255.0, 11 / 255.0, 44 / 255.0, 1.0f) : Color[ImGuiCol_ButtonHovered]);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, selected.count(name) ? ImVec4(162 / 255.0, 16 / 255.0, 44 / 255.0, 1.0f) : Color[ImGuiCol_ButtonActive]);
                if (ImGui::Button(name.c_str(), {WIDTH ,HEIGHT}) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
                {
                    if (selected.count(name)) {
                        selected.erase(name);
                    }
                    else {
                        selected.insert(name);
                    }
                    //Tab = TAB::Main;
                }
                ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();
                cnt++;
            }
        }
        if (ButtonLock) drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
    }
    ImGui::EndChild();

    //随机跳题，可重复，不可重复，结算界面，错题记录

    //随机抽题
    ImGui::SetCursorPos({ width * 2 / 3 - buttonwidth / 2, height / 2 });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"开始抽题", { buttonwidth, buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        if (selected.size() == 0) {
            ButtonLock = 1;
            function<void(bool)> window = [&](bool alive)->void {
                drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
                float w, h, x, y;//长宽，中心坐标
                w = width / 2.8;
                h = height / 4;
                x = width / 2;
                y = height / 2;
                ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
                ImGui::BeginChild(
                    u8"空",
                    ImVec2(w, h),
                    1,
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoSavedSettings
                );
                drawText(
                    w / 2, Screen_Width / 1024 * 24.0f / 2,
                    Screen_Width / 1024 * 24.0f / 2,
                    ImVec4(255, 255, 255, 255),
                    u8"Unaccepted"
                );
                ImGui::Indent(width / 30);
                ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
                ImGui::Text(u8"请至少选择一组题集");

                //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

                ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
                ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
                if (ImGui::Button(u8"了解", { buttonwidth / 2,buttonwidth / kbw }) && alive)
                {
                    subWindow.pop_back();
                    ButtonLock = 0;
                }
                ImGui::PopStyleColor();
                ImGui::EndChild();
                };
            subWindow.push_back({ window,0,0 });
        }
        else {
            Exercise_VisitedProblems.clear();
            Exercise_UnvisitedProblems.clear();
            for (auto& name : selected) {
                int n = ProblemSets[name]["problems"].size();//可能还得存个，题目出自哪个题单，但是我懒得写了
                for (int i = 0; i < n; i++) {
                    Exercise_UnvisitedProblems.push_back(ProblemSets[name]["problems"][i]);
                    Exercise_UnvisitedProblems.back()["Locked"] = Json::booleanValue;
                    Exercise_UnvisitedProblems.back()["Statu"] = 0;
                    Exercise_UnvisitedProblems.back()["Choose"] = 4;

                    //std::cout << Exercise_UnvisitedProblems.size() << "]\n";//test
                }
            }
            selected.clear();

            std::shuffle(Exercise_UnvisitedProblems.begin(), Exercise_UnvisitedProblems.end(), myrand);
            
            ExerciseList.append(Exercise_UnvisitedProblems.back());
            Exercise_VisitedProblems.push_back(Exercise_UnvisitedProblems.back());
            Exercise_UnvisitedProblems.pop_back();

            index = 0;
            //choose = 4;
            ACcount = 0;
            TabStack.push(TAB::Exercise);
        }
    }
    ImGui::PopStyleColor();
    //是否允许题目重复
    //启用判题器
    

    //抽一组题目做
    //

    //drawText_autoNewLine(
    //    0.1 * width, 0.5 * height, width / 50, 
    //    ImVec4(255, 255, 255, 255), 
    //    "The topic of dynamics is based on the ___ that forces create movements which can be described as spatial changes within an all-controlling time domain.", 
    //    width * 0.9, //这个参数实际上是“右边界”
    //    " ", 
    //    0
    //);

    ImGui::SetCursorPos({ gap, gap });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"Back", { buttonwidth / 4, buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        selected.clear();
        //ButtonLock = 1;
        TabStack.pop();
        //animeManager.addAnime(&animeTimeL, 0, width, FrameDelay, OutSine, 0, [&]() {//    注册动画
        //    ButtonLock = 0;
        //    Tab = TAB::Main;//    页面切换
        //    animeManager.addAnime(&animeTimeR, 0, width, FrameDelay, InSine);
        //    });
    }
    ImGui::PopStyleColor();
}

void EXERCISE() {
    static bool repeatedProblem = 0;//允许重复题出现
    static bool AutoJudge = 0;//自动判题，选了就Judge

    int cnt = ExerciseList.size();

    auto rect = getTextSize_autoNewLine(width / 50, ExerciseList[index]["problem"].asString(), width * 2 / 3 - gap - 2 * gap);
    //drawRect(gap, gap, rect.x, rect.y, ImVec4(0.5f, 0.0f, 0.0f, 1.0f), 0);
    float WIDTH = width * 2 / 3 - gap;
    float HEIGHT = rect.y + 2 * gap + (buttonwidth / kbw + gap) * 2;
    ImGui::SetCursorPos({ gap, height / 2 - HEIGHT / 2 });
    ImGui::BeginChild(
        u8"题",
        ImVec2(WIDTH, HEIGHT),//height - gap - buttonwidth / kbw - 2 * gap),
        1,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings// |
        //ImGuiWindowFlags_NoMove
    ); {
        
        drawText_autoNewLine(
            gap, gap,
            width / 50,
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
            std::to_string(index + 1) + std::string(". ") + (ExerciseList[index]["problem"].asString()),
            width * 2 / 3 - gap - 2 * gap,
            u8"",
            0
        );

        
        std::function<void(int)> setColor = [&](int i) -> void {
            if (ExerciseList[index]["Locked"].asBool()) {//已有结果
                ImVec4 tmp = ImVec4(44 / 255.0f, 44 / 255.0f, 44 / 255.0f, 150 / 255.0f);
                if (ExerciseList[index]["Choose"].asInt() == i) {
                    tmp = ImVec4(255 / 255.0f, 0 / 255.0f, 0 / 255.0f, 150 / 255.0f);
                }
                if (ExerciseList[index]["answer"].asString()[0] - 'A' == i) {
                    tmp = ImVec4(0 / 255.0f, 255 / 255.0f, 0 / 255.0f, 150 / 255.0f);
                }
                ImGui::PushStyleColor(ImGuiCol_Button, tmp);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, tmp);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, tmp);                
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, 
                    ExerciseList[index]["Choose"].asInt() != i ? ImVec4(44 / 255.0f, 44 / 255.0f, 44 / 255.0f, 150 / 255.0f) : Color[ImGuiCol_Button]
                );
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 
                    ExerciseList[index]["Choose"].asInt() != i ? ImVec4(54 / 255.0f, 54 / 255.0f, 54 / 255.0f, 150 / 255.0f) : Color[ImGuiCol_ButtonHovered]
                );
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, 
                    ExerciseList[index]["Choose"].asInt() != i ? ImVec4(46 / 255.0f, 46 / 255.0f, 46 / 255.0f, 150 / 255.0f) : Color[ImGuiCol_ButtonActive]
                );
            }
        };

        ImGui::SetCursorPos({ gap, gap + rect.y + gap});
        setColor(0);
        if (ImGui::Button((std::string("A. ") + ExerciseList[index]["choices"][0].asString()).c_str(), { buttonwidth ,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0 && !ExerciseList[index]["Locked"].asBool())   //  注册按钮类
        {
            ExerciseList[index]["statu"] = 1;
            ExerciseList[index]["Choose"] = 0;
            if (AutoJudge) judge();
        }
        ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

        ImGui::SetCursorPos({ WIDTH - gap - buttonwidth, gap + rect.y + gap});
        setColor(1);
        if (ImGui::Button((std::string("B. ") + ExerciseList[index]["choices"][1].asString()).c_str(), { buttonwidth ,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0 && !ExerciseList[index]["Locked"].asBool())   //  注册按钮类
        {
            ExerciseList[index]["statu"] = 1;
            ExerciseList[index]["Choose"] = 1;
            if (AutoJudge) judge();
        }
        ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

        ImGui::SetCursorPos({ gap, gap + rect.y + gap + buttonwidth / kbw + gap});
        setColor(2);
        if (ImGui::Button((std::string("C. ") + ExerciseList[index]["choices"][2].asString()).c_str(), { buttonwidth ,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0 && !ExerciseList[index]["Locked"].asBool())   //  注册按钮类
        {
            ExerciseList[index]["statu"] = 1;
            ExerciseList[index]["Choose"] = 2;
            if (AutoJudge) judge();
        }
        ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();

        ImGui::SetCursorPos({ WIDTH - gap - buttonwidth, gap + rect.y + gap + buttonwidth / kbw + gap });
        setColor(3);
        if (ImGui::Button((std::string("D. ") + ExerciseList[index]["choices"][3].asString()).c_str(), { buttonwidth ,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0 && !ExerciseList[index]["Locked"].asBool())   //  注册按钮类
        {
            ExerciseList[index]["statu"] = 1;
            ExerciseList[index]["Choose"] = 3;
            if (AutoJudge) judge();
        }
        ImGui::PopStyleColor(); ImGui::PopStyleColor(); ImGui::PopStyleColor();


        if (ButtonLock) drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
    }
    ImGui::EndChild();

    ImVec2 label_size = ImGui::CalcTextSize(u8"允许题目重复", NULL, true);
    ImGui::SetCursorPos({ gap , height - (gap + label_size.y) * 2 - gap });
    if (ImGui::Checkbox(u8"允许题目重复", &repeatedProblem)) {
        //如果点了会怎么样
        //测试用接口
        //std::cout << ExerciseList[index]["problem"].asString() << "\n";
    }

    ImGui::SetCursorPos({ gap , height - (gap + label_size.y) * 1 - gap });
    if (ImGui::Checkbox(u8"自动判题", &AutoJudge)) {
        judge(1);//是否跳过最后一题
        //什么事都没有感觉
    }

    //ExerciseList
    if (ExerciseList[index]["Choose"].asInt() != 4 && !ExerciseList[index]["Locked"].asBool() && !AutoJudge) {//如果选了至少一个，且该题没被judge过
        ImGui::SetCursorPos({ width / 3 - buttonwidth / 2, height / 2 + HEIGHT / 2 + gap + gap + buttonwidth / kbw });
        ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
        if (ImGui::Button(u8"一键判题", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)
        {
            judge();
        }
        ImGui::PopStyleColor();
    }

    if (ExerciseList[index]["Choose"].asInt() != 4 || index < cnt - 1) {//如果不准备开新题，或者已经选了最后一题，那么允许“下一题”
        ImGui::SetCursorPos({ width / 3 - buttonwidth / 2, height / 2 + HEIGHT / 2 + gap });
        ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
        if (ImGui::Button(u8"下一题", { buttonwidth,buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)
        {
            if (index == cnt - 1) {//迫切需要随机一道题
                if (repeatedProblem) {//随便随机一题就行
                    int i = rnd(0, (int)Exercise_VisitedProblems.size() + (int)Exercise_UnvisitedProblems.size() - 1);

                    if (i >= Exercise_VisitedProblems.size()) {
                        ExerciseList.append(Exercise_UnvisitedProblems.back());
                        Exercise_VisitedProblems.push_back(Exercise_UnvisitedProblems.back());
                        Exercise_UnvisitedProblems.pop_back();
                    }
                    else {
                        ExerciseList.append(Exercise_VisitedProblems[i]);
                    }
                    index++;
                }
                else {//只能随机没出现过的
                    if (Exercise_UnvisitedProblems.size() == 0) {//已经全写过一次了
                        ButtonLock = 1;
                        function<void(bool)> window = [&](bool alive)->void {
                            drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
                            float w, h, x, y;//长宽，中心坐标
                            w = width / 2.8;
                            h = height / 4;
                            x = width / 2;
                            y = height / 2;
                            ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
                            ImGui::BeginChild(
                                u8"无",
                                ImVec2(w, h),
                                1,
                                ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoSavedSettings
                            );
                            drawText(
                                w / 2, Screen_Width / 1024 * 24.0f / 2,
                                Screen_Width / 1024 * 24.0f / 2,
                                ImVec4(255, 255, 255, 255),
                                u8"Undersupply"
                            );
                            ImGui::Indent(width / 30);
                            ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
                            ImGui::Text(u8"这些题单你已经全刷完了");

                            //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

                            ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
                            ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
                            if (ImGui::Button(u8"了解", { buttonwidth / 2,buttonwidth / kbw }) && alive)
                            {
                                subWindow.pop_back();
                                ButtonLock = 0;
                            }
                            ImGui::PopStyleColor();
                            ImGui::EndChild();
                            };
                        subWindow.push_back({ window,0,0 });
                    }
                    else {
                        ExerciseList.append(Exercise_UnvisitedProblems.back());
                        Exercise_VisitedProblems.push_back(Exercise_UnvisitedProblems.back());
                        Exercise_UnvisitedProblems.pop_back();
                        index++;
                    }
                }
            }
            else {
                index++;
            }
        }
        ImGui::PopStyleColor();
    }

    WIDTH = width / 3 - 2 * gap;
    HEIGHT = height - gap - buttonwidth / kbw - 2 * gap;
    ImGui::SetCursorPos({ width * 2 / 3 + gap , gap + buttonwidth / kbw + gap });
    ImGui::BeginChild(
        u8"列表",
        ImVec2(WIDTH, HEIGHT),
        1,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings// |
        //ImGuiWindowFlags_NoMove
    ); {
        float siz = (WIDTH - 3 * gap) / 11;
        for (int i = 0; i < cnt; i++) {
            drawRect(gap / 2 + (siz + gap / 2) * (i % 10), gap + (siz + gap / 2) * (i / 10), siz, siz, TinyButton[ExerciseList[i]["statu"].asInt()]);
            drawText(gap / 2 + (siz + gap / 2) * (i % 10) + siz / 2, gap + (siz + gap / 2) * (i / 10) + siz / 2, width / 60, ImVec4(0, 0, 0, 255.0f), std::to_string(i + 1));
            ImGui::SetCursorPos({ gap / 2 + (siz + gap / 2) * (i % 10), gap + (siz + gap / 2) * (i / 10) });
            if (PaintBoard((std::string("_") + std::to_string(i + 1)).c_str(), {siz, siz}) && !ButtonLock && subWindow.size() == 0)
            {
                //std::cout << i << "]\n";
                index = i;
            }
        }
        if (ButtonLock) drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
    }
    ImGui::EndChild();

    ImGui::SetCursorPos({ gap, gap });//    设置绘制指针
    ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);//   设置绘制样式
    if (ImGui::Button(u8"结算", { buttonwidth / 4, buttonwidth / kbw }) && !ButtonLock && subWindow.size() == 0)   //  注册按钮类
    {
        judge();//强制judge一次

        uniqueWA();//错题本去重

        ButtonLock = 1;
        function<void(bool)> window = [&](bool alive)->void {
            drawRect(0, 0, width, height, ImVec4(0, 0, 0, 50));
            float w, h, x, y;//长宽，中心坐标
            w = width / 2.8;
            h = height / 4;
            x = width / 2;
            y = height / 2;
            ImGui::SetCursorPos({ x - w / 2, y - h / 2 });
            ImGui::BeginChild(
                u8"无",
                ImVec2(w, h),
                1,
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings
            );
            drawText(
                w / 2, Screen_Width / 1024 * 24.0f / 2,
                Screen_Width / 1024 * 24.0f / 2,
                ImVec4(255, 255, 255, 255),
                u8"Training Complished"
            );
            ImGui::Indent(width / 30);
            ImGui::SetCursorPos({ width / 30, Screen_Width / 1024 * 24.0f });
            std::stringstream ss;
            ss << std::setprecision(2) << std::fixed << 100.0 * ACcount / ExerciseList.size();//   显示图像缩放进程
            ImGui::Text(encode::string_To_UTF8(std::string("您的正确率为: ") + ss.str() + std::string("%%")).c_str());

            //ImGui::TextColored(ImVec4(255, 0, 0, 255), tip.c_str());

            ImGui::PushStyleColor(ImGuiCol_Button, Color[ImGuiCol_Button]);
            ImGui::SetCursorPos({ w / 2 - buttonwidth / 2 / 2, h - buttonwidth / kbw * 1.3f });
            if (ImGui::Button(u8"稳了", { buttonwidth / 2,buttonwidth / kbw }) && alive)
            {
                subWindow.pop_back();
                TabStack.pop();
                ButtonLock = 0;

                if (isWApractise) {//如果在错题练习，那么没抽的题目也要归还错题本
                    for (auto& p : Exercise_UnvisitedProblems) {
                        userInfo["WA"].append(p);
                        userInfo["WA"].back()["statu"] = 0;
                        userInfo["WA"].back()["Choose"] = 4;
                        userInfo["WA"].back()["Locked"] = 0;
                    }
                    isWApractise = 0;
                }

                ExerciseList.clear();
                Exercise_VisitedProblems.clear();
                Exercise_UnvisitedProblems.clear();
            }
            ImGui::PopStyleColor();
            ImGui::EndChild();
            };
        subWindow.push_back({ window,0,0 });
    }
    ImGui::PopStyleColor();
}
////////////////////////////
void MainUI() {
    if (TabStack.size() == 0) EXIT();

    ImGuiStyle& Style = ImGui::GetStyle();
    Color = Style.Colors;

    switch (Color_)
    {
    case Color_::Red:
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(172, 31, 54, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(192, 51, 74, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(172, 31, 54, 255);

        Color[ImGuiCol_Header] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(172, 31, 54, 255);

        Color[ImGuiCol_TitleBg] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_TitleBgActive] = ImColor(172, 31, 54, 255);
        Color[ImGuiCol_ResizeGrip] = ImColor(192, 51, 74, 255);
        Color[ImGuiCol_ResizeGripHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_ResizeGripActive] = ImColor(172, 31, 54, 255);

        //ImGuiCol_Separator,
        Color[ImGuiCol_SeparatorHovered] = ImColor(212, 71, 94, 255);
        Color[ImGuiCol_SeparatorActive] = ImColor(172, 31, 54, 255);
        break;
    case Color_::Green:
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(10, 105, 56, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(30, 125, 76, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(0, 95, 46, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(10, 105, 56, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(10, 105, 56, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(0, 95, 46, 255);

        Color[ImGuiCol_Header] = ImColor(10, 105, 56, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(30, 125, 76, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(0, 95, 46, 255);

        break;
    case Color_::Blue:
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(51, 120, 255, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(71, 140, 255, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(31, 100, 225, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(51, 120, 255, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(51, 120, 255, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(31, 100, 225, 255);

        Color[ImGuiCol_Header] = ImColor(51, 120, 255, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(71, 140, 255, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(31, 100, 225, 255);

        break;
    case Color_::Orange://233,87,33
        Style.ChildRounding = 8.0f;
        Style.FrameRounding = 5.0f;

        Color[ImGuiCol_Button] = ImColor(233, 87, 33, 255);
        Color[ImGuiCol_ButtonHovered] = ImColor(253, 107, 53, 255);
        Color[ImGuiCol_ButtonActive] = ImColor(213, 67, 13, 255);

        Color[ImGuiCol_FrameBg] = ImColor(54, 54, 54, 150);
        Color[ImGuiCol_FrameBgActive] = ImColor(42, 42, 42, 150);
        Color[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 150);

        Color[ImGuiCol_CheckMark] = ImColor(233, 87, 33, 255);

        Color[ImGuiCol_SliderGrab] = ImColor(233, 87, 33, 255);
        Color[ImGuiCol_SliderGrabActive] = ImColor(213, 67, 13, 255);

        Color[ImGuiCol_Header] = ImColor(233, 87, 33, 255);
        Color[ImGuiCol_HeaderHovered] = ImColor(253, 107, 53, 255);
        Color[ImGuiCol_HeaderActive] = ImColor(213, 67, 13, 255);

        break;
    }

    ImGui::Begin(
        "SCUT WORD DASH",
        NULL,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |//|
        //ImGuiWindowFlags_NoMove//请添加移动条谢谢
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );
    ImGui::SetWindowSize({ Screen_Width * 0.6f, Screen_Width * 0.4f });//设置窗口大小

    switch (TabStack.top()) {
        case TAB::Main:
            MAIN();
            break;
        case TAB::ProblemsSetSelect:
            PROBLEMSET();
            break;
        case TAB::Exercise:
            EXERCISE();
            break;
    }

    if (subWindow.size()) {//   更新更优秀的（应该）子窗口管理系统，使用了vector作为“可下标访问的stack”来实现
        for (int index = 0; index < subWindow.size(); index++) {
            if (index < subWindow.size() - 1) {
                if (subWindow[index].isDraw) {
                    subWindow[index].window(subWindow[index].isAct);
                }
            }
            else {
                subWindow[index].window(true);
            }
        }
    }

    //界面切换用的FRAMEMASK
    //drawRect(swicthFrameL, 0, swicthFrameR, height, ImColor(0, 0, 0, 255));
    ImGui::End();
    animeManager.updateAnime();
}
//<-行数

/*
* v1.0.0beta
版本特性：
1. 附带学术英语 Unit 1-5 的AWL词汇题题单
2. 任意选择题单进行随机抽题
3. 错题练习

* v1.0.1beta
BUG修复
1. 修复了在错题练习中提前结算时，错题数量会被吃一大半的bug
2. 改良了 “一键判题” 的逻辑


Todo（考完再说）:
支持所有题目同页显示（UI设计）

下册题库（没人整理）

学术英语和大学英语词汇练习二合一（得找人要这些题的照片）

自定义题库练习

支持任意数量选项

支持多选题

历史练习记录情况（有必要写这个吗？如果要的话，需要存什么东西？存 “题单名-题目序号-AC状态”？   以及UI设计）

题库管理：(逻辑以及UI设计)
- 题单重命名以及题目编号调整（感觉没必要调整编号，反正是随机抽题）
- 修改已有题单中的题目以及答案
- 新增自定义题单（但是谁会用这种东西练题啊，还得先自己导入题库才能练）
    -> 考虑怎么解决题单资源少的缺陷（似乎并没有什么好的解决方案） -> 考虑如何降低创建题单的成本
- 题目和选项显示支持MarkDown语法（似乎暂时并没有必要？cpp不支持Markdown渲染）

能不能想办法移植到手机上？
    -> 学习如何创建手机应用程序 -> 学习方便一点的框架 -> 重新设计相关UI

或者说网页版（否决该方案，我没服务器）（但是话又说回来，可以用github搞吧，开个小号什么的？还是算了）
*/