#include "UILatex.h"

#include <d3d11.h>
#include <gdiplus.h>
#undef transparent
#undef TRANSPARENT
#define MICROTEX_STATIC

#include <microtex.h>
#undef min
#undef max
#include <atom/atom_basic.h>
#include <filesystem>
#include <cctype>
#include <memory>
#include <unordered_map>


#include "graphic_win32.h"

#pragma comment(lib, "gdiplus.lib")

namespace scut_ui {
namespace {
struct FormulaTexture {
    ID3D11ShaderResourceView* view = nullptr;
    int width = 0;
    int height = 0;
};

ID3D11Device* g_device = nullptr;
ULONG_PTR g_gdiplusToken = 0;
bool g_ready = false;
std::unordered_map<std::string, FormulaTexture> g_cache;

std::string ResourcePath(const char* name) {
    return (std::filesystem::path("res") / "latex" / name).string();
}

bool EnsureReady() {
    if (g_ready || !g_device) return g_ready;
    Gdiplus::GdiplusStartupInput input;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &input, nullptr) != Gdiplus::Ok) return false;
    try {
        const microtex::FontSrcFile font(ResourcePath("latinmodern-math.clm1"), ResourcePath("latinmodern-math.otf"));
        microtex::MicroTeX::init(font);
        microtex::PlatformFactory::registerFactory("gdi", std::make_unique<microtex::PlatformFactory_gdi>());
        microtex::PlatformFactory::activate("gdi");
        // Use CSS RGB values consistently, including names absent from MicroTeX.
        const std::pair<const char*, unsigned int> colors[] = {
            {"red",0xFF0000},{"green",0x008000},{"blue",0x0000FF},{"pink",0xFFC0CB},
            {"lightgreen",0x90EE90},{"lightblue",0xADD8E6},{"orange",0xFFA500},{"cyan",0x00FFFF},
            {"magenta",0xFF00FF},{"purple",0x800080},{"brown",0xA52A2A},{"olive",0x808000},
            {"lime",0x00FF00},{"teal",0x008080},{"violet",0xEE82EE},{"black",0x000000},
            {"gray",0x808080},{"darkgray",0xA9A9A9},{"lightgray",0xD3D3D3},{"white",0xFFFFFF},
            {"maroon",0x800000},{"crimson",0xDC143C},{"orangered",0xFF4500},{"coral",0xFF7F50},
            {"salmon",0xFA8072},{"khaki",0xF0E68C},{"gold",0xFFD700},{"goldenrod",0xDAA520},
            {"tan",0xD2B48C},{"seagreen",0x2E8B57},{"forestgreen",0x228B22},{"yellowgreen",0x9ACD32},
            {"greenyellow",0xADFF2F},{"limegreen",0x32CD32},{"springgreen",0x00FF7F},{"aquamarine",0x7FFFD4},
            {"turquoise",0x40E0D0},{"cadetblue",0x5F9EA0},{"skyblue",0x87CEEB},{"cornflowerblue",0x6495ED},
            {"royalblue",0x4169E1},{"midnightblue",0x191970},{"navy",0x000080},{"indigo",0x4B0082},
            {"blueviolet",0x8A2BE2},{"darkorchid",0x9932CC},{"fuchsia",0xFF00FF},{"orchid",0xDA70D6},
            {"plum",0xDDA0DD},{"linen",0xFAF0E6},{"beige",0xF5F5DC}
        };
        for (const auto& entry : colors)
            microtex::ColorAtom::defineColor(entry.first, 0xFF000000u | entry.second);
        g_ready = true;
    } catch (...) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    return g_ready;
}

// Convert the scoped declaration \\color{c} into a color atom spanning the
// remainder of its TeX group. Explicit nested groups retain their scope.
std::string NormalizeColors(const std::string& tex, size_t& pos, bool grouped = false) {
    std::string out;
    while (pos < tex.size()) {
        if (tex[pos] == '}' && grouped) { ++pos; break; }
        if (tex.compare(pos, 6, "\\color") == 0 &&
            (pos + 6 == tex.size() || !std::isalpha(static_cast<unsigned char>(tex[pos + 6])))) {
            size_t p = pos + 6;
            while (p < tex.size() && std::isspace(static_cast<unsigned char>(tex[p]))) ++p;
            if (p < tex.size() && tex[p] == '{') {
                const size_t end = tex.find('}', p + 1);
                if (end != std::string::npos) {
                    const std::string color = tex.substr(p + 1, end - p - 1);
                    pos = end + 1;
                    out += "\\fgcolor{" + color + "}{" + NormalizeColors(tex, pos, grouped) + "}";
                    return out;
                }
            }
        }
        if (tex[pos] == '\\' && pos + 1 < tex.size()) {
            out += tex[pos++]; out += tex[pos++];
        } else if (tex[pos] == '{') {
            ++pos;
            out += "{" + NormalizeColors(tex, pos, true) + "}";
        } else out += tex[pos++];
    }
    return out;
}

FormulaTexture BuildTexture(const std::string& tex, float size) {
    FormulaTexture result;
    if (!EnsureReady()) return result;
    size_t position = 0;
    const std::string normalized = NormalizeColors(tex, position);
    std::unique_ptr<microtex::Render> render(microtex::MicroTeX::parse(normalized, 0, size, size / 3.f,
        microtex::white, false));
    if (!render || render->getWidth() <= 0 || render->getHeight() <= 0) return result;
    const int padding = (std::max)(4, static_cast<int>(std::ceil(size * 0.35f)));
    const int w = render->getWidth() + padding * 2;
    const int h = render->getHeight() + padding * 2;
    Gdiplus::Bitmap bitmap(w, h, PixelFormat32bppARGB);
    Gdiplus::Graphics graphics(&bitmap);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    microtex::Graphics2D_win32 canvas(&graphics);
    render->setForeground(microtex::white);
    // MicroTeX draw() receives the top-left origin of the formula box.
    render->draw(canvas, padding, padding);

    Gdiplus::Rect rect(0, 0, w, h);
    Gdiplus::BitmapData data{};
    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) != Gdiplus::Ok) return result;
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = w; desc.Height = h; desc.MipLevels = 1; desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT; desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA sub{};
    sub.pSysMem = data.Scan0; sub.SysMemPitch = static_cast<UINT>(data.Stride);
    ID3D11Texture2D* texture = nullptr;
    if (FAILED(g_device->CreateTexture2D(&desc, &sub, &texture))) { bitmap.UnlockBits(&data); return result; }
    D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
    srv.Format = desc.Format; srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D; srv.Texture2D.MipLevels = 1;
    g_device->CreateShaderResourceView(texture, &srv, &result.view);
    texture->Release();
    bitmap.UnlockBits(&data);
    result.width = w; result.height = h;
    return result;
}
}

void InitializeLatexRenderer(ID3D11Device* device) { g_device = device; EnsureReady(); }

void ShutdownLatexRenderer() {
    for (auto& item : g_cache) if (item.second.view) item.second.view->Release();
    g_cache.clear();
    if (g_ready) microtex::MicroTeX::release();
    g_ready = false; g_device = nullptr;
    if (g_gdiplusToken) { Gdiplus::GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }
}

bool IsLatexFormula(const std::string& text) { return text.size() >= 2 && text.front() == '$' && text.back() == '$'; }

ImVec2 MeasureLatexFormula(const std::string& text, float fontSize) {
    if (!IsLatexFormula(text) || !EnsureReady()) return {};
    const std::string key = text + "\n" + std::to_string(fontSize);
    auto it = g_cache.find(key);
    if (it == g_cache.end()) it = g_cache.emplace(key, BuildTexture(text.substr(text.size() > 3 && text[1] == '$' ? 2 : 1,
        text.size() > 3 && text[text.size() - 2] == '$' ? text.size() - 4 : text.size() - 2), fontSize)).first;
    return ImVec2(static_cast<float>(it->second.width), static_cast<float>(it->second.height));
}

bool DrawLatexFormula(const std::string& text, ImVec2 position, float fontSize) {
    const ImVec2 size = MeasureLatexFormula(text, fontSize);
    if (size.x <= 0 || size.y <= 0) return false;
    const std::string key = text + "\n" + std::to_string(fontSize);
    const auto it = g_cache.find(key);
    if (it == g_cache.end() || !it->second.view) return false;
    ImGui::GetWindowDrawList()->AddImage(it->second.view, position,
        ImVec2(position.x + size.x, position.y + size.y));
    return true;
}
}
