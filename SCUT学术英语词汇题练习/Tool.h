#pragma once
#include "GLOBAL.h"

//using namespace std;

namespace encode
{
	std::string wideToMulti(std::wstring_view sourceStr, UINT pagecode);
	std::string wideToUtf8(std::wstring_view sourceWStr);
	std::string wideToOme(std::wstring_view sourceWStr);
	std::string string_To_UTF8(const std::string& str);
	std::string UTF8_To_String(const std::string& s);
	std::wstring UTF8_To_Wstring(const std::string& s);
}

std::ostream& operator<<(std::ostream& os, std::wstring_view str);

void setFont(ImFont* F);

void drawRect(float x, float y, float w, float h, ImVec4 color, bool noRelated = 0);
void drawText(float x, float y, float size, ImVec4 color, string text, bool iscenter = 1);
ImVec2 getTextSize_autoNewLine(float size, string text, float width);

void drawText_autoNewLine(float x, float y, float size, ImVec4 color, std::string text, float width, std::string tip = "", bool iscenter = 1);
std::string fillWords(std::vector<std::string>& words, int bg, int ed, int maxWidth, bool lastLine = false);
std::vector<std::string> fullJustify(std::vector<std::string>& words, int maxWidth);
void drawText_autoNewLine_English(float x, float y, float size, ImVec4 color, std::string text, float width, std::string tip, bool iscenter);
void drawText_RAINBOW(float x, float y, float size, std::string text, bool iscenter = 1, std::string tip = "");
ImVec2 getTextSize(float size, std::string text);

template<typename T, typename T2, typename T3>
T constrict(T v, T2 minv, T3 maxv) {
	return MAX(MIN(maxv, v), minv);
};

template<typename T>
bool sgn(T v) {
	if (abs(v) < 1e-9)return 0;
	return v > 0;
}

template<typename T>
long double dis(T a, T b) {
	long double d = 0;
	for (int i = 0; i < a.size(); i++) {
		d += sqrtl((a[i] - b[i]) * (a[i] - b[i]));
	}
	return d;
}

std::vector<ImVec2> prettyLine(ImVec2 a, ImVec2 b);
std::vector<ImVec2> prettyCirc(ImVec2 a, ImVec2 b);
std::vector<ImVec2> prettyCircF(ImVec2 a, ImVec2 b);


//	计分板

class ScoreBoard {
public:
	ScoreBoard();
	~ScoreBoard();
	void Reset();
	int GetTime();
	int calcScore();
	void step();
	void regret();
	void hint();
	void submit();
	void lock();
	void unlock();
private:
	int baseTime;
	int stepCount;
	int regretCount;
	int hintCount;
	int submitFailedCount;
	bool locked;
	int _score;
	int _time;
};

std::string sec2time(int sec);

//	JSON文件操作与简易加密解密

bool checkFile(const wstring& filename);
void writeFileFromString(const std::string& filename, const std::string& body);
std::string readFileIntoString(const char* filename);
Json::Value readJsonFromString(const std::string& mystr);
Json::Value readJsonFile(const std::string& filename);
void writeJsonFile(const std::string& filename, const Json::Value& root);
std::string ENCODE(std::string &origin);
std::string DECODE(std::string encoded);

std::string JSONToEJSON(Json::Value& R);
Json::Value EJSONToJSON(std::string& ejson);

std::string trans(std::string s);
extern std::mt19937 myrand;
int rnd(int l, int r);
long long rnd(long long l, long long r);
int norm_rnd(int range, double k);

ImVec2 GetMousePos_WithRestrict(float Max_x, float Max_y, float Min_x = 0, float Min_y = 0, bool noRelated = 0);
ImVec2 GetMousePos_InWindow();
std::ostream& operator<<(std::ostream& COUT, ImVec2 vec);
std::ostream& operator<<(std::ostream& COUT, ImVec4 vec);
bool PaintBoard(const char* label, const ImVec2& size_arg);

//	图像加载器

struct IMAGE_{
	void* image;
	int w, h;
	unsigned char* data;
};

class ImageLoader {
public:
	ImageLoader();
	~ImageLoader();
	void Init(ID3D11Device* pd3dDevice_);
	void* LoadImageToTexture(std::string index,const char* path);
	void ScaleImage(std::string baseImg, std::string saveAs, int w, int h);
	void remove(std::string index);
	std::map<std::string, IMAGE_> IMG;
	float percent = 0;
private:
	ID3D11Device* pd3dDevice;
};

class AudioLoader {
public:
	static AudioLoader& get() {
		static AudioLoader instance;
		return instance;
	}
	~AudioLoader();
	void addButton(std::wstring path, std::string as);
	void addBGM(std::wstring path, std::string as);
	void playButton(std::string as);
	void playBGM(std::string as);

	void SetButtonVolume(int v);
	void SetBGMVolume(int v);
private:
	AudioLoader();
	std::string playing="";
	std::map<std::string, std::wstring> BGMs;
	std::map<std::string, std::wstring> Buttons;
};

bool ImageButton(const char* label, const ImVec2& size_arg, ImTextureID tex, bool noDefault = 0, ImVec2 size_tex = { 0,0 }, int ID = -1);

double CIEDE2000(float R1, float G1, float B1, float R2, float G2, float B2);