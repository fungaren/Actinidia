#pragma once
#include "App.xaml.g.h"

namespace winrt::GameHost::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
        void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
		void OnResuming(IInspectable const&, IInspectable const&);
        void OnNavigationFailed(IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const&);
	private:
		//std::unique_ptr<DirectXPage> m_directXPage;
    };
}
