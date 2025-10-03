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

    winrt::make<winrt::Win32App::implementation::App>().as(m_app);
    auto webView = winrt::Controls::WebView2();
    webView.Source(winrt::Windows::Foundation::Uri{ L"http://www.ddg.gg" });
    m_islandWv2 = winrt::Xaml::XamlIsland();
    m_islandWv2.Content(webView);

    // Create bridge to host parent island and connect.
    auto windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(hWnd);
    m_bridge = DesktopChildSiteBridge::Create(m_compositor, windowId);
    m_bridge.OverrideScale(1.0f);
    m_bridge.Connect(m_islandWv2.ContentIsland());
    m_bridge.MoveAndResize({ 0, 0, static_cast<int>(kWindowSize.x), static_cast<int>(kWindowSize.y) });
    m_bridge.Show();
}
