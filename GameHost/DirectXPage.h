#pragma once

#include "DirectXPage.g.h"

#include "Common\DeviceResources.h"
#include "HostMain.h"

namespace winrt::GameHost::implementation
{
	/// <summary>
	///包含 DirectX SwapChainPanel 的页。
	/// </summary>
	struct DirectXPage : DirectXPageT<DirectXPage>
	{
		DirectXPage();
		virtual ~DirectXPage();

		void SaveInternalState(Windows::Foundation::Collections::IPropertySet const& state);
		void LoadInternalState(Windows::Foundation::Collections::IPropertySet const& state);

	private:
		// XAML 低级渲染事件处理程序。
		void OnRendering(Windows::UI::Xaml::DependencyObject const& sender, Windows::UI::Xaml::DependencyObject const& args);

		// 窗口事件处理程序。
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow const& sender, Windows::UI::Core::VisibilityChangedEventArgs const& args);

		// DisplayInformation 事件处理程序。
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation const& sender, Windows::Foundation::IInspectable const& args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation const& sender, Windows::Foundation::IInspectable const& args);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation const& sender, Windows::Foundation::IInspectable const& args);

		// 其他事件处理程序。
		void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel const& sender, Windows::Foundation::IInspectable const& args);
		void OnSwapChainPanelSizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::SizeChangedEventArgs const& e);

		// 在后台工作线程上跟踪我们的独立输入。
		Windows::Foundation::IAsyncAction m_inputLoopWorker{nullptr};
		Windows::UI::Core::CoreIndependentInputSource m_coreInput{ nullptr };

		// 独立输入处理函数。
		void OnPointerPressed_(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::PointerEventArgs const& e);
		void OnPointerMoved_(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::PointerEventArgs const& e);
		void OnPointerReleased_(Windows::Foundation::IInspectable const& sender, Windows::UI::Core::PointerEventArgs const& e);

		// 用于在 XAML 页面背景中呈现 DirectX 内容的资源。
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::unique_ptr<HostMain> m_main;
		bool m_windowVisible;

		event_token windowVisibilityChanged_token;
		//virtual void OnNavigatedTo(NavigationEventArgs const& e) override;
		void BackInvoked(Windows::UI::Xaml::Input::KeyboardAccelerator const& sender, Windows::UI::Xaml::Input::KeyboardAcceleratorInvokedEventArgs const& args);

	};
}

namespace winrt::GameHost::factory_implementation
{
	struct DirectXPage : DirectXPageT<DirectXPage, implementation::DirectXPage>
	{

	};
}