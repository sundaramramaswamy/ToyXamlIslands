#include "pch.h"
#include "Application.h"

using winrt::MUCn::DesktopChildSiteBridge;
using winrt::MU::ColorHelper;
using winrt::MU::Colors;

namespace {

    // Creates a single XAML control based on index
    winrt::Xaml::UIElement CreateSingleControl(int index)
    {
        const auto orange = ColorHelper::FromArgb(0xff, 0xff, 0xa5, 0x00);
        winrt::Xaml::Thickness margin{ 10.0, 10.0, 10.0, 10.0 };

        switch (index) {
        case 0: {
            winrt::Controls::Button button;
            button.Content(winrt::box_value(L"Button Control"));
            button.FontSize(32);
            button.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Stretch);
            button.VerticalAlignment(winrt::Xaml::VerticalAlignment::Stretch);
            button.Margin(margin);
            return button;
        }
        case 1: {
            winrt::Controls::TextBox textBox;
            textBox.PlaceholderText(L"TextBox - Enter text here.. .");
            textBox.FontSize(28);
            textBox.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Stretch);
            textBox.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
            textBox.Margin(margin);
            return textBox;
        }
        case 2: {
            winrt::Controls::CheckBox checkBox;
            checkBox.Content(winrt::box_value(L"CheckBox Control"));
            checkBox.FontSize(28);
            checkBox.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Center);
            checkBox.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
            return checkBox;
        }
        case 3: {
            winrt::Controls::ComboBox comboBox;
            comboBox.Items().Append(winrt::box_value(L"Option 1"));
            comboBox.Items().Append(winrt::box_value(L"Option 2"));
            comboBox.Items().Append(winrt::box_value(L"Option 3"));
            comboBox.Items().Append(winrt::box_value(L"Option 4"));
            comboBox.SelectedIndex(0);
            comboBox.FontSize(28);
            comboBox.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Stretch);
            comboBox.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
            comboBox.Margin(margin);
            return comboBox;
        }
        case 4: {
            winrt::Controls::CalendarDatePicker cal{};
            cal.FontSize(48);
            cal.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Stretch);
            cal.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
            return cal;
        }
        /*{
            winrt::Controls::Slider slider;
            slider.Minimum(0);
            slider.Maximum(100);
            slider.Value(50);
            slider.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Stretch);
            slider.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
            slider.Margin(margin);
            return slider;
        }*/
        case 5: {
            winrt::Controls::ProgressRing progressRing;
            progressRing.IsActive(true);
            progressRing.Width(80);
            progressRing.Height(80);
            progressRing.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Center);
            progressRing.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
            return progressRing;
        }
        case 6: {
            winrt::Controls::ToggleSwitch toggleSwitch;
            toggleSwitch.Header(winrt::box_value(L"ToggleSwitch Control"));
            toggleSwitch.OnContent(winrt::box_value(L"On"));
            toggleSwitch.OffContent(winrt::box_value(L"Off"));
            toggleSwitch.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Center);
            toggleSwitch.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
            return toggleSwitch;
        }
        default: {
            winrt::Controls::TextBlock textBlock;
            textBlock.Text(L"Unknown Control");
            textBlock.FontSize(28);
            return textBlock;
        }
        }
    }

    // Background colors for each island
    winrt::Windows::UI::Color GetIslandColor(int index) {
        static const winrt::Windows::UI::Color colors[] = {
            Colors::LightBlue(),
            Colors::LightGreen(),
            Colors::LightCoral(),
            Colors::LightGoldenrodYellow(),
            Colors::LightPink(),
            Colors::LightCyan(),
            Colors::LightSalmon()
        };
        return colors[index % 7];
    }

} // namespace

void Application::Initialize(HWND hWnd) {
    if (m_bridge)
        throw std::runtime_error("Application is already initialized.");

    const float contentHeight = GetContentHeight();

    // Reserve space for vectors
    m_xamlIslands.reserve(kIslandCount);
    m_siteLinks.reserve(kIslandCount);
    m_islandVisuals.reserve(kIslandCount);

    // Create a root container visual (the viewport/clip area)
    winrt::MUC::ContainerVisual visualParent = m_compositor.CreateContainerVisual();
    visualParent.RelativeSizeAdjustment({ 1.0f, 1.0f });
    visualParent.Clip(m_compositor.CreateInsetClip());  // Clip content to viewport

    // Create a sprite visual to paint root container background
    auto parentFill = m_compositor.CreateSpriteVisual();
    parentFill.Brush(m_compositor.CreateColorBrush(Colors::DarkGray()));
    parentFill.RelativeSizeAdjustment({ 1.0f, 1.0f });
    visualParent.Children().InsertAtBottom(parentFill);

    // Create scrollable content container - this will be moved by InteractionTracker
    m_scrollableContent = m_compositor.CreateContainerVisual();
    m_scrollableContent.Size({ kWindowSize.x, contentHeight });
    visualParent.Children().InsertAtTop(m_scrollableContent);

    // Create parent island at root visual's bounds
    m_islandParent = winrt::MUCn::ContentIsland::Create(visualParent);

    // --- SETUP INTERACTION TRACKER FOR SCROLLING ---
    m_tracker = winrt::MUCi::InteractionTracker::CreateWithOwner(m_compositor, *this);

    // Set vertical scroll boundaries
    m_tracker.MinPosition({ 0.0f, 0.0f, 0.0f });
    float maxScroll = contentHeight - kWindowSize.y;
    if (maxScroll < 0) maxScroll = 0;
    m_tracker.MaxPosition({ 0.0f, maxScroll, 0.0f });

    // Create interaction source from the parent visual
    auto interactionSource = winrt::MUCi::VisualInteractionSource::Create(visualParent);
    interactionSource.PositionYSourceMode(winrt::MUCi::InteractionSourceMode::EnabledWithInertia);
    interactionSource.PositionXSourceMode(winrt::MUCi::InteractionSourceMode::Disabled);
    interactionSource.ManipulationRedirectionMode(
        winrt::MUCi::VisualInteractionSourceRedirectionMode::CapableTouchpadAndPointerWheel);

    m_tracker.InteractionSources().Add(interactionSource);

    // Bind tracker position to scrollable content offset
    auto scrollExpression = m_compositor.CreateExpressionAnimation(L"-tracker.Position.Y");
    scrollExpression.SetReferenceParameter(L"tracker", m_tracker);
    m_scrollableContent.StartAnimation(L"Offset.Y", scrollExpression);

    // Handle resize events
    m_islandParent.StateChanged([this, contentHeight](const auto&,
        const winrt::MUCn::ContentIslandStateChangedEventArgs& args) {
            if (args.DidActualSizeChange()) {
                const auto islandSize = m_islandParent.ActualSize();

                // Update tracker max position on resize
                float maxScroll = contentHeight - islandSize.y;
                if (maxScroll < 0) maxScroll = 0;
                m_tracker.MaxPosition({ 0.0f, maxScroll, 0.0f });

                // Update island visual widths and site links
                for (int i = 0; i < kIslandCount; ++i) {
                    m_islandVisuals[i].Size({ islandSize.x - 2 * kIslandSpacing, kIslandHeight });

                    if (m_siteLinks[i]) {
                        m_siteLinks[i].ActualSize({ islandSize.x - 2 * kIslandSpacing, kIslandHeight });
                    }
                }
            }
        });

    // Create bridge to host parent island
    auto windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(hWnd);
    m_bridge = DesktopChildSiteBridge::Create(m_compositor, windowId);
    m_bridge.OverrideScale(1.0f);
    m_bridge.ResizePolicy(winrt::MUCn::ContentSizePolicy::ResizeContentToParentWindow);
    m_bridge.Connect(m_islandParent);
    m_bridge.MoveAndResize({ 0, 0,
        static_cast<int>(kWindowSize.x),
        static_cast<int>(kWindowSize.y) });
    m_bridge.Show();

    // --- GET INPUT POINTER SOURCE FOR WHEEL HANDLING ---
    // Get the InputPointerSource from the ContentIsland
    m_pointerSource = winrt::MUI::InputPointerSource::GetForIsland(m_islandParent);

    // Dismiss popups when clicking on empty space (light-dismiss behavior)
    m_pointerSource.PointerPressed(
        [this](const winrt::Microsoft::UI::Input::InputPointerSource&,
            const winrt::Microsoft::UI::Input::PointerEventArgs&) {
                for (auto& island : m_xamlIslands) {
                    if (island && island.Content()) {
                        auto xamlRoot = island.Content().XamlRoot();
                        if (xamlRoot) {
                            auto popups = winrt::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot);
                            for (uint32_t i = 0; i < popups.Size(); ++i) {
                                popups.GetAt(i).IsOpen(false);
                            }
                        }
                    }
                }
        });

    // Initialize the XAML app (only once for all islands)
    winrt::make<winrt::Win32App::implementation::App>().as(m_app);

    // Create 7 XAML Islands, each with a single control
    for (int i = 0; i < kIslandCount; ++i) {
        // Calculate vertical position for this island
        float yOffset = kIslandSpacing + i * (kIslandHeight + kIslandSpacing);

        // Create visual to host this island
        auto visual = m_compositor.CreateSpriteVisual();
        visual.Brush(m_compositor.CreateColorBrush(GetIslandColor(i)));
        visual.Size({ kWindowSize.x - 2 * kIslandSpacing, kIslandHeight });
        visual.Offset({ kIslandSpacing, yOffset, 0.0f });
        m_scrollableContent.Children().InsertAtTop(visual);
        m_islandVisuals.push_back(visual);

        // Create XAML Island with a single control
        auto xamlIsland = winrt::Xaml::XamlIsland();
        xamlIsland.Content(CreateSingleControl(i));
        m_xamlIslands.push_back(xamlIsland);

        // Create child site link and connect to island
        auto siteLink = winrt::MUCn::ChildSiteLink::Create(m_islandParent, visual);
        siteLink.ActualSize({ kWindowSize.x - 2 * kIslandSpacing, kIslandHeight });

        // Connect island to link
        siteLink.Connect(xamlIsland.ContentIsland());
        m_siteLinks.push_back(siteLink);
    }
}

void Application::ValuesChanged(
    winrt::Microsoft::UI::Composition::Interactions::InteractionTracker const&,
    winrt::Microsoft::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs const&)
{
    // Optional: Handle scroll position changes if needed
    // e.g., for lazy loading, visibility culling, etc.
}

void Application::showPopup() {
    assert(m_bridge);
    if (m_bridgePopup) {
        OutputDebugStringA("showPopup:  Ignoring request as popup already showing up.");
        return;
    }
    auto fill = m_compositor.CreateSpriteVisual();
    fill.Brush(m_compositor.CreateColorBrush(Colors::Orange()));
    fill.RelativeSizeAdjustment({ 1.0f, 1.0f });
    auto popupIsland = winrt::MUCn::ContentIsland::Create(fill);
    m_bridgePopup = winrt::MUCn::DesktopPopupSiteBridge::Create(m_islandParent);
    m_bridgePopup.Connect(popupIsland);

    const auto parentView = m_bridge.SiteView();
    const auto parentSize = parentView.ActualSize();
    const auto convertor = parentView.CoordinateConverter();
    m_bridgePopup.MoveAndResize(convertor.ConvertLocalToScreen({
        parentSize.x * 0.25f,
        parentSize.y * 0.25f,
        parentSize.x * 0.5f,
        parentSize.y * 0.5f
        }));
    m_bridgePopup.Show();
}

void Application::dismissPopup() {
    if (m_bridgePopup && m_bridgePopup.IsVisible()) {
        m_bridgePopup.Hide();
        m_bridgePopup.Close();
        m_bridgePopup = nullptr;
    }
}
