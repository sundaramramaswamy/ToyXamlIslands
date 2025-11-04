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

    ~Application() {
        if (m_bridgePopup)
            m_bridgePopup.Close();
        if (m_bridge)
            m_bridge.Close();
    }

    bool isPopupShowing() { return !!m_bridgePopup; }
    void showPopup();
    void dismissPopup();

    // Non-copyable, immovable, non-assignable.
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

private:
    Application() = default;

    winrt::MUC::Compositor m_compositor = winrt::MUC::Compositor{};
    winrt::MUCn::DesktopChildSiteBridge m_bridge{ nullptr };
    winrt::MUCn::DesktopPopupSiteBridge m_bridgePopup{ nullptr };
    winrt::MUCn::ContentIsland m_islandParent{ nullptr };
    winrt::Xaml::XamlIsland m_islandXaml{ nullptr };
    winrt::Xaml::XamlIsland m_islandWv2{ nullptr };
    winrt::MUCn::ChildSiteLink m_siteXaml{ nullptr };
    winrt::MUCn::ChildSiteLink m_siteWv2{ nullptr };
    winrt::com_ptr<winrt::Win32App::implementation::App> m_app{ nullptr };
};
