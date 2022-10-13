#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <string>
#include <Windows.h>
#include <CommCtrl.h>

using namespace std;

HWND window = nullptr;
HWND trackBar = nullptr;
HWND progressBar = nullptr;
HWND staticText = nullptr;
WNDPROC defWndProc = nullptr;

static HBITMAP hBitmapThumb, hBitmapBar;
static BITMAP bm;

LRESULT OnWindowClose(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PostQuitMessage(0);
    return CallWindowProc(defWndProc, hwnd, message, wParam, lParam);
}

LRESULT OnTrackBarChanged(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT value = SendMessage(trackBar, TBM_GETPOS, 0, 0);
    SendMessage(progressBar, PBM_SETPOS, value, 0);
    SendMessage(staticText, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(to_wstring(value).c_str()));
    return CallWindowProc(defWndProc, hwnd, message, wParam, lParam);
}

void DrawBitmapTransparent(
    HDC hDCDest,
    int nXDest,
    int nYDest,
    int nBitmapWidth,
    int nBitmapHeight,
    HBITMAP hBitmap,
    int nXSrc,
    int nYSrc,
    int nTransparentColor)
{
    HDC hDCSrc;
    HBITMAP hBitmapOld;
    HDC hDCMask;
    HBITMAP hBitmapMask;
    HBITMAP hBitmapMaskOld;
    HDC hDCMem;
    HBITMAP hBitmapMem;
    HBITMAP hBitmapMemOld;
    int nBkColorOld;
    int nTextColorOld;
    BITMAP bm;

    GetObject( hBitmap, sizeof( BITMAP ), &bm );

    if (!nBitmapWidth) {
        nBitmapWidth = bm.bmWidth;
    }

    if (!nBitmapHeight) {
        nBitmapHeight = bm.bmHeight;
    }

    hDCSrc = CreateCompatibleDC( hDCDest );
    hBitmapOld = reinterpret_cast<HBITMAP>(SelectObject( hDCSrc, hBitmap ));
    hDCMask = CreateCompatibleDC( hDCDest );
    hBitmapMask = CreateBitmap( nBitmapWidth, nBitmapHeight, 1, 1, 0 );
    hBitmapMaskOld = reinterpret_cast<HBITMAP>(SelectObject( hDCMask, hBitmapMask ));
    hDCMem = CreateCompatibleDC( hDCDest );
    hBitmapMem = CreateCompatibleBitmap( hDCDest, nBitmapWidth, nBitmapHeight );
    hBitmapMemOld = reinterpret_cast<HBITMAP>(SelectObject( hDCMem, hBitmapMem ));
    nBkColorOld = SetBkColor( hDCSrc, nTransparentColor );
    BitBlt( hDCMask, 0, 0, nBitmapWidth, nBitmapHeight, hDCSrc, nXSrc, nYSrc, SRCCOPY );
    SetBkColor( hDCSrc, nBkColorOld );
    nBkColorOld = SetBkColor( hDCDest, RGB(255,255,255) );
    nTextColorOld = SetTextColor( hDCDest, RGB(0,0,0) );
    BitBlt( hDCMem, 0, 0, nBitmapWidth, nBitmapHeight, hDCDest, nXDest, nYDest, SRCCOPY );
    BitBlt( hDCMem, 0, 0, nBitmapWidth, nBitmapHeight, hDCSrc, nXSrc, nYSrc, SRCINVERT );
    BitBlt( hDCMem, 0, 0, nBitmapWidth, nBitmapHeight, hDCMask, 0, 0, SRCAND );
    BitBlt( hDCMem, 0, 0, nBitmapWidth, nBitmapHeight, hDCSrc, nXSrc, nYSrc, SRCINVERT );
    BitBlt( hDCDest, nXDest, nYDest, nBitmapWidth, nBitmapHeight, hDCMem, 0, 0, SRCCOPY );
    SetBkColor( hDCDest, nBkColorOld );
    SetTextColor( hDCDest, nTextColorOld );
    SelectObject( hDCMem, hBitmapMemOld );
    DeleteDC( hDCMem );
    DeleteObject( hBitmapMem );
    SelectObject( hDCMask, hBitmapMaskOld );
    DeleteDC( hDCMask );
    DeleteObject( hBitmapMask );
    SelectObject( hDCSrc, hBitmapOld );
    DeleteDC( hDCSrc );
}

LRESULT CALLBACK SliderControlWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
switch(message) {
    case WM_NOTIFY :
    {
        LPNMHDR lpNmhdr = reinterpret_cast<LPNMHDR>(lParam);
        if (lpNmhdr->code == NM_CUSTOMDRAW)
        {
            LPNMCUSTOMDRAW lpNMCustomDraw = reinterpret_cast<LPNMCUSTOMDRAW>(lParam);

            if (lpNMCustomDraw->dwDrawStage == CDDS_PREPAINT) {
                return CDRF_NOTIFYITEMDRAW;
            }

            else if (lpNMCustomDraw->dwDrawStage == CDDS_ITEMPREPAINT)
            {
                long nLeft = lpNMCustomDraw->rc.left;
                long nTop = lpNMCustomDraw->rc.top;
                long nRight = lpNMCustomDraw->rc.right;
                long nBottom = lpNMCustomDraw->rc.bottom;

                if (lpNMCustomDraw->dwItemSpec == TBCD_THUMB && hBitmapThumb)
                {
                    long nWidth = nRight - nLeft;
                    long nHeight = nBottom - nTop;

                    if (nWidth - bm.bmWidth > 0)
                    {
                        nLeft += (nWidth - bm.bmWidth)/2;
                        nWidth = bm.bmWidth;
                    }

                    if (nHeight - bm.bmHeight > 0)
                    {
                        nTop += (nHeight - bm.bmHeight) / 2;
                        nHeight = bm.bmHeight;
                    }

                    DrawBitmapTransparent(lpNMCustomDraw->hdc , nLeft, nTop, nWidth, nHeight, hBitmapThumb, 0, 0, RGB( 255, 0, 255 ));

                    return CDRF_SKIPDEFAULT ;
                }
            }
        }
    }
    break;
}
    if (message == WM_CLOSE && hwnd == window) return OnWindowClose(hwnd, message, wParam, lParam);
    if (message == WM_HSCROLL && hwnd == window && reinterpret_cast<HWND>(lParam) == trackBar) return OnTrackBarChanged(hwnd, message, wParam, lParam);
    return CallWindowProc(defWndProc, hwnd, message, wParam, lParam);
}

int mainCreateSliderControl()
{
    window = CreateWindowEx(0, WC_DIALOG, L"TrackBar example", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, nullptr, nullptr, nullptr, nullptr);
    trackBar = CreateWindowEx(0, TRACKBAR_CLASS, nullptr, WS_CHILD | TBS_HORZ | TBS_BOTTOM | WS_VISIBLE | TBS_FIXEDLENGTH, 150, 10, 250, 70, window, nullptr, nullptr, nullptr);
    progressBar = CreateWindowEx(0, PROGRESS_CLASS, nullptr, WS_CHILD | PBS_SMOOTH | WS_VISIBLE, 20, 100, 200, 23, window, nullptr, nullptr, nullptr);
    staticText = CreateWindowEx(0, WC_STATIC, L"100", WS_CHILD | WS_VISIBLE, 20, 150, 100, 23, window, nullptr, nullptr, nullptr);

    defWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SliderControlWndProc)));

    hBitmapThumb = reinterpret_cast<HBITMAP>(LoadImage(NULL, reinterpret_cast<LPCWSTR>("pink.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
    GetObject( hBitmapThumb, sizeof( BITMAP ), &bm );

    SendMessage(trackBar, TBM_SETRANGEMIN, 1, 0);
    SendMessage(trackBar, TBM_SETRANGEMAX, 1, 200);
    SendMessage(trackBar, TBM_SETTHUMBLENGTH, bm.bmWidth*2, 0);
    SendMessage(progressBar, PBM_SETRANGE32, 0, 200);
    SendMessage(progressBar, PBM_SETPOS, 100, 0);

    ShowWindow(window, SW_SHOW);

    MSG message = { 0 };
    while (GetMessage(&message, nullptr, 0, 0))
    DispatchMessage(&message);
    return 0;
}
