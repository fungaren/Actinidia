#pragma once

#include "ActinidiaGame.g.h"

namespace winrt::GameHost::implementation
{
    struct ActinidiaGame : ActinidiaGameT<ActinidiaGame>
    {
        ActinidiaGame() = delete;
        ActinidiaGame(Windows::UI::Xaml::Media::ImageSource const& icon, Windows::UI::Xaml::Media::ImageSource const& photo, hstring const& title, hstring const& subtitle, hstring const& description, float rating);

        Windows::UI::Xaml::Media::ImageSource Icon();
        Windows::UI::Xaml::Media::ImageSource Photo();
        hstring Title();
        hstring Subtitle();
        hstring Description();
        float Rating();
        winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;
    };
}

namespace winrt::GameHost::factory_implementation
{
    struct ActinidiaGame : ActinidiaGameT<ActinidiaGame, implementation::ActinidiaGame>
    {
    };
}
