﻿#pragma once

#include <string>
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

namespace winrt::GameHost::implementation
{
	// 使用 Direct2D 和 DirectWrite 在屏幕右下角呈现当前的 FPS 值。
	class SampleFpsTextRenderer
	{
	public:
		SampleFpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		// 缓存的设备资源指针。
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// 与文本渲染相关的资源。
		std::wstring                                    m_text;
		DWRITE_TEXT_METRICS	                            m_textMetrics;
		winrt::com_ptr<ID2D1SolidColorBrush>    m_whiteBrush{ nullptr };
		winrt::com_ptr<ID2D1DrawingStateBlock1> m_stateBlock{ nullptr };
		winrt::com_ptr<IDWriteTextLayout3>      m_textLayout{ nullptr };
		winrt::com_ptr<IDWriteTextFormat2>      m_textFormat{ nullptr };
	};
}