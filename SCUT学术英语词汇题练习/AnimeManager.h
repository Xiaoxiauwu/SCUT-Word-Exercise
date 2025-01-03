#pragma once
#include<vector>
#include<map>
#include<functional>
enum MotionType {
    Linear,
    OutSine,
    InSine,
    InOutSine,
    InQuad,
    OutQuad,
    InOutQuad,
    InCubic,
    OutCubic,
    InOutCubic,
    InQuart,
    OutQuart,
    InOutQuart,
    InQuint,
    OutQuint,
    InOutQuint,
    InExpo,
    OutExpo,
    InOutExpo,
    InCirc,
    OutCirc,
    InOutCirc,
    InElastic,
    OutElastic,
    InOutElastic,
    InBack,
    OutBack,
    InOutBack,
    InBounce,
    OutBounce,
    InOutBounce,
    CustomMotion
};
struct ANIME {
    //  以time(0)为标准，所以开long
	long startTime;     //  起始时间
    long duringTime;    //  动画时长
	float* Value;       //  绑定的float变量
    float beginValue;   //  起始值
    float endValue;     //  结束值
    MotionType mType;   //  使用的缓动动画类型
    std::function<void()> endFunc = nullptr;//  该动画结束后执行的函数
    std::function<long double(long, long, long)> customMotion = nullptr;//  自定义缓动类型
};

class AnimeManager {
public:
	AnimeManager();
	~AnimeManager();
	void updateAnime();
    void addAnime(float* Value, float beginValue, float endValue, long during, MotionType mt, long offset = 0, std::function<void()> endFunc = nullptr, std::function<long double(long, long, long)> customMotion = nullptr);
    void unBind(float* Value);
private:
    std::map<float*, ANIME> animeList;
};

extern AnimeManager animeManager;