#pragma once

#include "ActinidiaGame.g.h"

namespace winrt::GameHost::implementation
{
    struct ActinidiaGame : ActinidiaGameT<ActinidiaGame>
    {
        ActinidiaGame() = delete;
        ActinidiaGame(Windows::UI::Xaml::Media::ImageSource const& icon,
			Windows::UI::Xaml::Media::ImageSource const& photo,
			hstring const& title, hstring const& subtitle, hstring const& description, float rating);

        Windows::UI::Xaml::Media::ImageSource Icon();
        Windows::UI::Xaml::Media::ImageSource Photo();
        hstring Title();
        hstring Subtitle();
        hstring Description();
        float Rating();
        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(event_token const& token);
	private:
		Windows::UI::Xaml::Media::ImageSource m_icon;
		Windows::UI::Xaml::Media::ImageSource m_photo;
		winrt::hstring m_title;
		winrt::hstring m_subtitle;
		winrt::hstring m_description;
		float m_rating;
		winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}

namespace winrt::GameHost::factory_implementation
{
    struct ActinidiaGame : ActinidiaGameT<ActinidiaGame, implementation::ActinidiaGame>
    {
    };
}
