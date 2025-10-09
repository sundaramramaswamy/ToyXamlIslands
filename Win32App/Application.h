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
    winrt::MUCn::DesktopChildSiteBridge m_bridge{ nullptr };
    winrt::MUCn::ContentIsland m_islandParent{ nullptr };
    winrt::Xaml::XamlIsland m_islandXaml{ nullptr };
    winrt::Xaml::XamlIsland m_islandWv2{ nullptr };
    winrt::MUCn::ChildSiteLink m_xamlSiteLink{ nullptr };
    winrt::MUCn::ChildSiteLink m_wv2SiteLink{ nullptr };
    winrt::com_ptr<winrt::Win32App::implementation::App> m_app{ nullptr };

    winrt::MUC::ContainerVisual m_visualParent{ nullptr };
};
