#pragma once

#include "App.xaml.g.h"
#include "winrt/Microsoft.UI.Xaml.Hosting.h"

namespace winrt::Win32App::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager m_windowsXamlManager{ nullptr };
    };
}
