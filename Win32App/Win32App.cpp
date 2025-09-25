// Win32App.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "Application.h"

#include <stdexcept>
#include <cstdio>

HWND InitializeWindow(HINSTANCE hInst, LONG nWidth, LONG nHeight);
LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

    // Initialize COM and the Windows Runtime.
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    HWND hWnd = InitializeWindow(hInstance,
        static_cast<LONG>(Application::kWindowSize.x),
        static_cast<LONG>(Application::kWindowSize.y));

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    auto dispatcherQueueController = winrt::MUD::DispatcherQueueController::CreateOnCurrentThread();
    auto app = Application::GetInstance();
    app.Initialize(hWnd);

#ifdef USER32_LOOP_INTEROP
    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!::ContentPreTranslateMessage(&msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
#else
    dispatcherQueueController.DispatcherQueue().RunEventLoop();
#endif

    dispatcherQueueController.ShutdownQueueAsync();

    return 0;
}

HWND InitializeWindow(HINSTANCE hInst, LONG nWidth, LONG nHeight)
{
    WNDCLASSEX wc = { sizeof(wc) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = hInst;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"Win32App";
    wc.lpszMenuName = NULL;
    wc.hIconSm = wc.hIcon;
    if (!RegisterClassEx(&wc))
        throw std::runtime_error("Failed to register class: Win32App");

    const auto dwStyle = WS_OVERLAPPEDWINDOW;
    RECT windowRect = { 0, 0, nWidth, nHeight };
    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, 0);

    // https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
    HWND hWindow = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName,
        L"Hello, Islands!",
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInst,
        nullptr);
    if (!hWindow) throw std::runtime_error("Failed to create window");

    return hWindow;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wparam, lparam);
    }
    return 0;
}
