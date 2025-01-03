#pragma once
#pragma once
#define MAX(a,b) ((a>b)?(a):(b))
#define MIN(a,b) ((a<b)?(a):(b))
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imconfig.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imstb_rectpack.h"
#include "ImGui/imstb_textedit.h"
#include "ImGui/imstb_truetype.h"
//#include "ImGui/improfx_control_base.h"

#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")

#define MAX_WIDTH 21
#define MAX_HEIGHT 21

#include "Font.h"
//#include "Tool.h"
//#include"data.h"
#include <tchar.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <stack>
#include <queue>
#include <set>
#include <random>
#include <ctime>
#include <algorithm>
#include <functional>
#include <codecvt>
#include <sstream>
#include <chrono>
#include "json\json.h"
#include "base64.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ImFileDialog.h"

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

//记得注释掉这句，不然没法debug
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 
//using namespace std;

#define IMVEC2_ADD2(p1, p2) ImVec2((p1).x + (p2).x, (p1).y + (p2).y)
#define IMVEC2_SUB2(p1, p2) ImVec2((p1).x - (p2).x, (p1).y - (p2).y)
#define IMVEC2_MUL1(p, v)   ImVec2((p).x * (v), (p).y * (v))
#define IMVEC2_MUL2(p1, p2) ImVec2((p1).x * (p2).x, (p1).y * (p2).y)
#define IMVEC4_CVT_COLU32(COLOR)      IM_COL32(ImU32((COLOR).x * 255.0f), ImU32((COLOR).y * 255.0f), ImU32((COLOR).z * 255.0f), ImU32((COLOR).w * 255.0f))