#pragma once

#include "DirectXPage.g.h"

namespace winrt::GameHost::implementation
{
    struct DirectXPage : DirectXPageT<DirectXPage>
    {
        DirectXPage() = default;

    };
}

namespace winrt::GameHost::factory_implementation
{
    struct DirectXPage : DirectXPageT<DirectXPage, implementation::DirectXPage>
    {
    };
}
