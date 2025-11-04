#include "pch.h"
#include "Application.h"

using winrt::MUCn::DesktopChildSiteBridge;
using winrt::MU::ColorHelper;
using winrt::MU::Colors;

namespace {

    winrt::Xaml::UIElement CreateXamlTree()
    {
        // Create a Xaml container and the control that goes into it.
        winrt::Controls::Grid container;
        container.Background(winrt::Xaml::Media::SolidColorBrush(Colors::LightBlue()));
        winrt::Controls::ColumnDefinition c1, c2;
        // Use 1:3 ratio for text block:text box.
        c1.Width(winrt::Xaml::GridLengthHelper::FromValueAndType(1.0, winrt::Xaml::GridUnitType::Star));
        c2.Width(winrt::Xaml::GridLengthHelper::FromValueAndType(3.0, winrt::Xaml::GridUnitType::Star));
        container.ColumnDefinitions().Append(c1);
        container.ColumnDefinitions().Append(c2);
        container.RowDefinitions().Append({});
        container.RowDefinitions().Append({});

        winrt::Controls::Button button;
        button.Content(winrt::box_value(L"Popup"));
        button.FontSize(48);
        button.Click([](const auto&, const winrt::Xaml::RoutedEventArgs&) {
            auto& app = Application::GetInstance();
            if (!app.isPopupShowing())
                app.showPopup();
            else
                app.dismissPopup();
            });
        container.SetRow(button, 0);
        container.SetColumn(button, 1);

        winrt::Controls::TextBlock textBlock;
        textBlock.Text(L"Input");
        textBlock.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
        textBlock.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Left);
        textBlock.FontSize(48);
        const auto orange = ColorHelper::FromArgb(0xff, 0xff, 0xa5, 0x00);
        textBlock.Foreground(winrt::Xaml::Media::SolidColorBrush(orange));
        winrt::Xaml::Thickness controlMargin{ 10.0, 0, 10.0 };
        textBlock.Margin(controlMargin);
        container.SetRow(textBlock, 1);
        container.SetColumn(textBlock, 0);

        winrt::Controls::TextBox textBox;
        textBox.PlaceholderText(L"Go on, enter something...");
        textBox.FontStyle(winrt::Text::FontStyle::Italic);
        textBox.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
        textBox.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Stretch);
        textBox.FontSize(48);
        textBox.Margin(controlMargin);
        container.SetRow(textBox, 1);
        container.SetColumn(textBox, 1);

        container.Children().Append(button);
        container.Children().Append(textBlock);
        container.Children().Append(textBox);
        container.UpdateLayout();

        return container;
    }

} // namespace

void Application::Initialize(HWND hWnd) {
    if (m_bridge)
        throw std::runtime_error("Application is already initialized.");

    // Create a root container visual.
    m_visualParent = m_compositor.CreateContainerVisual();
    m_visualParent.RelativeSizeAdjustment({ 1.0f, 1.0f });
    // Create a sprite visual to paint root container in red.
    auto parentFill = m_compositor.CreateSpriteVisual();
    parentFill.Brush(m_compositor.CreateColorBrush(Colors::Red()));
    parentFill.RelativeSizeAdjustment({ 1.0f, 1.0f });
    // Add sprite to container visual.
    m_visualParent.Children().InsertAtTop(parentFill);
    // Create parent island at root visual's bounds.  This visual is the root
    // for island's visual tree.
    m_islandParent = winrt::MUCn::ContentIsland::Create(m_visualParent);

    m_islandParent.StateChanged([&](const auto&,
        const winrt::MUCn::ContentIslandStateChangedEventArgs& args) {
            if (args.DidActualSizeChange()) {
                const auto islandSize = m_islandParent.ActualSize();
                const winrt::Numerics::float2 innerIslandSize = { islandSize.x, islandSize.y * 0.5f };
                // Setting these properties are important for the child island to show up with a
                // non-zero size both during init and later resizes.
                m_xamlSiteLink.LocalToParentTransformMatrix(winrt::Numerics::float4x4::identity());
                m_xamlSiteLink.ActualSize(innerIslandSize);

                m_wv2SiteLink.LocalToParentTransformMatrix(
                    winrt::Numerics::make_float4x4_translation({ 0.0f, islandSize.y * 0.5f, 0.0f }));
                m_wv2SiteLink.ActualSize(innerIslandSize);
            }
        });

    // Create bridge to host parent island and connect.
    auto windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(hWnd);
    m_bridge = DesktopChildSiteBridge::Create(m_compositor, windowId);
    m_bridge.OverrideScale(1.0f);
    // Auto-resize visual tree on WM_SIZE events.
    m_bridge.ResizePolicy(winrt::MUCn::ContentSizePolicy::ResizeContentToParentWindow);
    m_bridge.Connect(m_islandParent);
    m_bridge.MoveAndResize({ 0, 0,
        static_cast<int>(kWindowSize.x),
        static_cast<int>(kWindowSize.y) });
    m_bridge.Show();

    // Create a visual to host Xaml island.  Add it to root container visual.
    auto xamlVisual = m_compositor.CreateSpriteVisual();
    xamlVisual.Brush(m_compositor.CreateColorBrush(Colors::Teal()));
    xamlVisual.RelativeSizeAdjustment({ 1.0f, 0.5f });
    m_visualParent.Children().InsertAtTop(xamlVisual);

    // Create a visual to host WV2 island.  Add it to root container visual.
    auto wv2Visual = m_compositor.CreateSpriteVisual();
    wv2Visual.Brush(m_compositor.CreateColorBrush(Colors::Yellow()));
    wv2Visual.RelativeSizeAdjustment({ 1.0f, 0.5f });
    wv2Visual.RelativeOffsetAdjustment({ 0.0f, 0.5f, 0.0f });
    m_visualParent.Children().InsertAtTop(wv2Visual);

    // Create child Xaml island.
    winrt::make<winrt::Win32App::implementation::App>().as(m_app);
    m_islandXaml = winrt::Xaml::XamlIsland();
    m_islandXaml.Content(CreateXamlTree());

    // Create child site link in parent for Xaml at one of its visuals and connect to island.
    m_xamlSiteLink = winrt::MUCn::ChildSiteLink::Create(m_islandParent, xamlVisual);
    m_xamlSiteLink.Connect(m_islandXaml.ContentIsland());

    WINRT_ASSERT(m_islandXaml.ContentIsland().Environment().AppWindowId() == windowId);

    // Create a child WebView2 island.
    auto webView = winrt::Controls::WebView2();
    winrt::Windows::Foundation::Uri uri{ L"http://www.bing.com" };
    webView.Source(uri);
    m_islandWv2 = winrt::Xaml::XamlIsland();
    m_islandWv2.Content(webView);

    // Create child site link in parent for WebView2 at one of its visuals and connect to island.
    m_wv2SiteLink = winrt::MUCn::ChildSiteLink::Create(m_islandParent, wv2Visual);
    m_wv2SiteLink.Connect(m_islandWv2.ContentIsland());
}

void Application::showPopup() {
    assert(m_bridge);
    if (m_bridgePopup) {
        OutputDebugStringA("showPopup: Ignoring request as popup already showing up.");
        return;
    }
    auto fill = m_compositor.CreateSpriteVisual();
    fill.Brush(m_compositor.CreateColorBrush(Colors::Orange()));
    fill.RelativeSizeAdjustment({ 1.0f, 1.0f });
    auto popupIsland = winrt::MUCn::ContentIsland::Create(fill);
    m_bridgePopup = winrt::MUCn::DesktopPopupSiteBridge::Create(m_islandParent);
    m_bridgePopup.Connect(popupIsland);

#ifndef POPUP_PARENT_IS_WEBVIEW
    const auto parentView = m_bridge.SiteView();
#else
    const auto parentView = m_wv2SiteLink.SiteView();
#endif // POPUP_PARENT_IS_WEBVIEW
    const auto parentSize = parentView.ActualSize();
    const auto convertor = parentView.CoordinateConverter();
    m_bridgePopup.MoveAndResize(convertor.ConvertLocalToScreen({
        parentSize.x * 0.25f,  // Offset
        parentSize.y * 0.25f,
        parentSize.x * 0.5f,   // Size
        parentSize.y * 0.5f
    }));
    m_bridgePopup.Show();
}

void Application::dismissPopup() {
    if (m_bridgePopup && m_bridgePopup.IsVisible()) {
        // Hide and close as close doesn't call hide and we continue to see a popup!
        m_bridgePopup.Hide();
        m_bridgePopup.Close();
        m_bridgePopup = nullptr;
    }
}