#pragma once

namespace DX
{
	// 提供一个接口，以使拥有 DeviceResources 的应用程序在设备丢失或创建时收到通知。
	interface IDeviceNotify
	{
		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceRestored() = 0;
	};

	// 控制所有 DirectX 设备资源。
	class DeviceResources
	{
	public:
		DeviceResources();
		void SetSwapChainPanel(winrt::Windows::UI::Xaml::Controls::SwapChainPanel const& panel);
		void SetLogicalSize(winrt::Windows::Foundation::Size logicalSize);
		void SetCurrentOrientation(winrt::Windows::Graphics::Display::DisplayOrientations currentOrientation);
		void SetDpi(float dpi);
		void SetCompositionScale(float compositionScaleX, float compositionScaleY);
		void ValidateDevice();
		void HandleDeviceLost();
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify);
		void Trim();
		void Present();

		// 呈现器目标的大小，以像素为单位。
		winrt::Windows::Foundation::Size	GetOutputSize() const			{ return m_outputSize; }

		// 呈现器目标的大小，以 dip 为单位。
		winrt::Windows::Foundation::Size	GetLogicalSize() const			{ return m_logicalSize; }
		float						GetDpi() const							{ return m_effectiveDpi; }

		// D3D 访问器。
		ID3D11Device3*				GetD3DDevice() const					{ return m_d3dDevice.get(); }
		ID3D11DeviceContext3*		GetD3DDeviceContext() const				{ return m_d3dContext.get(); }
		IDXGISwapChain3*			GetSwapChain() const					{ return m_swapChain.get(); }
		D3D_FEATURE_LEVEL			GetDeviceFeatureLevel() const			{ return m_d3dFeatureLevel; }
		ID3D11RenderTargetView1*	GetBackBufferRenderTargetView() const	{ return m_d3dRenderTargetView.get(); }
		ID3D11DepthStencilView*		GetDepthStencilView() const				{ return m_d3dDepthStencilView.get(); }
		D3D11_VIEWPORT				GetScreenViewport() const				{ return m_screenViewport; }
		DirectX::XMFLOAT4X4			GetOrientationTransform3D() const		{ return m_orientationTransform3D; }

		// D2D 访问器。
		ID2D1Factory3*				GetD2DFactory() const					{ return m_d2dFactory.get(); }
		ID2D1Device2*				GetD2DDevice() const					{ return m_d2dDevice.get(); }
		ID2D1DeviceContext2*		GetD2DDeviceContext() const				{ return m_d2dContext.get(); }
		ID2D1Bitmap1*				GetD2DTargetBitmap() const				{ return m_d2dTargetBitmap.get(); }
		IDWriteFactory3*			GetDWriteFactory() const				{ return m_dwriteFactory.get(); }
		IWICImagingFactory2*		GetWicImagingFactory() const			{ return m_wicFactory.get(); }
		D2D1::Matrix3x2F			GetOrientationTransform2D() const		{ return m_orientationTransform2D; }

	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void UpdateRenderTargetSize();
		DXGI_MODE_ROTATION ComputeDisplayRotation();

		// Direct3D 对象。
		winrt::com_ptr<ID3D11Device3>			m_d3dDevice{ nullptr };
		winrt::com_ptr<ID3D11DeviceContext3>	m_d3dContext{ nullptr };
		winrt::com_ptr<IDXGISwapChain3>			m_swapChain{ nullptr };

		// Direct3D 渲染对象。3D 所必需的。
		winrt::com_ptr<ID3D11RenderTargetView1>	m_d3dRenderTargetView{ nullptr };
		winrt::com_ptr<ID3D11DepthStencilView>	m_d3dDepthStencilView{ nullptr };
		D3D11_VIEWPORT							m_screenViewport;

		// Direct2D 绘制组件。
		winrt::com_ptr<ID2D1Factory3>		m_d2dFactory{ nullptr };
		winrt::com_ptr<ID2D1Device2>		m_d2dDevice{ nullptr };
		winrt::com_ptr<ID2D1DeviceContext2>	m_d2dContext{ nullptr };
		winrt::com_ptr<ID2D1Bitmap1>		m_d2dTargetBitmap{ nullptr };

		// DirectWrite 绘制组件。
		winrt::com_ptr<IDWriteFactory3>		m_dwriteFactory{ nullptr };
		winrt::com_ptr<IWICImagingFactory2>	m_wicFactory{ nullptr };

		// 对 XAML 面板的缓存引用。
		winrt::Windows::UI::Xaml::Controls::SwapChainPanel	m_swapChainPanel{nullptr};

		// 缓存的设备属性。
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel;
		winrt::Windows::Foundation::Size						m_d3dRenderTargetSize;
		winrt::Windows::Foundation::Size						m_outputSize;
		winrt::Windows::Foundation::Size						m_logicalSize;
		winrt::Windows::Graphics::Display::DisplayOrientations	m_nativeOrientation;
		winrt::Windows::Graphics::Display::DisplayOrientations	m_currentOrientation;
		float											m_dpi;
		float											m_compositionScaleX;
		float											m_compositionScaleY;

		// 考虑应用是否支持高分辨率屏幕的变量。
		float											m_effectiveDpi;
		float											m_effectiveCompositionScaleX;
		float											m_effectiveCompositionScaleY;

		// 用于显示方向的转换。
		D2D1::Matrix3x2F	m_orientationTransform2D;
		DirectX::XMFLOAT4X4	m_orientationTransform3D;

		// IDeviceNotify 可直接保留，因为它拥有 DeviceResources。
		IDeviceNotify* m_deviceNotify;
	};
}