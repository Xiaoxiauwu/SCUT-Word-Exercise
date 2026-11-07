#pragma once
#include <algorithm>
#include <cmath>

namespace scut_ui {
// Derive a reference scale for typography from the current ImGui window.
// Font rasterization size is fixed; resizing changes geometry and draw scale only.
constexpr float FontAtlasSize = 20.0f;
constexpr float WindowAspectRatio = 1.5f;
constexpr float BaseWindowWidth = 960.0f;
inline float WindowScale(float width) { return width / BaseWindowWidth; }
struct WindowSize { float width, height; };
inline WindowSize ConstrainWindowSize(WindowSize current, WindowSize desired) {
    // Follow the dragged axis, so dragging a horizontal OR vertical edge works.
    const float dx = std::abs(desired.width - current.width);
    const float dy = std::abs(desired.height - current.height) * WindowAspectRatio;
    const float requestedHeight = dx >= dy ? desired.width / WindowAspectRatio : desired.height;
    // Integer 3x2 steps prevent ImGui's pixel rounding from breaking the ratio.
    const float units = (std::max)(220.0f, std::round(requestedHeight / 2.0f));
    return {3.0f * units, 2.0f * units};
}
struct LayoutSize { float width, height, referenceWidth; };
inline LayoutSize FitLayout(float clientWidth, float clientHeight) {
    const float scale = (std::max)(1.0f, (std::min)(clientWidth / 0.6f, clientHeight / 0.4f));
    return {scale * 0.6f, scale * 0.4f, scale};
}
}
