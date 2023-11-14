//***************************************************************************
// 
// 파일: main.c
// 
// 설명: main 함수
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/10/27
// 
//***************************************************************************

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <windowsx.h>
#include <crtdbg.h>

#include "safe99_common/defines.h"
#include "key_event.h"
#include "game.h"

HINSTANCE g_hinstance;
HWND g_hwnd;

HRESULT init_window(const size_t width, const size_t height);
LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int main(void)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF
                   | _CRTDBG_LEAK_CHECK_DF
                   | _CRTDBG_DELAY_FREE_MEM_DF);

    const int window_width = 800;
    const int window_height = 600;

    if (FAILED(init_window(window_width, window_height)))
    {
        ASSERT(false, "Failed to init window");
        return 0;
    }

    gp_game = (game_t*)malloc(sizeof(game_t));
    if (gp_game == NULL)
    {
        ASSERT(false, "Failed to malloc app");
        return 0;
    }

    if (!init_game())
    {
        ASSERT(false, "Failed to init app");
        return 0;
    }

    // Main message loop
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (!is_running_game())
            {
                break;
            }

            tick_game();
        }
    }

    shutdown_game();
    SAFE_FREE(gp_game);

    _CrtDumpMemoryLeaks();

    return (int)msg.wParam;
}

HRESULT init_window(const size_t width, const size_t height)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wnd_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandleW(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"safe99";
    wcex.hIconSm = NULL;
    if (!RegisterClassEx(&wcex))
    {
        return E_FAIL;
    }

    // Create window
    g_hinstance = wcex.hInstance;
    RECT rc = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hwnd = CreateWindow(wcex.lpszClassName, L"safe99 - bit game template",
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                          CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, wcex.hInstance,
                          NULL);
    if (!g_hwnd)
    {
        return E_FAIL;
    }

    ShowWindow(g_hwnd, SW_SHOWDEFAULT);

    return S_OK;
}

LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        on_down_left_mouse();
        break;
    }
    case WM_LBUTTONUP:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        on_up_left_mouse();
        break;
    }
    case WM_RBUTTONDOWN:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        on_down_right_mouse();
        break;
    }
    case WM_RBUTTONUP:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        on_up_right_mouse();
        break;
    }
    case WM_MOUSEMOVE:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        on_move_mouse(x, y);
        break;
    }

    case WM_KEYDOWN:
        on_key_down((uint8_t)wParam);
        break;
    case WM_KEYUP:
        on_key_up((uint8_t)wParam);
        break;

    case WM_SIZE:
        if (gp_game != NULL)
        {
            update_window_size_game();
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}