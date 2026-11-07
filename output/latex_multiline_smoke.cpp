
#define NOMINMAX
#define MICROTEX_STATIC
#include <windows.h>
#include <gdiplus.h>
#undef transparent
#undef TRANSPARENT
#include <microtex.h>
#include "graphic_win32.h"
#include <memory>
#include <cassert>
#include <iostream>
int main() {
 ULONG_PTR token=0; Gdiplus::GdiplusStartupInput input;
 assert(Gdiplus::GdiplusStartup(&token,&input,nullptr)==Gdiplus::Ok);
 microtex::FontSrcFile font("SCUT学术英语词汇题练习/res/latex/latinmodern-math.clm1", "SCUT学术英语词汇题练习/res/latex/latinmodern-math.otf");
 microtex::MicroTeX::init(font);
 microtex::PlatformFactory::registerFactory("gdi",std::make_unique<microtex::PlatformFactory_gdi>());
 microtex::PlatformFactory::activate("gdi");
 {
 std::unique_ptr<microtex::Render> render(microtex::MicroTeX::parse(R"(
f\left(
   \left[
 \frac{
   1+\left\{x,y\right\}
 }{
   \left(
      \frac{x}{y}+\frac{y}{x}
   \right)
   \left(u+1\right)
 }+a
   \right]^{3/2}
\right)
)",0,24,8,microtex::white,false));
 assert(render && render->getWidth()>0 && render->getHeight()>0);
 Gdiplus::Bitmap bitmap(render->getWidth()+32,render->getHeight()+32,PixelFormat32bppARGB);
 Gdiplus::Graphics graphics(&bitmap);
 microtex::Graphics2D_win32 canvas(&graphics);
 render->draw(canvas,16,16);
 std::cout<<"Multiline complex formula rendered: "<<render->getWidth()<<"x"<<render->getHeight()<<"\n";
 }
 microtex::MicroTeX::release(); Gdiplus::GdiplusShutdown(token);
}
