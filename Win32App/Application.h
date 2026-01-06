#pragma once

#include "pch.h"
#include "App.xaml.h"

#include <vector>

class Application : public winrt::implements<Application,
    winrt::Microsoft::UI::Composition::Interactions::IInteractionTrackerOwner>
{
public:
    static Application& GetInstance() {
        static Application* instance = winrt::make_self<Application>().detach();
        return *instance;
    }

    void Initialize(HWND hWnd);

    static constexpr winrt::Numerics::float2 kWindowSize = { 800.0f, 600.0f };
    static constexpr int kIslandCount = 7;
    static constexpr float kIslandHeight = 150.0f;
    static constexpr float kIslandSpacing = 20.0f;

    ~Application() {
        if (m_bridgePopup)
            m_bridgePopup.Close();
        if (m_bridge)
            m_bridge.Close();
    }

    bool isPopupShowing() { return !!m_bridgePopup; }
    void showPopup();
    void dismissPopup();

    // IInteractionTrackerOwner methods
    void CustomAnimationStateEntered(
        winrt::Microsoft::UI::Composition::Interactions::InteractionTracker const&,
        winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs const&) {
    }
    void IdleStateEntered(
        winrt::Microsoft::UI::Composition::Interactions::InteractionTracker const&,
        winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs const&) {
    }
    void InertiaStateEntered(
        winrt::Microsoft::UI::Composition::Interactions::InteractionTracker const&,
        winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs const&) {
    }
    void InteractingStateEntered(
        winrt::Microsoft::UI::Composition::Interactions::InteractionTracker const&,
        winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs const&) {
    }
    void RequestIgnored(
        winrt::Microsoft::UI::Composition::Interactions::InteractionTracker const&,
        winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs const&) {
    }
    void ValuesChanged(
        winrt::Microsoft::UI::Composition::Interactions::InteractionTracker const&,
        winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs const&);

    // Non-copyable, immovable, non-assignable. 
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    Application() = default;

private:
    float GetContentHeight() const {
        return kIslandCount * kIslandHeight + (kIslandCount + 1) * kIslandSpacing;
    }

    winrt::MUC::Compositor m_compositor = winrt::MUC::Compositor{};
    winrt::MUCn::DesktopChildSiteBridge m_bridge{ nullptr };
    winrt::MUCn::DesktopPopupSiteBridge m_bridgePopup{ nullptr };
    winrt::MUCn::ContentIsland m_islandParent{ nullptr };

    // 7 XAML Islands and their site links (vectors initialized in Initialize())
    std::vector<winrt::Xaml::XamlIsland> m_xamlIslands;
    std::vector<winrt::MUCn::ChildSiteLink> m_siteLinks;
    std::vector<winrt::MUC::SpriteVisual> m_islandVisuals;

    // Scrollable content container
    winrt::MUC::ContainerVisual m_scrollableContent{ nullptr };

    // InteractionTracker for scrolling
    winrt::MUC::Interactions::InteractionTracker m_tracker{ nullptr };

    winrt::com_ptr<winrt::Win32App::implementation::App> m_app{ nullptr };
    winrt::MUI::InputPointerSource m_pointerSource{ nullptr };
};
