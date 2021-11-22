#pragma once

#include "MainPage.g.h"
#include "ActinidiaGame.h"

namespace winrt::GameHost::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Games();
		void Games(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& value);
	private:
		Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> m_games;

	public:
		void ButtonRunLocal_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
		void ButtonPlay_Click(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::GameHost::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
