#include "pch.h"
#include "ActinidiaGame.h"

namespace winrt::GameHost::implementation
{
    ActinidiaGame::ActinidiaGame(Windows::UI::Xaml::Media::ImageSource const& icon, Windows::UI::Xaml::Media::ImageSource const& photo, hstring const& title, hstring const& subtitle, hstring const& description, float rating)
    {
        throw hresult_not_implemented();
    }

    Windows::UI::Xaml::Media::ImageSource ActinidiaGame::Icon()
    {
        throw hresult_not_implemented();
    }

    Windows::UI::Xaml::Media::ImageSource ActinidiaGame::Photo()
    {
        throw hresult_not_implemented();
    }

    hstring ActinidiaGame::Title()
    {
        throw hresult_not_implemented();
    }

    hstring ActinidiaGame::Subtitle()
    {
        throw hresult_not_implemented();
    }

    hstring ActinidiaGame::Description()
    {
        throw hresult_not_implemented();
    }

    float ActinidiaGame::Rating()
    {
        throw hresult_not_implemented();
    }

    winrt::event_token ActinidiaGame::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        throw hresult_not_implemented();
    }

    void ActinidiaGame::PropertyChanged(winrt::event_token const& token) noexcept
    {
        throw hresult_not_implemented();
    }
}
