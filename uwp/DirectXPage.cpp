#include "pch.h"
#include "DirectXPage.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics::Display;
using namespace winrt::Windows::System;
using namespace winrt::Windows::System::Threading;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Navigation;
using namespace concurrency;

using namespace GameHost;
using namespace GameHost::implementation;

namespace winrt::GameHost::implementation
{
	DirectXPage::DirectXPage() :
		m_windowVisible(true),
		m_coreInput(nullptr)
	{
		InitializeComponent();

		// 注册页面生命周期的事件处理程序。
		CoreWindow window = Window::Current().CoreWindow();

		windowVisibilityChanged_token = window.VisibilityChanged({ this, &DirectXPage::OnVisibilityChanged });

		DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();

		currentDisplayInformation.DpiChanged({ this, &DirectXPage::OnDpiChanged });

		currentDisplayInformation.OrientationChanged({ this, &DirectXPage::OnOrientationChanged });

		DisplayInformation::DisplayContentsInvalidated({ this, &DirectXPage::OnDisplayContentsInvalidated });

		swapChainPanel().CompositionScaleChanged({ this, &DirectXPage::OnCompositionScaleChanged });

		swapChainPanel().SizeChanged({ this, &DirectXPage::OnSwapChainPanelSizeChanged });

		// 此时，我们具有访问设备的权限。
		// 我们可创建与设备相关的资源。
		m_deviceResources = std::make_shared<DX::DeviceResources>();
		m_deviceResources->SetSwapChainPanel(swapChainPanel());

		// 注册我们的 SwapChainPanel 以获取独立的输入指针事件
		auto workItemHandler = WorkItemHandler([this](IAsyncAction const&)
		{
			// 对于指定的设备类型，无论它是在哪个线程上创建的，CoreIndependentInputSource 都将引发指针事件。
			m_coreInput = swapChainPanel().CreateCoreIndependentInputSource(
				Windows::UI::Core::CoreInputDeviceTypes::Mouse |
				Windows::UI::Core::CoreInputDeviceTypes::Touch |
				Windows::UI::Core::CoreInputDeviceTypes::Pen
			);

			// 指针事件的寄存器，将在后台线程上引发。
			m_coreInput.PointerPressed({ this, &DirectXPage::OnPointerPressed_ });
			m_coreInput.PointerMoved({ this, &DirectXPage::OnPointerMoved_ });
			m_coreInput.PointerReleased({ this, &DirectXPage::OnPointerReleased_ });

			// 一旦发送输入消息，即开始处理它们。
			m_coreInput.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
		});

		// 在高优先级的专用后台线程上运行任务。
		m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

		m_main = std::unique_ptr<HostMain>(new HostMain(m_deviceResources));
		m_main->StartRenderLoop();

		// 通过为 BackRequested 事件注册侦听器来处理来自从硬件和软件系统后退键的输入
		Windows::UI::Xaml::Input::KeyboardAccelerator goBack;
		goBack.Key(Windows::System::VirtualKey::GoBack);
		goBack.Invoked({ this, &DirectXPage::BackInvoked });
		KeyboardAccelerators().Append(goBack);

		Windows::UI::Xaml::Input::KeyboardAccelerator altLeft;
		altLeft.Key(Windows::System::VirtualKey::Left);
		altLeft.Invoked({ this, &DirectXPage::BackInvoked });
		KeyboardAccelerators().Append(altLeft);
		// ALT routes here.
		altLeft.Modifiers(Windows::System::VirtualKeyModifiers::Menu);
	}

	DirectXPage::~DirectXPage()
	{
		// 撤销跟窗口绑定的事件代理
		Window::Current().CoreWindow().VisibilityChanged(windowVisibilityChanged_token);

		// 析构时停止渲染和处理事件。
		m_main->StopRenderLoop();
		m_coreInput.Dispatcher().StopProcessEvents();
	}

	// 针对挂起和终止事件保存应用程序的当前状态。
	void DirectXPage::SaveInternalState(IPropertySet const& state)
	{
		critical_section::scoped_lock lock(m_main->GetCriticalSection());
		m_deviceResources->Trim();

		// 挂起应用程序时停止渲染。
		m_main->StopRenderLoop();

		// 在此处放置代码以保存应用程序状态。
	}

	// 针对恢复事件加载应用程序的当前状态。
	void DirectXPage::LoadInternalState(IPropertySet const& state)
	{
		// 在此处放置代码以加载应用程序状态。

		// 恢复应用程序时开始渲染。
		m_main->StartRenderLoop();
	}

	// 窗口事件处理程序。

	void DirectXPage::OnVisibilityChanged(CoreWindow const& sender, VisibilityChangedEventArgs const& args)
	{
		m_windowVisible = args.Visible();
		if (m_windowVisible)
		{
			m_main->StartRenderLoop();
		}
		else
		{
			m_main->StopRenderLoop();
		}
	}

	// DisplayInformation 事件处理程序。

	void DirectXPage::OnDpiChanged(DisplayInformation const& sender, IInspectable const& args)
	{
		critical_section::scoped_lock lock(m_main->GetCriticalSection());
		// 注意: 在此处检索到的 LogicalDpi 值可能与应用的有效 DPI 不匹配
		// 如果正在针对高分辨率设备对它进行缩放。在 DeviceResources 上设置 DPI 后，
		// 应始终使用 GetDpi 方法进行检索。
		// 有关详细信息，请参阅 DeviceResources.cpp。
		m_deviceResources->SetDpi(sender.LogicalDpi());
		m_main->CreateWindowSizeDependentResources();
	}

	void DirectXPage::OnOrientationChanged(DisplayInformation const& sender, IInspectable const& args)
	{
		critical_section::scoped_lock lock(m_main->GetCriticalSection());
		m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
		m_main->CreateWindowSizeDependentResources();
	}

	void DirectXPage::OnDisplayContentsInvalidated(DisplayInformation const& sender, IInspectable const& args)
	{
		critical_section::scoped_lock lock(m_main->GetCriticalSection());
		m_deviceResources->ValidateDevice();
	}

	void DirectXPage::OnPointerPressed_(IInspectable const& sender, PointerEventArgs const& e)
	{
		// 按下指针时开始跟踪指针移动。
		m_main->StartTracking();
	}

	void DirectXPage::OnPointerMoved_(IInspectable const& sender, PointerEventArgs const& e)
	{
		// 更新指针跟踪代码。
		if (m_main->IsTracking())
		{
			m_main->TrackingUpdate(e.CurrentPoint().Position().X);
		}
	}

	void DirectXPage::OnPointerReleased_(IInspectable const& sender, PointerEventArgs const& e)
	{
		// 释放指针时停止跟踪指针移动。
		m_main->StopTracking();
	}

	void DirectXPage::OnCompositionScaleChanged(SwapChainPanel const& sender, IInspectable const& args)
	{
		critical_section::scoped_lock lock(m_main->GetCriticalSection());
		m_deviceResources->SetCompositionScale(sender.CompositionScaleX(), sender.CompositionScaleY());
		m_main->CreateWindowSizeDependentResources();
	}

	void DirectXPage::OnSwapChainPanelSizeChanged(IInspectable const& sender, SizeChangedEventArgs const& e)
	{
		critical_section::scoped_lock lock(m_main->GetCriticalSection());
		m_deviceResources->SetLogicalSize(e.NewSize());
		m_main->CreateWindowSizeDependentResources();
	}
	//
	//void DirectXPage::OnNavigatedTo(NavigationEventArgs^ e)
	//{
	//
	//	::Windows::UI::Xaml::Controls::Page::OnNavigatedTo(e);
	//}

	void DirectXPage::BackInvoked(KeyboardAccelerator const& sender, KeyboardAcceleratorInvokedEventArgs const& args)
	{
		Controls::Frame f = Frame();
		if (f == nullptr) {
			auto content = Window::Current().Content();
			if (content)
			{
				f = content.try_as<Controls::Frame>();
			}
		}
		if (f.CanGoBack())
		{
			f.GoBack();
			args.Handled(true);
		}
		args.Handled(false);
	}
}
