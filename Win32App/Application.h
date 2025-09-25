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

    static constexpr winrt::Numerics::float2 kWindowSize = { 800.0f, 600.0f };

private:
    Application() = default;

    winrt::MUC::Compositor m_compositor = winrt::MUC::Compositor{};
    winrt::MUCn::DesktopAttachedSiteBridge m_bridge{ nullptr };
    winrt::MUCn::ContentIsland m_islandParent{ nullptr };
    winrt::MUCn::ContentIsland m_islandChild{ nullptr };
    winrt::MUCn::ChildSiteLink m_childSiteLink{ nullptr };
    winrt::com_ptr<winrt::Win32App::implementation::App> m_app{ nullptr };

    winrt::MUC::ContainerVisual m_visualParent{ nullptr };
    winrt::MUC::ContainerVisual m_visualChild{ nullptr };
};
