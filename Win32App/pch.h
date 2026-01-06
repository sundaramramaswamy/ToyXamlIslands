// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"

// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// This tells App.xaml.g.hpp the app will define its own WinMain
#define DISABLE_XAML_GENERATED_MAIN

// There's an API named GetCurrentTime in the Storyboard type.
#undef GetCurrentTime

// Com and WinRT headers
#include <Unknwn.h>

// init_apartment
#include <winrt/base.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Composition.Interactions.h>

#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Dispatching.Interop.h>

namespace winrt
{
    namespace MU = Microsoft::UI;
    namespace MUD = Microsoft::UI::Dispatching;
    namespace MUC = Microsoft::UI::Composition;
    namespace MUCn = Microsoft::UI::Content;
    namespace MUI = Microsoft::UI::Input;
    namespace MUCi = winrt::Microsoft::UI::Composition::Interactions;

    namespace Xaml = winrt::Microsoft::UI::Xaml;
    namespace Controls = winrt::Microsoft::UI::Xaml::Controls;

    namespace Text = winrt::Windows::UI::Text;
    namespace Numerics = winrt::Windows::Foundation::Numerics;
}