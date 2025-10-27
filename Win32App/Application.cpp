#include "pch.h"
#include "Application.h"

using winrt::MUCn::DesktopChildSiteBridge;
using winrt::MU::ColorHelper;
using winrt::MU::Colors;

namespace {

    winrt::Xaml::UIElement CreateXamlTree()
    {
        // Create a Xaml container and the control that goes into it.
        winrt::Controls::StackPanel container;
        container.Background(winrt::Xaml::Media::SolidColorBrush(Colors::LightBlue()));
        winrt::Controls::TextBox textBox;
        textBox.PlaceholderText(L"Go on, enter something...");
        textBox.Width(550);
        textBox.FontStyle(winrt::Text::FontStyle::Italic);
        textBox.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
        textBox.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Center);
        textBox.FontSize(48);
        winrt::Controls::TextBlock textBlock;
        textBlock.Text(L"Input");
        textBlock.VerticalAlignment(winrt::Xaml::VerticalAlignment::Center);
        textBlock.HorizontalAlignment(winrt::Xaml::HorizontalAlignment::Center);
        textBlock.FontSize(48);
        const auto orange = ColorHelper::FromArgb(0xff, 0xff, 0xa5, 0x00);
        textBlock.Foreground(winrt::Xaml::Media::SolidColorBrush(orange));
        winrt::Xaml::Thickness textBlockMargin{ 10.0, 0, 10.0 };
        textBlock.Margin(textBlockMargin);
        container.Orientation(winrt::Xaml::Controls::Orientation::Horizontal);
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
                m_xamlSiteLink.LocalToParentTransformMatrix(m_islandXaml.Content().TransformMatrix());
                m_xamlSiteLink.ActualSize(innerIslandSize);

                m_wv2SiteLink.LocalToParentTransformMatrix(m_islandWv2.Content().TransformMatrix());
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
