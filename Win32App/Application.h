#pragma once

#include "pch.h"
#include "App.xaml.h"

class Application
{
public:
    static Application& GetInstance() {
        static Application a;
        return a;
    }

    void Initialize(HWND hWnd);

    static constexpr float kWindowWidth = 800.0f;
    static constexpr float kWindowHeight = 600.0f;

private:
    Application() = default;

    winrt::MUC::Compositor m_compositor = winrt::MUC::Compositor{};
    winrt::MUCn::DesktopAttachedSiteBridge m_bridge{ nullptr };
    winrt::com_ptr<winrt::Win32App::implementation::App> m_app{ nullptr };
    winrt::Xaml::XamlIsland m_islandXaml{ nullptr };
};
