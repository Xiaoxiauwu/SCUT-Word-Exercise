#pragma once

#include "ImGui/imgui.h"
#include <string>

struct ID3D11Device;

namespace scut_ui {
void InitializeLatexRenderer(ID3D11Device* device);
void ShutdownLatexRenderer();
float MeasureMarkdown(const std::string& source, float width, float fontSize, const std::string& baseDirectory = ".");
bool RenderMarkdown(const std::string& source, ImVec2 position, float width, float fontSize,
                    const std::string& baseDirectory = ".");
}
