#include "UIInternal.h"

namespace scut_ui {

void Initialize(ID3D11Device* g_pd3dDevice) {
    InitializeLatexRenderer(g_pd3dDevice);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigViewportsNoDecoration = true;
    io.ConfigWindowsResizeFromEdges = true;
    io.IniFilename = nullptr;
    ImFontConfig Font_cfg;
    // Chinese glyphs do not need the default 3x horizontal oversampling.
    Font_cfg.OversampleH = 1;
    Font_cfg.OversampleV = 1;
    io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;

    //ImFont* Font = io.Fonts->AddFontFromFileTTF("..\\ImGui Tool\\Font.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    //Font = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, Screen_Width / 1024 * 18.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    //Font_Big = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, Screen_Width / 1024 * 24.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());

    std::vector<std::wstring> tmp;
    if ((tmp = GetAllFiles(L"res", L"\\*.ttf")).size() || 
        (tmp = GetAllFiles(L"res", L"\\*.otf")).size() ||
        (tmp = GetAllFiles(L"res", L"\\*.ttc")).size()
    ) {//优先抽ttf
        Font = io.Fonts->AddFontFromFileTTF(encode::wideToOme(std::wstring(L"res\\") + tmp[0]).c_str(), FontAtlasSize, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    } else {
        //  防止用户误删字体文件，用户换别的字体文件的话，不一定保证字库包含中文，所以用自己的
        Font_cfg.FontDataOwnedByAtlas = false;
        Font = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, FontAtlasSize, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    }
    
    //Font_Big = io.Fonts->AddFontFromFileTTF("Source Han Sans & Saira Hybrid-Regular.ttf", Screen_Width / 1024 * 24.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());

    setFont(Font);
    setTextScale(0.85f);

    //  初始化图片资源
    imageLoader->Init(g_pd3dDevice);
    
    //注册并绑定图片
    //imageLoader->LoadImageToTexture("drawpix", "res/drawpix.png");
    //imageLoader->LoadImageToTexture("eraser", "res/eraser.png");
    imageLoader->LoadImageToTexture("edit", "res/edit.png");

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


    refresh();

    std::string rawInfo = readFileIntoString("userinfo.dat");
    Json::Value R = readJsonFromString(DECODE(rawInfo));

    if (R == Json::nullValue) {
        R["WA"] = Json::arrayValue;
    }
    
    userInfo = R;
    NormalizeWrongAnswers(userInfo);
    exercise.collectWrongAnswers = userInfo["preferences"].get("collectWrongAnswers", true).asBool();
    exercise.removeCorrectWrongAnswers = userInfo["preferences"].get("removeCorrectWrongAnswers", false).asBool();

    //std::cout << problemSets.sets << "\n";
    
    while (TabStack.size()) TabStack.pop();
    TabStack.push(Main);

    //  注册播放用音频
    //AudioLoader::get().addButton(L"res\\Tap1.mp3", "TAP");
    //AudioLoader::get().addButton(L"res\\Tap5.mp3", "SELECT");
    //AudioLoader::get().addBGM(L"res\\baba.mp3", "GAME");
}

void UpdateScreenRect() {
    ImVec2 size = ImGui::GetWindowSize();
    if (size.x <= 0.0f || size.y <= 0.0f) {
        size = ImVec2(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)),
                      static_cast<float>(GetSystemMetrics(SM_CYSCREEN)));
    }
    const auto layout = FitLayout(size.x, size.y);
    Screen_Width = layout.referenceWidth;
    Screen_Height = size.y;
    width = size.x;
    height = size.y;
    ImGui::GetIO().FontGlobalScale = 16.0f * WindowScale(width) / FontAtlasSize;
    // Refresh ImGui's cached base/current font sizes in this frame, before widgets.
    ImGui::PushFont(Font);
    ImGui::PopFont();
    buttonwidth = width / 4;
    baseoffset = Screen_Width / 1024 * 24.0f + width / 25;

    gap = height / 80;
    //cout << 12345;
}

} // namespace scut_ui
