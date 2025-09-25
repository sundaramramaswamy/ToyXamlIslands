#include "pch.h"
#include "Application.h"

using winrt::MUCn::DesktopAttachedSiteBridge;
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

    // Create parent {visual filled red, island}.
    auto parentFill = m_compositor.CreateSpriteVisual();
    parentFill.Brush(m_compositor.CreateColorBrush(Colors::Red()));
    parentFill.RelativeSizeAdjustment({ 1.0f, 1.0f });
    m_visualParent = m_compositor.CreateContainerVisual();
    m_visualParent.RelativeSizeAdjustment({ 1.0f, 1.0f });
    m_visualParent.Children().InsertAtTop(parentFill);
    m_islandParent = winrt::MUCn::ContentIsland::Create(m_visualParent);

    // Create child Xaml island.
    winrt::make<winrt::Win32App::implementation::App>().as(m_app);
    m_islandXaml = winrt::Xaml::XamlIsland();
    m_islandXaml.Content(CreateXamlTree());

    // Create bridge to host parent island and connect.
    m_bridge = DesktopAttachedSiteBridge::CreateFromWindowId(
        m_compositor.DispatcherQueue(),
        winrt::Microsoft::UI::GetWindowIdFromWindow(hWnd));
    m_bridge.OverrideScale(1.0f);
    m_bridge.Connect(m_islandParent);

    // Create child site link in parent at one of its visuals and connect child.
    m_childSiteLink = winrt::MUCn::ChildSiteLink::Create(m_islandParent, parentFill);
    m_childSiteLink.Connect(m_islandXaml.ContentIsland());

    // Set link's properties.
    // This is an important for the child island to show up with a non-zero size.
#ifndef ABSOLUTE_SIZES
    m_childSiteLink.ActualSize(kWindowSize);
#else
    m_childSiteLink.ActualSize(m_visualChild.Size());
#endif // !ABSOLUTE_SIZES
    m_childSiteLink.LocalToParentTransformMatrix(m_islandXaml.Content().TransformMatrix());
}
