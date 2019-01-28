#pragma once

#include "MainPage.g.h"

namespace winrt::GameHost::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage() = default;

        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Games();
    };
}

namespace winrt::GameHost::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
