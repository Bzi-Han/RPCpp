#include "contract.h"

#include <iostream>

#include <Windows.h>

RPCService(Service);

RPCImplement(void, Service::normal, const std::string_view &string, uint32_t integer)
{
    std::cout << "[=] Service::normal => " << string << " " << integer << std::endl
              << std::endl;
}
RPCImplement(void, Service::container, const std::vector<std::list<std::wstring>> &strings)
{
    std::cout << "[=] Service::container => " << std::endl;

    for (const auto &stringList : strings)
    {
        for (const auto &string : stringList)
        {
            std::wcout << string << std::endl;
        }
    }

    std::cout << std::endl;
}
RPCImplement(uint32_t, Service::customData, const TrivialCustomData &customData01, const NonTrivialCustomData &customData02)
{
    std::cout << "[=] Service::customData => " << std::endl;

    std::cout << customData01.size << " " << customData01.name << std::endl;
    std::cout << customData02.playList.size() << std::endl;
    for (const auto &play : customData02.playList)
        std::cout << "play:" << play << std::endl;

    std::cout << std::endl;

    return 888;
}
RPCImplement(TrivialCustomResult, Service::customResult)
{
    std::cout << "[=] Service::customResult" << std::endl
              << std::endl;

    return {::GetCurrentProcessId(), SOCKET{}, ::GetCurrentProcess(), 114514};
}
RPCImplement(rpc::data::message, Service::customReadResult, Service::unordered_map_int_int data)
{
    rpc::data::message message;

    std::cout << "[=] Service::customReadResult => " << std::endl;

    for (const auto &[key, value] : data)
        std::cout << key << " => " << value << std::endl;

    message.writeValue<std::string_view>("hello");
    message.writeValue<std::wstring>(L"world");

    std::cout << std::endl;

    return message;
}

int main()
{
    rpc::start(true);

    getchar();

    rpc::stop();

    return 0;
}