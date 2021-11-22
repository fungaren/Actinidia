#include "pch.h"
#include "SampleFpsTextRenderer.h"

#include "Common/DirectXHelper.h"

namespace winrt::GameHost::implementation
{
	// 初始化用于渲染文本的 D2D 资源。
	SampleFpsTextRenderer::SampleFpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_text(L""),
		m_deviceResources(deviceResources)
	{
		ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

		// 创建独立于设备的资源
		winrt::com_ptr<IDWriteTextFormat> textFormat = nullptr;
		winrt::check_hresult(
			m_deviceResources->GetDWriteFactory()->CreateTextFormat(
				L"Segoe UI",
				nullptr,
				DWRITE_FONT_WEIGHT_LIGHT,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				32.0f,
				L"en-US",
				textFormat.put()
			)
		);

		winrt::check_hresult(
			textFormat.try_as(m_textFormat)
		);

		winrt::check_hresult(
			m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);

		winrt::check_hresult(
			m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
		);

		CreateDeviceDependentResources();
	}

	// 更新要显示的文本。
	void SampleFpsTextRenderer::Update(DX::StepTimer const& timer)
	{
		// 更新显示文本。
		uint32_t fps = timer.GetFramesPerSecond();

		m_text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

		winrt::com_ptr<IDWriteTextLayout> textLayout = nullptr;
		winrt::check_hresult(
			m_deviceResources->GetDWriteFactory()->CreateTextLayout(
				m_text.c_str(),
				(uint32_t)m_text.length(),
				m_textFormat.get(),
				240.0f, // 输入文本的最大宽度。
				50.0f, // 输入文本的最大高度。
				textLayout.put()
			)
		);

		winrt::check_hresult(
			textLayout.try_as(m_textLayout)
		);

		winrt::check_hresult(
			m_textLayout->GetMetrics(&m_textMetrics)
		);
	}

	// 将帧呈现到屏幕。
	void SampleFpsTextRenderer::Render()
	{
		ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
		winrt::Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

		context->SaveDrawingState(m_stateBlock.get());
		context->BeginDraw();

		// 定位在右下角
		D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
			logicalSize.Width - m_textMetrics.layoutWidth,
			logicalSize.Height - m_textMetrics.height
		);

		context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());

		winrt::check_hresult(
			m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
		);

		context->DrawTextLayout(
			D2D1::Point2F(0.f, 0.f),
			m_textLayout.get(),
			m_whiteBrush.get()
		);

		// 此处忽略 D2DERR_RECREATE_TARGET。此错误指示该设备
		// 丢失。将在下一次调用 Present 时对其进行处理。
		HRESULT hr = context->EndDraw();
		if (hr != D2DERR_RECREATE_TARGET)
		{
			winrt::check_hresult(hr);
		}

		context->RestoreDrawingState(m_stateBlock.get());
	}

	void SampleFpsTextRenderer::CreateDeviceDependentResources()
	{
		winrt::check_hresult(
			m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), m_whiteBrush.put())
		);
	}
	void SampleFpsTextRenderer::ReleaseDeviceDependentResources()
	{
		m_whiteBrush = nullptr;

	}
}