﻿// WARNING: Please don't edit this file. It was generated by C++/WinRT v1.0.180821.2

#pragma once

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Data {

struct PropertyChangedEventHandler;

}

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Interop {

struct TypeName;

}

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Markup {

struct IXamlType;
struct XmlnsDefinition;

}

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Media {

struct ImageSource;

}

WINRT_EXPORT namespace winrt::Windows::UI::Xaml::Markup {

struct IXamlMetadataProvider;

}

WINRT_EXPORT namespace winrt::GameHost {

struct IActinidiaGame;
struct IActinidiaGameFactory;
struct IDirectXPage;
struct IMainPage;
struct ActinidiaGame;
struct DirectXPage;
struct MainPage;
struct XamlMetaDataProvider;

}

namespace winrt::impl {

template <> struct category<GameHost::IActinidiaGame>{ using type = interface_category; };
template <> struct category<GameHost::IActinidiaGameFactory>{ using type = interface_category; };
template <> struct category<GameHost::IDirectXPage>{ using type = interface_category; };
template <> struct category<GameHost::IMainPage>{ using type = interface_category; };
template <> struct category<GameHost::ActinidiaGame>{ using type = class_category; };
template <> struct category<GameHost::DirectXPage>{ using type = class_category; };
template <> struct category<GameHost::MainPage>{ using type = class_category; };
template <> struct category<GameHost::XamlMetaDataProvider>{ using type = class_category; };
template <> struct name<GameHost::IActinidiaGame>{ static constexpr auto & value{ L"GameHost.IActinidiaGame" }; };
template <> struct name<GameHost::IActinidiaGameFactory>{ static constexpr auto & value{ L"GameHost.IActinidiaGameFactory" }; };
template <> struct name<GameHost::IDirectXPage>{ static constexpr auto & value{ L"GameHost.IDirectXPage" }; };
template <> struct name<GameHost::IMainPage>{ static constexpr auto & value{ L"GameHost.IMainPage" }; };
template <> struct name<GameHost::ActinidiaGame>{ static constexpr auto & value{ L"GameHost.ActinidiaGame" }; };
template <> struct name<GameHost::DirectXPage>{ static constexpr auto & value{ L"GameHost.DirectXPage" }; };
template <> struct name<GameHost::MainPage>{ static constexpr auto & value{ L"GameHost.MainPage" }; };
template <> struct name<GameHost::XamlMetaDataProvider>{ static constexpr auto & value{ L"GameHost.XamlMetaDataProvider" }; };
template <> struct guid_storage<GameHost::IActinidiaGame>{ static constexpr guid value{ 0x7A4E1168,0xE84B,0x5612,{ 0x8F,0x9C,0x3C,0xDB,0x75,0x03,0x40,0xD2 } }; };
template <> struct guid_storage<GameHost::IActinidiaGameFactory>{ static constexpr guid value{ 0xFB388988,0x5148,0x5960,{ 0xAD,0xA7,0xB5,0xC0,0xEF,0x31,0x3F,0xEC } }; };
template <> struct guid_storage<GameHost::IDirectXPage>{ static constexpr guid value{ 0x39762274,0x1978,0x56FF,{ 0x9F,0x28,0x1E,0xE8,0x5F,0x20,0x7F,0x28 } }; };
template <> struct guid_storage<GameHost::IMainPage>{ static constexpr guid value{ 0x433E55B3,0x1FC3,0x56AA,{ 0x9E,0xB4,0xB9,0x38,0xC7,0x78,0xFE,0x58 } }; };
template <> struct default_interface<GameHost::ActinidiaGame>{ using type = GameHost::IActinidiaGame; };
template <> struct default_interface<GameHost::DirectXPage>{ using type = GameHost::IDirectXPage; };
template <> struct default_interface<GameHost::MainPage>{ using type = GameHost::IMainPage; };
template <> struct default_interface<GameHost::XamlMetaDataProvider>{ using type = Windows::UI::Xaml::Markup::IXamlMetadataProvider; };

template <> struct abi<GameHost::IActinidiaGame>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL get_Icon(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL get_Photo(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL get_Title(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL get_Subtitle(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL get_Description(void** value) noexcept = 0;
    virtual int32_t WINRT_CALL get_Rating(float* value) noexcept = 0;
};};

template <> struct abi<GameHost::IActinidiaGameFactory>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL CreateInstance(void* icon, void* photo, void* title, void* subtitle, void* description, float rating, void** value) noexcept = 0;
};};

template <> struct abi<GameHost::IDirectXPage>{ struct type : IInspectable
{
};};

template <> struct abi<GameHost::IMainPage>{ struct type : IInspectable
{
    virtual int32_t WINRT_CALL get_Games(void** value) noexcept = 0;
};};

template <typename D>
struct consume_GameHost_IActinidiaGame
{
    Windows::UI::Xaml::Media::ImageSource Icon() const;
    Windows::UI::Xaml::Media::ImageSource Photo() const;
    hstring Title() const;
    hstring Subtitle() const;
    hstring Description() const;
    float Rating() const;
};
template <> struct consume<GameHost::IActinidiaGame> { template <typename D> using type = consume_GameHost_IActinidiaGame<D>; };

template <typename D>
struct consume_GameHost_IActinidiaGameFactory
{
    GameHost::ActinidiaGame CreateInstance(Windows::UI::Xaml::Media::ImageSource const& icon, Windows::UI::Xaml::Media::ImageSource const& photo, param::hstring const& title, param::hstring const& subtitle, param::hstring const& description, float rating) const;
};
template <> struct consume<GameHost::IActinidiaGameFactory> { template <typename D> using type = consume_GameHost_IActinidiaGameFactory<D>; };

template <typename D>
struct consume_GameHost_IDirectXPage
{
};
template <> struct consume<GameHost::IDirectXPage> { template <typename D> using type = consume_GameHost_IDirectXPage<D>; };

template <typename D>
struct consume_GameHost_IMainPage
{
    Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Games() const;
};
template <> struct consume<GameHost::IMainPage> { template <typename D> using type = consume_GameHost_IMainPage<D>; };

}