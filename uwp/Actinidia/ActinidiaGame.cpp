#include "pch.h"
#include "ActinidiaGame.h"

namespace winrt::GameHost::implementation
{
	ActinidiaGame::ActinidiaGame(Windows::UI::Xaml::Media::ImageSource const& icon,
		Windows::UI::Xaml::Media::ImageSource const& photo,
		hstring const& title, hstring const& subtitle, hstring const& description, float rating) :
		m_icon(icon),
		m_photo(photo),
		m_title(title),
		m_subtitle(subtitle),
		m_description(description)
	{
		if (rating < 0) rating = -1;
		else if (rating > 5) rating = 5;
	}

    Windows::UI::Xaml::Media::ImageSource ActinidiaGame::Icon()
    {
		return m_icon;
    }

    Windows::UI::Xaml::Media::ImageSource ActinidiaGame::Photo()
    {
		return m_photo;
    }

    hstring ActinidiaGame::Title()
    {
		return m_title;
    }

    hstring ActinidiaGame::Subtitle()
    {
		return m_subtitle;
    }

    hstring ActinidiaGame::Description()
    {
		return m_description;
    }

    float ActinidiaGame::Rating()
    {
		return m_rating;
    }

    event_token ActinidiaGame::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value)
    {
		return m_propertyChanged.add(value);
    }

    void ActinidiaGame::PropertyChanged(event_token const& token)
    {
		m_propertyChanged.remove(token);
    }
}
