#ifndef CONTRACT_H // !CONTRACT_H
#define CONTRACT_H

#include "rpc++.h"

struct TrivialCustomData
{
    char name[18] = "TrivialCustomData";
    size_t size = 17;
};

struct TrivialCustomResult
{
    uint32_t processId;
    SOCKET duplicateSocketHandle;
    HANDLE processHandle;
    uint64_t memoryUsage;
};

struct NonTrivialCustomData final : rpc::Serializable
{
    std::vector<std::string> playList;

    NonTrivialCustomData() {}
    NonTrivialCustomData(const std::vector<std::string> &data) : playList(data) {}

    virtual rpc::data::message serialization() override
    {
        rpc::data::message message;

        message.writeValue(playList);

        return message;
    }
    virtual void deserialization(rpc::data::message &data) override
    {
        playList = data.readValue<decltype(playList)>();
    }
};

struct Service
{
    using unordered_map_int_int = const std::unordered_map<int, int> &;

    virtual void normal(const std::string_view &string, uint32_t integer);
    virtual void container(const std::vector<std::list<std::wstring>> &strings);
    virtual uint32_t customData(const TrivialCustomData &customData01, const NonTrivialCustomData &customData02);
    virtual TrivialCustomResult customResult();
    virtual rpc::data::message customReadResult(unordered_map_int_int data);
};

#endif // !CONTRACT_H