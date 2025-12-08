#pragma once

#include <vector>
#include <string>
#include <etl/string.h>
#include <etl/vector.h>
#include "Strings.hpp"

namespace HeatTreatFurnace::EnumString
{




// template <typename UNDERLYING_TYPE, size_t SIZE>
// struct EnumString
// {
//     UNDERLYING_TYPE value;
//     etl::vector<etl::string<MAX_VALUE_NAME_LENGTH>, SIZE> myValueMap;
//     EnumString(UNDERLYING_TYPE anInitialValue, const etl::string<MAX_VALUE_NAME_LENGTH> (&aValueMap)[SIZE]) : value(anInitialValue), myValueMap(myValueMap) {}
//
//     [[nodiscard]] etl::string<MAX_STRING_LENGTH> ToString() const { return myValueMap[value]; }
//
//     explicit operator UNDERLYING_TYPE() const { return value; }
//
//     explicit operator etl::string<MAX_STRING_LENGTH>() const { return ToString(); }
// };

// template<typename T>
// struct EnumInfo;
//
// #define ENUM(name_, type_, ...)\
// enum class name_: type_ {__VA_ARGS__};\
// template<>\
// struct name_##Info<name_> {\
// using type = type_;\
// static constexpr const char* name = #name_;\
// static constexpr const char* members[] = {#__VA_ARGS__};\
// };

// template<typename T>
// etl::string<MAX_VALUE_NAME_LENGTH> ToString(const T& value) {
//     using Info = EnumInfo<T>;
//     return etl::string<MAX_VALUE_NAME_LENGTH>(Info::members[std::to_underlying(value)]);
// }
}//namespace EnumString
