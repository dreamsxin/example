## HICON 与 HBITMAP 相互转换

### 将BMP转换成ICON图标

方法一、
```c++
HICON HICONFromCBitmap( CBitmap& bitmap)
{
  BITMAP bmp;
  bitmap.GetBitmap(&bmp);
  
  HBITMAP hbmMask = ::CreateCompatibleBitmap(::GetDC(NULL), 
    bmp.bmWidth, bmp.bmHeight);
  
  ICONINFO ii = {0};
  ii.fIcon = TRUE;
  ii.hbmColor = bitmap;
  ii.hbmMask = hbmMask;
  
  HICON hIcon = ::CreateIconIndirect(&ii);//一旦不再需要，注意用DestroyIcon函数释放占用的内存及资源
  ::DeleteObject(hbmMask);
  
  return hIcon;
}
```
方法二、
```c++
#include <gdiplus.h>
#pragma comment(lib,"GdiPlus.lib")
 
HICON CreateIcon(HBITMAP hBitmap)
{
 Gdiplus::Bitmap* pTmpBitmap=Gdiplus::Bitmap::FromHBITMAP(hBitmap,NULL);
 HICON hIcon=NULL;
 pTmpBitmap->GetHICON(&hIcon);
 delete pTmpBitmap;
 return hIcon;
}
```
## HICON转HBITMAP

利用GetIconInfo函数获取ICONINFO信息, 再将其复制到位图当中去, 代码如下:
```c++
HBITMAP ConvertIconToBitmap(HICON  hIcon)
{ 
  HBITMAP   hBmp; 
  BITMAP   bmp; 
  CDC   bmpDC; 
  CDC   iconDC; 
  ICONINFO         csII; 
  int bRetValue   =   ::GetIconInfo(hIcon,   &csII); 
  if   (bRetValue   ==   FALSE)   return   NULL; 
  bmpDC.Attach(::GetDC(NULL)); 
  iconDC.CreateCompatibleDC(&bmpDC); 
  if   (::GetObject(csII.hbmColor,   sizeof(BITMAP),   &bmp)) 
  { 
    DWORD       dwWidth   =   csII.xHotspot*2; 
    DWORD       dwHeight   =   csII.yHotspot*2; 
    hBmp=   ::CreateBitmap(dwWidth,   dwHeight,   bmp.bmPlanes,   
      bmp.bmBitsPixel,   NULL);
    iconDC.SelectObject(csII.hbmColor); 
    bmpDC.SelectObject(hBmp); 
    bmpDC.BitBlt(0,0,dwWidth,dwHeight,&iconDC,0,0,SRCCOPY); 
    return   hBmp; 
  }
  return NULL;
}
```

还有一种方法就是先创建一个兼容位图, 然后通过API函数::DrawIcon()复制上去, CopyImage函数可以用来替换创建兼容DC以后的那些骤了

### 例子

```c++
#include <windows.h>
#include <gdiplus.h>
#include <sstream>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

// 将数字转换为字符串
std::wstring NumberToString(int number) {
    std::wstringstream ss;
    ss << number;
    return ss.str();
}

// 将字符串绘制到位图上
HICON CreateHICONFromNumber(int number) {
    // 初始化GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 创建位图
    Bitmap* bitmap = new Bitmap(32, 32, PixelFormat32bppARGB);
    Graphics* graphics = new Graphics(bitmap);

    // 设置字体和颜色
    FontFamily *fontFamily = new FontFamily(L"Arial");
    Font *font = new Gdiplus::Font(fontFamily, 12, FontStyleRegular, UnitPixel);
    SolidBrush *brush = new SolidBrush(Color(255, 0, 0, 0)); // 红色

    // 将数字转换为字符串
    std::wstring numberStr = NumberToString(number);

    // 绘制字符串到位图
    RectF layoutRect(0, 0, 32, 32);
    graphics->DrawString(numberStr.c_str(), -1, font, layoutRect, NULL, brush);

    // 将位图转换为HICON
    HICON hIcon = NULL;
    bitmap->GetHICON(&hIcon);

    // 清理GDI+
    delete bitmap;
    delete graphics;
    delete font;
    delete brush;
    GdiplusShutdown(gdiplusToken);

    return hIcon;
}

int main() {
    int number = 123;
    HICON hIcon = CreateHICONFromNumber(number);

    // 使用HICON，例如将其设置为窗口图标
    HWND hwnd = GetDesktopWindow();
    (HICON)::SendMessageW(hwnd, WM_SETICON, (WPARAM)ICON_BIG,
        (LPARAM)hIcon);
    (HICON)::SendMessageW(hwnd, WM_SETICON, (WPARAM)ICON_SMALL,
        (LPARAM)hIcon);

    // 等待一段时间，以便可以看到图标
    Sleep(15000);

    // 释放HICON
    DestroyIcon(hIcon);

    return 0;
}
```
```c++
#include <windows.h>
#include <gdiplus.h>
#include <sstream>

using namespace Gdiplus;

#pragma comment (lib,"Gdiplus.lib")

// 将数字转换为字符串
std::wstring NumberToString(int number) {
    std::wstringstream ss;
    ss << number;
    return ss.str();
}

// 从文件加载ICO图标
HICON LoadIconFromFile(const std::wstring& filePath) {
    return (HICON)LoadImage(NULL, filePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}

// 在图标上绘制数字
HICON DrawNumberOnIcon(HICON hIcon, int number) {
    // 初始化GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 获取图标信息
    ICONINFO iconInfo;
    GetIconInfo(hIcon, &iconInfo);

    // 创建位图
    Bitmap* bitmap = Bitmap::FromHBITMAP(iconInfo.hbmColor, NULL);

    // 创建Graphics对象
    Graphics* graphics = new Graphics(bitmap);

    // 设置字体和颜色
    FontFamily* fontFamily = new FontFamily(L"Arial");
    Font* font = new Gdiplus::Font(fontFamily, 12, FontStyleRegular, UnitPixel);
    SolidBrush* brush = new SolidBrush(Color(255, 0, 0, 0)); // 红色

    // 将数字转换为字符串
    std::wstring numberStr = NumberToString(number);

    // 计算字符串位置
    RectF layoutRect(0, 0, 32, 32); // 假设图标大小为32x32
    StringFormat* stringFormat = new StringFormat;
    stringFormat->SetAlignment(StringAlignmentCenter);
    stringFormat->SetLineAlignment(StringAlignmentCenter);

    // 绘制字符串
    graphics->DrawString(numberStr.c_str(), -1, font, layoutRect, stringFormat, brush);

    // 将位图转换为HICON
    HICON hNewIcon = NULL;
    bitmap->GetHICON(&hNewIcon);

    // 清理资源
    delete bitmap;
    delete graphics;
    delete font;
    delete brush;
    delete stringFormat;
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    GdiplusShutdown(gdiplusToken);

    return hNewIcon;
}

int main() {
    std::wstring filePath = L"your_icon.ico"; // 替换为你的ICO文件路径
    int number = 123;

    // 从文件加载ICO图标
    HICON hIcon = LoadIconFromFile(filePath);

    // 在图标上绘制数字
    HICON hNewIcon = DrawNumberOnIcon(hIcon, number);

    // 使用HICON，例如将其设置为窗口图标
    HWND hwnd = GetDesktopWindow();
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hNewIcon);

    // 等待一段时间，以便可以看到图标
    Sleep(5000);

    // 释放HICON
    DestroyIcon(hNewIcon);
    DestroyIcon(hIcon);

    return 0;
}
```

## 保存为 png
```c++
#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <sstream>

using namespace Gdiplus;

#pragma comment (lib,"Gdiplus.lib")

// 将数字转换为字符串
std::wstring NumberToString(int number) {
    std::wstringstream ss;
    ss << number;
    return ss.str();
}

// 从文件加载ICO图标
HICON LoadIconFromFile(const std::wstring& filePath) {
    return (HICON)LoadImage(NULL, filePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}

// 在图标上绘制数字
HICON DrawNumberOnIcon(HICON hIcon, int number) {
    // 初始化GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 获取图标信息
    ICONINFO iconInfo;
    GetIconInfo(hIcon, &iconInfo);

    // 创建位图
    Bitmap* bitmap = Bitmap::FromHBITMAP(iconInfo.hbmColor, NULL);

    // 创建Graphics对象
    Graphics* graphics = new Graphics(bitmap);

    // 设置字体和颜色
    FontFamily* fontFamily = new FontFamily(L"Arial");
    Font* font = new Gdiplus::Font(fontFamily, 12, FontStyleRegular, UnitPixel);
    SolidBrush* brush = new SolidBrush(Color(255, 0, 0, 0)); // 红色

    // 将数字转换为字符串
    std::wstring numberStr = NumberToString(number);

    // 计算字符串位置
    RectF layoutRect(0, 0, 48, 48); // 假设图标大小为32x32
    StringFormat* stringFormat = new StringFormat;
    stringFormat->SetAlignment(StringAlignmentCenter);
    stringFormat->SetLineAlignment(StringAlignmentFar);

    // 绘制字符串
    graphics->DrawString(numberStr.c_str(), -1, font, layoutRect, stringFormat, brush);

    // 将位图转换为HICON
    HICON hNewIcon = NULL;
    bitmap->GetHICON(&hNewIcon);

    // 清理资源
    delete bitmap;
    delete graphics;
    delete font;
    delete brush;
    delete stringFormat;
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    GdiplusShutdown(gdiplusToken);

    return hNewIcon;
}

static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;  // Failure

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;  // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

// 将HICON保存为ICO文件
BOOL SaveHICONToFile(HICON hIcon, const std::wstring& filePath) {
    // 初始化GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 获取图标信息
    ICONINFO iconInfo;
    GetIconInfo(hIcon, &iconInfo);

    // 创建位图
    Bitmap* bitmap = Bitmap::FromHBITMAP(iconInfo.hbmColor, NULL);

    // 创建ICO文件
    CLSID clsid;
    GetEncoderClsid(L"image/png", &clsid);
    if (clsid != CLSID_NULL) {
        Status status = bitmap->Save(filePath.c_str(), &clsid);
        std::cout << status << std::endl;
    }
    // 清理资源
    delete bitmap;
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    GdiplusShutdown(gdiplusToken);

    return true;
}

int main() {
    std::wstring filePath = L"serial48.ico"; // 替换为你的ICO文件路径
    int number = 123;

    // 从文件加载ICO图标
    HICON hIcon = LoadIconFromFile(filePath);

    // 在图标上绘制数字
    HICON hNewIcon = DrawNumberOnIcon(hIcon, number);

    // 保存HICON为ICO文件
    BOOL result = SaveHICONToFile(hNewIcon, L"D:\\mozilla-source\\DemoConsoleApplication1\\x64\\Debug\\testserial48.png");

    // 使用HICON，例如将其设置为窗口图标
   // HWND hwnd = GetDesktopWindow();
    //SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hNewIcon);



    // 等待一段时间，以便可以看到图标
    //Sleep(5000);

    // 释放HICON
    DestroyIcon(hNewIcon);
    DestroyIcon(hIcon);

    return 0;
}
```
