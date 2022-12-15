#include "contract.h"

#include <iostream>

struct RPCService final
{
    RPCInvoke(void, normal, const std::string_view &, uint32_t);
    RPCInvoke(void, container, const std::vector<std::list<std::wstring>> &);
    RPCInvoke(uint32_t, customData, const TrivialCustomData &, const NonTrivialCustomData &);
    RPCInvoke(TrivialCustomResult, customResult);
    RPCInvoke(rpc::data::message, customReadResult, Service::unordered_map_int_int);
} service;

int main()
{
    rpc::start();
    getchar();

    try
    {
        service.normal("123", 321);

        service.container({
            {
                L"data01",
                L"data02",
            },
            {
                L"data03",
            },
        });

        auto result01 = service.customData(
            {},
            std::vector<std::string>{
                "Rain",
                "BLAZE",
            });
        std::cout << "[=] service.customData => " << result01 << std::endl;

        auto result02 = service.customResult();
        std::cout << "[=] service.customResult => "
                  << result02.processId << " "
                  << result02.duplicateSocketHandle << " "
                  << result02.processHandle << " "
                  << result02.memoryUsage << std::endl;

        auto result03 = service.customReadResult({{123, 456}, {456, 789}, {789, 101112}});
        std::cout << "[=] service.customReadResult => " << std::endl;
        std::cout << "[=] " << result03.readValue<std::string_view>() << std::endl;
        std::wcout << "[=] " << result03.readValue<std::wstring>() << std::endl;

        result03.readValue<size_t>(); // throw out of range exception
    }
    catch (const std::exception &e)
    {
        std::cerr << "[-] " << e.what() << '\n';
    }

    getchar();
    rpc::stop();

    return 0;
}