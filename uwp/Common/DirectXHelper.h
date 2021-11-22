#pragma once

#include <ppltasks.h>	// 对于 create_task

namespace DX
{
	// 从二进制文件中执行异步读取操作的函数。
	inline Concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
	{
		using namespace winrt::Windows::Storage;
		using namespace Concurrency;

		auto folder = winrt::Windows::ApplicationModel::Package::Current().InstalledLocation();

		return create_task([=]()->std::vector<byte>
		{
			StorageFile file = folder.GetFileAsync(winrt::hstring(filename)).get();
			Streams::IBuffer const fileBuffer = FileIO::ReadBufferAsync(file).get();
			std::vector<byte> returnBuffer;
			returnBuffer.resize(fileBuffer.Length());
			Streams::DataReader::FromBuffer(fileBuffer).ReadBytes(
				winrt::array_view<byte>(returnBuffer.data(), returnBuffer.data() + fileBuffer.Length())
			);
			return returnBuffer;
		});
	}

	// 将使用与设备无关的像素(DIP)表示的长度转换为使用物理像素表示的长度。
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // 舍入到最接近的整数。
	}

#if defined(_DEBUG)
	// 请检查 SDK 层支持。
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // 无需创建实际硬件设备。
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // 请检查 SDK 层。
			nullptr,                    // 任何功能级别都会这样。
			0,
			D3D11_SDK_VERSION,          // 对于 Windows 应用商店应用，始终将此值设置为 D3D11_SDK_VERSION。
			nullptr,                    // 无需保留 D3D 设备引用。
			nullptr,                    // 无需知道功能级别。
			nullptr                     // 无需保留 D3D 设备上下文引用。
			);

		return SUCCEEDED(hr);
	}
#endif
}
