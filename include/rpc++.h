#ifndef RPC_CPP_H // !RPC_CPP_H
#define RPC_CPP_H

#include "ThreadPool.h"

#include <yasio/yasio.hpp>
#include <yasio/ibstream.hpp>
#include <yasio/obstream.hpp>

#include <algorithm>
#include <memory>
#include <future>
#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

namespace rpc
{
    namespace exception
    {
        struct message_incomplete_exception : std::exception
        {
            message_incomplete_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct call_method_notfound_exception : std::exception
        {
            call_method_notfound_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct service_provider_notfound_exception : std::exception
        {
            service_provider_notfound_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct parameter_type_error_exception : std::exception
        {
            parameter_type_error_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct service_no_connected_exception : std::exception
        {
            service_no_connected_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct service_not_avaliable_exception : std::exception
        {
            service_not_avaliable_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct message_response_exception : std::exception
        {
            message_response_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct invalid_message_exception : std::exception
        {
            invalid_message_exception(const char *const message)
                : std::exception(message)
            {
            }
        };

        struct unsupported_platform_exception : std::exception
        {
            unsupported_platform_exception(const char *const message)
                : std::exception(message)
            {
            }
        };
    }

    namespace calc
    {
        template <size_t n>
        struct const_function_name
        {
            constexpr const_function_name(const char (&prettyFunction)[n])
            {
                size_t s = 0;

                if ('R' == prettyFunction[0] && 'P' == prettyFunction[1] && 'C' == prettyFunction[2])
                {
                    std::copy_n(prettyFunction + 3, n - 3, name);
                    size = n - 4;
                    return;
                }

                for (size_t i = 0; i < n; ++i)
                {
                    if (s != 0 && '(' == prettyFunction[i])
                    {
                        std::copy_n(prettyFunction + s, i - s, name);
                        size = i - s;
                        break;
                    }

                    if (' ' == prettyFunction[i])
                    {
                        if ('R' == prettyFunction[i + 1] && 'P' == prettyFunction[i + 2] && 'C' == prettyFunction[i + 3])
                            i += 3;

                        s = i + 1;
                    }
                }
            }

            char name[n]{};
            size_t size{};
        };

        template <size_t n>
        constexpr inline uint32_t const_hash(const char (&data)[n]) noexcept
        {
            constexpr uint32_t fnvOffsetBasis = 2166136261U;
            constexpr uint32_t fnvPrime = 16777619U;
            uint32_t result = fnvOffsetBasis;

            for (size_t i = 0; i < n - 1; ++i)
            {
                result ^= static_cast<uint32_t>(data[i]);
                result *= fnvPrime;
            }

            return result;
        }

        template <size_t n>
        constexpr inline uint32_t const_hash_class(const char (&data)[n]) noexcept
        {
            constexpr uint32_t fnvOffsetBasis = 2166136261U;
            constexpr uint32_t fnvPrime = 16777619U;
            uint32_t result = fnvOffsetBasis;

            for (size_t i = 0; i < n - 1 && ':' != data[i]; ++i)
            {
                result ^= static_cast<uint32_t>(data[i]);
                result *= fnvPrime;
            }

            return result;
        }

        template <const_function_name methodName>
        constexpr inline uint32_t const_hash_macro()
        {
            constexpr uint32_t fnvOffsetBasis = 2166136261U;
            constexpr uint32_t fnvPrime = 16777619U;
            uint32_t result = fnvOffsetBasis;

            for (size_t i = 0; i < methodName.size; ++i)
            {
                result ^= static_cast<uint32_t>(methodName.name[i]);
                result *= fnvPrime;
            }

            return result;
        }
    }

    namespace data
    {
        static thread_local std::vector<void *> customDataCache;

        typedef enum class MessageType : uint8_t
        {
            tbool,
            tint8,
            tint16,
            tint32,
            tint64,
            tfloat,
            tdouble,
            tstring,
            tvector,
            tlist,
            tstack,
            tqueue,
            tdeque,
            thashset,
            tmultiset,
            thashmap,
            tmultimap,
            tcustom,
            tmessage,
        } message_t;

        class Message;

        class Serializable;

        template <typename any_t>
        struct data_trait;

        /******************************base type******************************/
        template <>
        struct data_trait<bool>
        {
            inline static bool read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tbool != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tbool");
                }

                return ibs.read<bool>();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, bool data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tbool));
                obs.write<bool>(data);
            }
        };

        template <>
        struct data_trait<uint8_t>
        {
            inline static uint8_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tint8 != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tint8");
                }

                return ibs.read<uint8_t>();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, uint8_t data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tint8));
                obs.write<uint8_t>(data);
            }
        };

        template <>
        struct data_trait<uint16_t>
        {
            inline static uint16_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tint16 != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tint16");
                }

                return ibs.read<uint16_t>();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, uint16_t data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tint16));
                obs.write<uint16_t>(data);
            }
        };

        template <>
        struct data_trait<uint32_t>
        {
            inline static uint32_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tint32 != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tint32");
                }

                return ibs.read<uint32_t>();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, uint32_t data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tint32));
                obs.write<uint32_t>(data);
            }
        };

        template <>
        struct data_trait<uint64_t>
        {
            inline static uint64_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tint64 != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tint64");
                }

                return ibs.read<uint64_t>();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, uint64_t data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tint64));
                obs.write<uint64_t>(data);
            }
        };

        template <>
        struct data_trait<float>
        {
            inline static float read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tfloat != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tfloat");
                }

                return ibs.read<float>();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, float data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tfloat));
                obs.write<float>(data);
            }
        };

        template <>
        struct data_trait<double>
        {
            inline static double read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tdouble != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tdouble");
                }

                return ibs.read<double>();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, double data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tdouble));
                obs.write<double>(data);
            }
        };

        template <>
        struct data_trait<std::string_view>
        {
            inline static std::string_view read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tstring != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tstring");
                }

                return ibs.read_v();
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::string_view &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tstring));
                obs.write_v(data);
            }
        };

        /******************************wrap type******************************/
        template <>
        struct data_trait<int8_t>
        {
            inline static int8_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                return static_cast<int8_t>(data_trait<uint8_t>::read(ibs, hasType));
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, int8_t data, bool hasType = true)
            {
                data_trait<uint8_t>::write(obs, data, hasType);
            }
        };

        template <>
        struct data_trait<int16_t>
        {
            inline static int16_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                return static_cast<int16_t>(data_trait<uint16_t>::read(ibs, hasType));
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, int16_t data, bool hasType = true)
            {
                data_trait<uint16_t>::write(obs, data, hasType);
            }
        };

        template <>
        struct data_trait<int32_t>
        {
            inline static int32_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                return static_cast<int32_t>(data_trait<uint32_t>::read(ibs, hasType));
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, int32_t data, bool hasType = true)
            {
                data_trait<uint32_t>::write(obs, data, hasType);
            }
        };

        template <>
        struct data_trait<int64_t>
        {
            inline static int64_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                return static_cast<int64_t>(data_trait<uint64_t>::read(ibs, hasType));
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, int64_t data, bool hasType = true)
            {
                data_trait<uint64_t>::write(obs, data, hasType);
            }
        };

        template <>
        struct data_trait<std::string>
        {
            inline static std::string read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                auto result = data_trait<std::string_view>::read(ibs, hasType);

                return {result.begin(), result.end()};
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::string &data, bool hasType = true)
            {
                data_trait<std::string_view>::write(obs, data, hasType);
            }
        };

        template <>
        struct data_trait<std::wstring>
        {
            inline static std::wstring read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::wstring result;

                auto wcharSize = ibs.read_byte();
                if (1 > wcharSize)
                    throw exception::message_incomplete_exception("no wchar_t size info");

                auto rawResult = data_trait<std::string_view>::read(ibs, hasType);
                result.resize(rawResult.size() / wcharSize);

                if (sizeof(wchar_t) != wcharSize)
                {
                    if (6 != sizeof(wchar_t) + wcharSize)
                        throw exception::unsupported_platform_exception("unsupported platform");

                    for (size_t i = 0; i < result.size(); ++i)
                    {
                        if constexpr (4 == sizeof(wchar_t))
                            result[i] = reinterpret_cast<const uint16_t *>(rawResult.data())[i];
                        else
                            result[i] = reinterpret_cast<const uint32_t *>(rawResult.data())[i];
                    }
                }
                else
                    memcpy(result.data(), rawResult.data(), rawResult.size());

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::wstring &data, bool hasType = true)
            {
                obs.write_byte(sizeof(wchar_t));
                data_trait<std::string_view>::write(obs, {reinterpret_cast<const char *>(data.data()), data.length() * sizeof(wchar_t)}, hasType);
            }
        };

        template <>
        struct data_trait<char *>
        {
            inline static char *read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                auto result = data_trait<std::string_view>::read(ibs, hasType);

                return const_cast<char *>(result.data());
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::string &data, bool hasType = true)
            {
                data_trait<std::string_view>::write(obs, data, hasType);
            }
        };

        template <>
        struct data_trait<wchar_t *>
        {
            inline static wchar_t *read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                auto result = data_trait<std::wstring>::read(ibs, hasType);

                return const_cast<wchar_t *>(result.data());
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, wchar_t *data, bool hasType = true)
            {
                data_trait<std::wstring>::write(obs, data, hasType);
            }
        };

        /******************************container wrap type******************************/
        template <typename any_t>
        struct data_trait<std::vector<any_t>>
        {
            inline static std::vector<any_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::vector<any_t> result;

                if (hasType)
                {
                    if (message_t::tvector != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tvector");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.push_back(data_trait<std::remove_cvref_t<any_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::vector<any_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tvector));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                for (size_t i = 0; i < data.size(); ++i)
                    data_trait<std::remove_cvref_t<any_t>>::write(obs, data[i], false);
            }
        };

        template <typename any_t>
        struct data_trait<std::list<any_t>>
        {
            inline static std::list<any_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::list<any_t> result;

                if (hasType)
                {
                    if (message_t::tlist != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tlist");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.push_back(data_trait<std::remove_cvref_t<any_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::list<any_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tlist));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                for (const auto &value : data)
                    data_trait<std::remove_cvref_t<any_t>>::write(obs, value, false);
            }
        };

        template <typename any_t>
        struct data_trait<std::stack<any_t>>
        {
            inline static std::stack<any_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::stack<any_t> result;

                if (hasType)
                {
                    if (message_t::tstack != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tstack");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.push(data_trait<std::remove_cvref_t<any_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, std::stack<any_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tstack));

                std::stack<any_t> reverseData;
                while (!data.empty())
                {
                    reverseData.push(data.top());
                    data.pop();
                }

                obs.write_ix<int32_t>(static_cast<int32_t>(reverseData.size()));
                while (!reverseData.empty())
                {
                    data_trait<std::remove_cvref_t<any_t>>::write(obs, reverseData.top(), false);
                    reverseData.pop();
                }
            }
        };

        template <typename any_t>
        struct data_trait<std::queue<any_t>>
        {
            inline static std::queue<any_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::queue<any_t> result;

                if (hasType)
                {
                    if (message_t::tqueue != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tqueue");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.push(data_trait<std::remove_cvref_t<any_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, std::queue<any_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tqueue));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                while (!data.empty())
                {
                    data_trait<std::remove_cvref_t<any_t>>::write(obs, data.front(), false);
                    data.pop();
                }
            }
        };

        template <typename any_t>
        struct data_trait<std::deque<any_t>>
        {
            inline static std::deque<any_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::deque<any_t> result;

                if (hasType)
                {
                    if (message_t::tdeque != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tdeque");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.push_back(data_trait<std::remove_cvref_t<any_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, std::deque<any_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tdeque));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                while (!data.empty())
                {
                    data_trait<std::remove_cvref_t<any_t>>::write(obs, data.front(), false);
                    data.pop_front();
                }
            }
        };

        template <typename any_t>
        struct data_trait<std::unordered_set<any_t>>
        {
            inline static std::unordered_set<any_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::unordered_set<any_t> result;

                if (hasType)
                {
                    if (message_t::thashset != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: thashset");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.insert(data_trait<std::remove_cvref_t<any_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::unordered_set<any_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::thashset));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                for (const auto &value : data)
                    data_trait<std::remove_cvref_t<any_t>>::write(obs, value, false);
            }
        };

        template <typename any_t>
        struct data_trait<std::set<any_t>>
        {
            inline static std::set<any_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::set<any_t> result;

                if (hasType)
                {
                    if (message_t::tmultiset != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tmultiset");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.insert(data_trait<std::remove_cvref_t<any_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::set<any_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tmultiset));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                for (const auto &value : data)
                    data_trait<std::remove_cvref_t<any_t>>::write(obs, value, false);
            }
        };

        template <typename key_t, typename value_t>
        struct data_trait<std::unordered_map<key_t, value_t>>
        {
            inline static std::unordered_map<key_t, value_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::unordered_map<key_t, value_t> result;

                if (hasType)
                {
                    if (message_t::thashmap != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: thashmap");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.emplace(data_trait<std::remove_cvref_t<key_t>>::read(ibs, false), data_trait<std::remove_cvref_t<value_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::unordered_map<key_t, value_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::thashmap));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                for (const auto &[key, value] : data)
                {
                    data_trait<std::remove_cvref_t<value_t>>::write(obs, value, false);
                    data_trait<std::remove_cvref_t<key_t>>::write(obs, key, false);
                }
            }
        };

        template <typename key_t, typename value_t>
        struct data_trait<std::map<key_t, value_t>>
        {
            inline static std::map<key_t, value_t> read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                std::map<key_t, value_t> result;

                if (hasType)
                {
                    if (message_t::tmultimap != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tmultimap");
                }

                auto size = ibs.read_ix<int32_t>();
                for (size_t i = 0; i < size; ++i)
                    result.emplace(data_trait<std::remove_cvref_t<key_t>>::read(ibs, false), data_trait<std::remove_cvref_t<value_t>>::read(ibs, false));

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const std::map<key_t, value_t> &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tmultimap));
                obs.write_ix<int32_t>(static_cast<int32_t>(data.size()));
                for (const auto &[key, value] : data)
                {
                    data_trait<std::remove_cvref_t<value_t>>::write(obs, value, false);
                    data_trait<std::remove_cvref_t<key_t>>::write(obs, key, false);
                }
            }
        };

        /******************************another wrap type******************************/
        template <>
        struct data_trait<void *>
        {
            inline static void *read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                static_assert(true, "Unsupported parameter type void*");

                return nullptr;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, void *data, bool hasType = true)
            {
                static_assert(true, "Unsupported parameter type void*");
            }
        };

        template <typename any_t>
        struct data_trait
        {
            inline static any_t read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                return *data_trait<any_t *>::read(ibs, hasType);
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const any_t &data, bool hasType = true)
            {
                data_trait<any_t *>::write(obs, const_cast<any_t *>(&data), hasType);
            }
        };

        template <typename any_t>
        struct data_trait<any_t *>
        {
            inline static constexpr bool isTrivial = std::is_trivially_copyable_v<any_t>;
            inline static constexpr bool isSerializable = std::derived_from<any_t, Serializable>;

            inline static any_t *read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                static_assert(isTrivial || isSerializable, "The custom data must be trivial or extended of rpc::Serializable");

                if (hasType)
                {
                    if (message_t::tcustom != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tcustom");
                }

                any_t *result = nullptr;
                if constexpr (isTrivial)
                {
                    auto size = ibs.read_ix<int32_t>();
                    result = const_cast<any_t *>(reinterpret_cast<const any_t *>(ibs.data() + ibs.tell()));
                    ibs.seek(size, SEEK_CUR);
                }
                else if constexpr (isSerializable)
                {
                    Message data{ibs.read_v()};
                    result = new any_t;
                    customDataCache.push_back(result);
                    result->deserialization(data);
                }

                return result;
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, any_t *data, bool hasType = true)
            {
                static_assert(isTrivial || isSerializable, "The custom data must be trivial or extended of rpc::Serializable");

                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tcustom));

                if constexpr (isTrivial)
                {
                    obs.write_ix<int32_t>(sizeof(any_t));
                    obs.write_bytes(data, sizeof(any_t));
                }
                else if constexpr (isSerializable)
                {
                    auto message = data->serialization();
                    obs.write_ix<int32_t>(static_cast<int32_t>(message.data.size()));
                    obs.write_bytes(message.data.data(), static_cast<int32_t>(message.data.size()));
                }
            }
        };

        typedef class Message
        {
        public:
            yasio::packet_t data;

            Message() {}
            Message(const std::string_view &data)
                : data(data.begin(), data.end())
            {
                reset();
            }
            Message(const Message &other) { data = std::move(other.data); }
            Message &operator=(const Message &other)
            {
                data = std::move(other.data);
                return *this;
            }

            yasio::ibstream_view &reader(bool reset = false)
            {
                if (reset || 0 == m_ibs.length())
                    m_ibs.reset(data);

                return m_ibs;
            }
            yasio::obstream_span<yasio::packet_t> &writer()
            {
                return m_obs;
            }

            template <typename read_t>
            auto readValue()
            {
                return data_trait<std::remove_cvref_t<read_t>>::read(m_ibs);
            }

            template <typename write_t>
            void writeValue(const write_t &data)
            {
                data_trait<std::remove_cvref_t<write_t>>::write(m_obs, data);
            }

            void reset()
            {
                m_ibs.reset(data);
            }

            // Only call once when you read custom data using "type*" manually and don't need it anymore.
            //   Otherwise, never call this function.
            void freeCustomDataCache() const
            {
                for (void *ptr : data::customDataCache)
                {
                    if (nullptr != ptr)
                        delete ptr;
                }
            }

        private:
            yasio::ibstream_view m_ibs;
            yasio::obstream_span<yasio::packet_t> m_obs{data};
        } message;

        class Serializable
        {
        public:
            virtual rpc::data::message serialization() = 0;
            virtual void deserialization(rpc::data::message &data) = 0;
        };

        template <>
        struct data_trait<Message>
        {
            inline static Message read(yasio::ibstream_view &ibs, bool hasType = true)
            {
                if (hasType)
                {
                    if (message_t::tmessage != static_cast<message_t>(ibs.read_byte()))
                        throw exception::parameter_type_error_exception("parameter type error: tmessage");
                }

                return {ibs.read_v()};
            }

            inline static void write(yasio::obstream_span<yasio::packet_t> &obs, const Message &data, bool hasType = true)
            {
                if (hasType)
                    obs.write_byte(static_cast<uint8_t>(message_t::tmessage));

                obs.write_v({data.data.begin(), data.data.end()});
            }
        };

        template <typename... any_t>
        struct reverse_parameter_list
        {
            reverse_parameter_list(message &data) {}
        };

        template <typename head_t, typename... tail_t>
        struct reverse_parameter_list<head_t, tail_t...> : private reverse_parameter_list<tail_t...>
        {
            reverse_parameter_list(message &data, const head_t &head, const tail_t &...tails)
                : reverse_parameter_list<tail_t...>(data, tails...)
            {
                data.writeValue<head_t>(head);
            }
        };
    }

    data::message dispatch(data::message &message);

    namespace network
    {
        static std::string_view SERVICE_IP = "127.0.0.1";
        static uint16_t SERVICE_PORT = 16888;

        static ThreadPool threadPool;
        static bool server = false;
        static std::unique_ptr<yasio::io_service> service;
        static std::unordered_map<uint32_t, yasio::transport_handle_t> clients;
        static std::unordered_map<uint64_t, std::unique_ptr<std::promise<data::message>>> waitForResponses;

        data::message request(const data::message &message)
        {
            static uint64_t requestId = 0;

            if (clients.empty())
                throw exception::service_no_connected_exception("service no connected");

            yasio::obstream obs;
            auto sizePos = obs.push<uint32_t>();
            obs.write<uint64_t>(requestId);
            obs.write<uint32_t>(static_cast<uint32_t>(message.data.size()));
            obs.write_bytes(message.data.data(), static_cast<int>(message.data.size()));
            obs.pop<uint32_t>(sizePos);

            for (const auto &[clientId, transport] : clients)
            {
                auto sendSize = service->write(transport, obs.buffer());

                if (0 < sendSize)
                {
                    auto it = waitForResponses.emplace(requestId++, std::make_unique<std::promise<data::message>>());
                    auto &[realId, promise] = *it.first;

                    auto result = promise->get_future().get();

                    waitForResponses.erase(realId);

                    return result;
                }
            }

            throw exception::service_not_avaliable_exception("not available service");
        }

        int response(uint32_t clientId, uint64_t requestId, const data::message &message)
        {
            if (!clients.contains(clientId))
                return -1;

            yasio::obstream obs;
            auto sizePos = obs.push<uint32_t>();
            obs.write<uint64_t>(requestId);
            obs.write<uint32_t>(static_cast<uint32_t>(message.data.size()));
            obs.write_bytes(message.data.data(), static_cast<int>(message.data.size()));
            obs.pop<uint32_t>(sizePos);

            return service->write(clients[clientId], std::move(obs.buffer()));
        }

        void dataHandler(uint32_t clientId, yasio::transport_handle_t transportHandle, yasio::packet_t &packet)
        {
            yasio::ibstream_view ibs(packet);

            try
            {
                if (!server)
                {
                    if (16 > ibs.length() || ibs.length() != ibs.read<uint32_t>() + 4)
                        throw exception::message_incomplete_exception("message incomplete");
                }
                else
                {
                    if (20 > ibs.length() || ibs.length() != ibs.read<uint32_t>() + 4)
                        throw exception::message_incomplete_exception("message incomplete");
                }

                auto requestId = ibs.read<uint64_t>();
                auto messageSize = ibs.read<uint32_t>();

                data::message message;
                message.data.resize(messageSize);
                ibs.read_bytes(message.data.data(), messageSize);

                if (!server)
                {
                    if (!waitForResponses.contains(requestId))
                        throw exception::invalid_message_exception("response invalid message");

                    waitForResponses[requestId]->set_value(std::move(message));
                }
                else
                {
                    if (0 >= response(clientId, requestId, rpc::dispatch(message)))
                        throw exception::message_response_exception("message response error");
                }
            }
            catch (const std::exception &)
            {
            }
        }
    }

    using Serializable = data::Serializable;

    std::unordered_map<uint32_t, void *> rpcObjects;
    std::unordered_map<uint32_t, std::pair<uint32_t, data::message (*)(void *, data::message &)>> rpcMethods;

    template <typename any_t, any_t>
    struct make_rpc_mapping;

    template <typename class_t, class_t *serviceObject>
    struct make_rpc_mapping<class_t *, serviceObject>
    {
        constexpr make_rpc_mapping(uint32_t classId)
        {
            rpcObjects.emplace(classId, serviceObject);
        }
    };

    template <typename return_t, typename class_t, typename... params_t, return_t (class_t::*methodPointer)(params_t...)>
    struct make_rpc_mapping<return_t (class_t::*)(params_t...), methodPointer>
    {
        constexpr make_rpc_mapping(uint32_t methodId, uint32_t classId)
        {
            rpcMethods.emplace(
                methodId,
                std::pair{
                    classId,
                    +[](void *thiz, data::message &data) -> data::message
                    {
                        data::message result;

                        if constexpr (std::is_same_v<void, return_t>)
                            (reinterpret_cast<class_t *>(thiz)->*methodPointer)(data.readValue<params_t>()...);
                        else
                            result.writeValue((reinterpret_cast<class_t *>(thiz)->*methodPointer)(data.readValue<params_t>()...));

                        return result;
                    }});
        }
    };

    template <typename return_t, uint32_t methodId, typename... params_t>
    constexpr return_t invoke(params_t... params)
    {
        data::message message;
        auto &writer = message.writer();

        writer.write<uint32_t>(methodId);
        if constexpr (0 != sizeof...(params))
            data::reverse_parameter_list<params_t...>(message, params...);

        auto result = network::request(message);
        result.reset();
        if constexpr (!std::is_same_v<void, return_t>)
            return result.readValue<return_t>();
    }

    data::message dispatch(data::message &message)
    {
        auto &reader = message.reader(true);

        auto methodId = reader.read<uint32_t>();
        if (!rpcMethods.contains(methodId))
            throw exception::call_method_notfound_exception("no method id");

        auto &[classId, methodProxy] = rpcMethods[methodId];
        if (!rpcObjects.contains(classId))
            throw exception::service_provider_notfound_exception("no class object");

        auto result = methodProxy(rpcObjects[classId], message);

        message.freeCustomDataCache();

        return result;
    }

    bool start(bool server = false)
    {
        if (nullptr != network::service)
            return false;

        network::server = server;
        network::service = std::make_unique<yasio::io_service>(yasio::io_hostent{network::SERVICE_IP, network::SERVICE_PORT});
        network::service->set_option(yasio::YOPT_C_UNPACK_PARAMS, 10 * 1024 * 1024, -1, 4, 0);
        network::service->set_option(yasio::YOPT_S_DEFERRED_EVENT, 0);
        network::service->open(0, server ? yasio::YCK_TCP_SERVER : yasio::YCK_TCP_CLIENT);

        network::service->start(
            [&](yasio::event_ptr &&ev)
            {
                switch (ev->kind())
                {
                case yasio::YEK_ON_OPEN:
                {
                    network::clients[ev->source_id()] = ev->transport();
                    break;
                }
                case yasio::YEK_ON_CLOSE:
                {
                    network::clients.erase(ev->source_id());
                    break;
                }
                case yasio::YEK_ON_PACKET:
                {
                    network::threadPool.addRunable(network::dataHandler, ev->source_id(), ev->transport(), ev->packet());
                    break;
                }
                };
            });

        return network::service->is_open(0);
    }

    bool stop()
    {
        if (nullptr == network::service)
            return false;

        network::service->stop();
        network::service->close(0);
        network::service = nullptr;

        return true;
    }
}

#define _MACRO_CONCAT(prefix, suffix) prefix##suffix
#define MACRO_CONCAT(prefix, suffix) _MACRO_CONCAT(prefix, suffix)

#define ANONYMOUS_VARIABLE MACRO_CONCAT(__, __COUNTER__)

#ifdef _MSC_VER // _MSC_VER

#define EXPAND(x) x
#define ___NPARAMS(_21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, n, ...) n
#define __NPARAMS(...) EXPAND(___NPARAMS(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define _NPARAMS(...) unused, __VA_ARGS__
#define NPARAMS(...) __NPARAMS(_NPARAMS(__VA_ARGS__))

#else

#define _NPARAMS(_21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, n, ...) n
#define NPARAMS(...) _NPARAMS(__VA_OPT__(, )##__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#endif // _MSC_VER

// definition list
#define _DL0()
#define _DL1(t1) t1 data01
#define _DL2(t1, t2) t1 data01, t2 data02
#define _DL3(t1, t2, t3) t1 data01, t2 data02, t3 data03
#define _DL4(t1, t2, t3, t4) t1 data01, t2 data02, t3 data03, t4 data04
#define _DL5(t1, t2, t3, t4, t5) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05
#define _DL6(t1, t2, t3, t4, t5, t6) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06
#define _DL7(t1, t2, t3, t4, t5, t6, t7) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07
#define _DL8(t1, t2, t3, t4, t5, t6, t7, t8) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08
#define _DL9(t1, t2, t3, t4, t5, t6, t7, t8, t9) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09
#define _DL10(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10
#define _DL11(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11
#define _DL12(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12
#define _DL13(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13
#define _DL14(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13, t14 data14
#define _DL15(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13, t14 data14, t15 data15
#define _DL16(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13, t14 data14, t15 data15, t16 data16
#define _DL17(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13, t14 data14, t15 data15, t16 data16, t17 data17
#define _DL18(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13, t14 data14, t15 data15, t16 data16, t17 data17, t18 data18
#define _DL19(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13, t14 data14, t15 data15, t16 data16, t17 data17, t18 data18, t19 data19
#define _DL20(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16, t17, t18, t19, t20) t1 data01, t2 data02, t3 data03, t4 data04, t5 data05, t6 data06, t7 data07, t8 data08, t9 data09, t10 data10, t11 data11, t12 data12, t13 data13, t14 data14, t15 data15, t16 data16, t17 data17, t18 data18, t19 data19, t20 data20

// name list
#define _NL0
#define _NL1 data01
#define _NL2 data01, data02
#define _NL3 data01, data02, data03
#define _NL4 data01, data02, data03, data04
#define _NL5 data01, data02, data03, data04, data05
#define _NL6 data01, data02, data03, data04, data05, data06
#define _NL7 data01, data02, data03, data04, data05, data06, data07
#define _NL8 data01, data02, data03, data04, data05, data06, data07, data08
#define _NL9 data01, data02, data03, data04, data05, data06, data07, data08, data09
#define _NL10 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10
#define _NL11 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11
#define _NL12 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12
#define _NL13 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13
#define _NL14 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13, data14
#define _NL15 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13, data14, data15
#define _NL16 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13, data14, data15, data16
#define _NL17 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13, data14, data15, data16, data17
#define _NL18 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13, data14, data15, data16, data17, data18
#define _NL19 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13, data14, data15, data16, data17, data18, data19
#define _NL20 data01, data02, data03, data04, data05, data06, data07, data08, data09, data10, data11, data12, data13, data14, data15, data16, data17, data18, data19, data20

#ifdef _MSC_VER
#define GENERATE_DEFINITION_LIST(...) EXPAND(MACRO_CONCAT(_DL, NPARAMS(__VA_ARGS__))(__VA_ARGS__))
#else
#define GENERATE_DEFINITION_LIST(...) MACRO_CONCAT(_DL, NPARAMS(__VA_ARGS__))(__VA_ARGS__)
#endif
#define GENERATE_NAME_LIST(...) MACRO_CONCAT(_NL, NPARAMS(__VA_ARGS__))

#ifdef _MSC_VER
#define _RPCInvoke(returnType, methodName, definitionList, nameList)                           \
    returnType methodName(definitionList)                                                      \
    {                                                                                          \
        return rpc::invoke<returnType, rpc::calc::const_hash_macro<__FUNCTION__>()>(nameList); \
    }
#else
#define _RPCInvoke(returnType, methodName, definitionList, nameList)                                  \
    returnType methodName(definitionList)                                                             \
    {                                                                                                 \
        return rpc::invoke<returnType, rpc::calc::const_hash_macro<__PRETTY_FUNCTION__>()>(nameList); \
    }
#endif
#define RPCInvoke(returnType, methodName, ...) \
    _RPCInvoke(returnType, methodName, GENERATE_DEFINITION_LIST(__VA_ARGS__), GENERATE_NAME_LIST(__VA_ARGS__))

#define _RPCService(className, variableName, ...) \
    className variableName{__VA_ARGS__};          \
    inline static rpc::make_rpc_mapping<decltype(&variableName), &variableName> ANONYMOUS_VARIABLE(rpc::calc::const_hash(#className))
#define RPCService(className, ...) \
    _RPCService(className, ANONYMOUS_VARIABLE, __VA_ARGS__)

#define RPCImplement(returnType, methodName, ...)                                                         \
    inline static rpc::make_rpc_mapping<decltype(&methodName), &methodName>                               \
        ANONYMOUS_VARIABLE{rpc::calc::const_hash(#methodName), rpc::calc::const_hash_class(#methodName)}; \
    returnType methodName(__VA_ARGS__)

#endif // !RPC_CPP_H