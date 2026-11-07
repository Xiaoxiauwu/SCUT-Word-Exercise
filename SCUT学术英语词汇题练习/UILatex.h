#pragma once

#include "ImGui/imgui.h"
#include <string>

struct ID3D11Device;

namespace scut_ui {
void InitializeLatexRenderer(ID3D11Device* device);
void ShutdownLatexRenderer();
bool IsLatexFormula(const std::string& text);
ImVec2 MeasureLatexFormula(const std::string& text, float fontSize);
bool DrawLatexFormula(const std::string& text, ImVec2 position, float fontSize);
}
