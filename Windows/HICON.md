HICON 与 HBITMAP 相互转换

## 将BMP转换成ICON图标

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
