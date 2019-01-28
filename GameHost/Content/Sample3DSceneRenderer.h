#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

namespace winrt::GameHost::implementation
{
	// 此示例呈现器实例化一个基本渲染管道。
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// 缓存的设备资源指针。
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// 立体几何的 Direct3D 资源。
		winrt::com_ptr<ID3D11InputLayout>	m_inputLayout{ nullptr };
		winrt::com_ptr<ID3D11Buffer>		m_vertexBuffer{ nullptr };
		winrt::com_ptr<ID3D11Buffer>		m_indexBuffer{ nullptr };
		winrt::com_ptr<ID3D11VertexShader>	m_vertexShader{ nullptr };
		winrt::com_ptr<ID3D11PixelShader>	m_pixelShader{ nullptr };
		winrt::com_ptr<ID3D11Buffer>		m_constantBuffer{ nullptr };

		// 立体几何的系统资源。
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32_t	m_indexCount;

		// 用于渲染循环的变量。
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

