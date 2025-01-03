#include <corecrt_math.h>
#include "AnimeManager.h"
#include <ctime>

AnimeManager animeManager;

long double BounceOut(long attime, long dur) {
    long double px = 1.0 * attime / dur;
    if (px < 0.363636374)
        return 7.5625 * px * px;
    if (px < 0.727272749) {
        px = px - 0.545454562;
        return 7.5625 * px * px + 0.75;
    }
    if (px < 0.909090936) {
        px = px - 0.8181818;
        return 7.5625 * px * px + 0.9375;
    }
    px = px - 0.954545438;
    return 7.5625 * px * px + 0.984375;
}

long double motion(long st, long et, long ct, MotionType mt) {
    long dur = et - st;
    long attime = ct - st;
    long double px = 1.0 * attime / dur;
    long double period = 0;
    long double overshootOrAmplitude = 0;

    long double num;
    switch (mt) {// 只运行一次的switch没有break的必要
    case Linear:
        return px;
    case InSine:
        return 1.0 - long double(cos(px * acos(-1) / 2));
    case OutSine:
        return long double(sin(px * acos(-1) / 2));
    case InOutSine:
        return -0.5 * long double(cos(acos(-1) * px) - 1.0);
    case InQuad:
        return px * px;
    case OutQuad:
        return -px * (px - 2.0);
    case InOutQuad:
        if (px * 2 < 1.0)return 2 * px * px;
        return -0.5 * ((2 * px - 1.0) * (2 * px - 3.0) - 1);
    case InCubic:
        return px* px* px;
    case OutCubic:
        px = px - 1;
        return px * px * px + 1;
    case InOutCubic:
        if (px * 2 < 1.0) return 4 * px * px * px;
        return 0.5 * ((px * 2 - 2.0) * (px * 2 - 2.0) * (px * 2 - 2.0) + 2.0);
    case InQuart:
        return px * px * px * px;
    case OutQuart:
        px = px - 1.0;
        return -(px * px * px * px - 1.0);
    case InOutQuart:
        if (px * 2 < 1.0)return 8 * px * px * px * px;
        px = 2 * px - 2.0;
        return -0.5 * (px * px * px * px - 2.0);
    case InQuint:
        return px * px * px * px * px;
    case OutQuint:
        px = px - 1.0;
        return px * px * px * px * px + 1.0;
    case InOutQuint:
        if (px * 2 < 1.0)return 0.5 * px * px * px * px * px;
        px = px - 2.0;
        return 0.5 * (px * px * px * px * px + 2.0);
    case InExpo:
        if (attime != 0)return pow(2.0, 10.0 * (px - 1.0));
        return 0;
    case OutExpo:
        if (px == 1)return 1;
        return -pow(2.0, -10.0 * px) + 1.0;
    case InOutExpo:
        if (px == 0 || px == 1) return px;
        if (px * 2 < 1.0)return 0.5 * pow(2.0, 10.0 * (px - 1.0));
        return 0.5 * (-pow(2.0, -10.0 * (px - 1.0)) + 2.0);
    case InCirc:
        return -sqrt(1.0 - px * px) + 1.0;
    case OutCirc:
        px = px - 1.0;
        return sqrt(1.0 - px * px);
    case InOutCirc:
        if (px * 2 < 1.0) return -0.5 * (sqrt(1.0 - 4 * px * px) - 1.0);
        px = px * 2 - 2;
        return 0.5 * (sqrt(1 - px * px) + 1.0);
    case InElastic:
        if (px == 0 || px == 1)return px;
        period = dur * 0.3;
        num = 0.0;
        if (overshootOrAmplitude < 1.0) {
            overshootOrAmplitude = 1.0;
            num = period / 4.0;
        }
        else {
            num = period / (2 * acos(-1)) * asin(1.0 / overshootOrAmplitude);
        }
        return -(overshootOrAmplitude * pow(2.0, 10.0 * (px - 1)) * (sin(((px - 1) * dur - num) * (2 * acos(-1)) / period)));
    case OutElastic:
        if (px == 0 || px == 1) return px;
        if (period == 0) {
            period = dur * 0.3;
        }
        num = 0.0;
        if (overshootOrAmplitude < 1.0) {
            overshootOrAmplitude = 1.0;
            num = period / 4.0;
        }
        else {
            num = period / (2 * acos(-1)) * asin(1.0 / overshootOrAmplitude);
        }
        return overshootOrAmplitude * pow(2.0, -10 * px) * sin((px * dur - num) * (2 * acos(-1)) / period) + 1.0;
    case InOutElastic:
        if (px == 0 || px == 1) return px;
        px = px * 2;
        if (period == 0) {
            period = dur * 0.450000018;
        }
        num = 0.0;
        if (overshootOrAmplitude < 1.0) {
            overshootOrAmplitude = 1.0;
            num = period / 4.0;
        }
        else {
            num = period / (2 * acos(-1)) * asin(1.0 / overshootOrAmplitude);
        }
        if (px < 1.0)return -0.5 * (overshootOrAmplitude * pow(2.0, 10.0 * (px - 1)) * (sin(((px - 1) * dur - num) * (2 * acos(-1)) / period)));
        px = px - 1;
        return 0.5 * overshootOrAmplitude * pow(2.0, -10 * px) * sin((px * dur - num) * (2 * acos(-1)) / period) + 1.0;
    case InBack:
        overshootOrAmplitude = 1.70158;
        return px * px * ((overshootOrAmplitude + 1.0) * px - overshootOrAmplitude);
    case OutBack:
        overshootOrAmplitude = 1.70158;
        px = px - 1;
        return  px * px * ((overshootOrAmplitude + 1.0) * px + overshootOrAmplitude) + 1.0;
    case InOutBack:
        overshootOrAmplitude = 1.70158;
        if (px * 2 < 1.0) return 0.5 * (4 * px * px * ((overshootOrAmplitude * 1.525 + 1.0) * 2 * px - overshootOrAmplitude * 1.525));
        px = px * 2 - 2;
        return 0.5 * (px * px * (((overshootOrAmplitude * 1.525) + 1.0) * px + overshootOrAmplitude * 1.525) + 2.0);
    case InBounce:
        return 1- BounceOut(dur-attime, dur);
    case OutBounce:
        return BounceOut(attime, dur);
    case InOutBounce:
        if(2 * attime<dur)return 1 - BounceOut(dur - 2 * attime, dur);
        return 0.5f * BounceOut(attime * 2 - dur, dur) + 0.5f;
    }
}

AnimeManager::AnimeManager()
{
}

AnimeManager::~AnimeManager()
{
}
#include<iostream>
void AnimeManager::updateAnime(){
    long now = clock();
    std::vector<float*> del;
    for (auto& [value, anime] : animeList) {//  遍历管理的动画列表，逐一更新动画
        if (anime.startTime <= now && anime.startTime + anime.duringTime >= now) {//    如果处在动画播放的时间范围内，那么它就是一个正在播放的动画，要更新该动画
            if (anime.mType != CustomMotion) {//    如果没有指定是自定义缓动函数，那么直接照着默认的motiontype查表，然后更新动画就行（查表功能在motion()函数里通过switch实现）
                *anime.Value = anime.beginValue + (anime.endValue - anime.beginValue) * motion(anime.startTime, anime.startTime + anime.duringTime, now, anime.mType);
            }
            else {//                                否则就按照提供的缓动函数更新变量的值
                *anime.Value = anime.beginValue + (anime.endValue - anime.beginValue) * anime.customMotion(anime.startTime, anime.startTime + anime.duringTime, now);
            }
        }
        else if (now > anime.startTime + anime.duringTime) {//  如果该动画的结束时间已经过了，那么移除该动画
            del.push_back(value);
            if (anime.endFunc != nullptr) {//   如果该动画有提供 endFunc，那么执行这个endFunc
                anime.endFunc();
            }
        }
    }

    for (auto value:del) {//    清理动画，注意不能再for循环内部清理，会报错
        //std::cout <<animeList[value].startTime<< "事件已销毁]\n";
        *animeList[value].Value = animeList[value].endValue;
        animeList.erase(value);
    }
}

//     绑定的变量，      初始值，          结束值，       动画时长，     缓动类型，起始时间偏移量（可选参数），动画结束时执行的函数（可选参数），             自定义缓动类型（可选参数）
void AnimeManager::addAnime(
    float* Value, float beginValue, float endValue, long during, MotionType mt, long offset, std::function<void()> endFunc, std::function<long double(long, long, long)> customMotion
) {
    if (animeList.count(Value)) {// 如果出现过，就直接提前结束之前的缓动操作，并覆写
        if (animeList[Value].endFunc != nullptr) {
            animeList[Value].endFunc();
        }
        *Value = animeList[Value].endValue;
    }
    //std::cout << "添加新事件：" << clock() + offset << " " << during << "]\n";
    animeList[Value] = { clock() + offset,during,Value,beginValue,endValue,mt,endFunc,customMotion };
}

void AnimeManager::unBind(float* Value){
    if (animeList.count(Value)) {
        animeList.erase(Value);
    }
}