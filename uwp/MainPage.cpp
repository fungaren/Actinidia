#include "pch.h"
#include "MainPage.h"
#include "DirectXPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace GameHost;
using namespace GameHost::implementation;

namespace winrt::GameHost::implementation
{
	MainPage::MainPage()
	{
		m_games = single_threaded_observable_vector<IInspectable>();
		m_games.Append(make<ActinidiaGame>(nullptr, nullptr,
			hstring(L"Demo Title1"),
			hstring(L"Demo Subtitle"),
			hstring(L"Demo Description"),
			1.0f
			));
		m_games.Append(make<ActinidiaGame>(nullptr, nullptr,
			hstring(L"Demo Title2"),
			hstring(L"Demo Subtitle2"),
			hstring(L"Demo Description"),
			1.0f
			));

		InitializeComponent();
	}

	IObservableVector<IInspectable> MainPage::Games()
	{
		return m_games;
	}

	void MainPage::ButtonRunLocal_Click(IInspectable const& sender, RoutedEventArgs const& e)
	{
		using namespace Windows::UI::Popups;
        int32_t id = GamelistView().SelectedIndex();
        if (id == -1) 
            return;
        auto dlg = MessageDialog(m_games.GetAt(id)
            .try_as<ActinidiaGame>()->Title());
        dlg.ShowAsync();
	}

	void MainPage::ButtonPlay_Click(IInspectable const& sender, RoutedEventArgs const& e)
	{
		Frame().Navigate(xaml_typename<GameHost::DirectXPage>(), box_value(e));

		//using namespace Windows::ApplicationModel::Core;
		//using namespace Windows::UI::Core;
		//using namespace Windows::UI::Xaml::Controls;
		//using namespace Windows::UI::ViewManagement;
		//// 为视图内容创建新窗口和线程。
		//CoreApplicationView^ newView = CoreApplication::CreateNewView();
		//// 跟踪新视图的 Id。 稍后使用此选项来显示视图。
		//int newViewId = 0;
		//// 在新视图的 UI 线程上安排工作。
		//newView->Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, &newViewId]()
		//{
		//	// 向 Window 的 Content 属性添加 Frame，然后将 Frame 导航到你已为其定义应用内容的 XAML Page。
		//	Frame^ frame = ref new Frame();
		//	frame->Navigate(Interop::TypeName(DirectXPage::typeid), nullptr);
		//	Window::Current->Content = frame;
		//	// You have to activate the window in order to show it later.
		//	// 在填充新 Window 后，必须调用 Window 的 Activate 方法才能在稍后显示 Window。
		//	// 这项工作在新视图的线程上进行，因此会激活新 Window。
		//	Window::Current->Activate();
		//	// 最后，获取新视图的 Id，用于稍后显示该视图。
		//	newViewId = ApplicationView::GetForCurrentView()->Id;
		//}));
		//// 在创建新视图之后将其显示在新窗口中。此方法的 viewId 参数是唯一标识你的应用中每个视图的整数。
		//bool viewShown = ApplicationViewSwitcher::TryShowAsStandaloneAsync(newViewId);
	}

}
