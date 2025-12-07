#pragma once

#include <vector>
#include <string>
#include "Strings.hpp"

namespace sEnum
{


    // http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
    inline std::vector<std::string> split(const std::string& text, char sep)
    {
        std::array<std::string, > tokens;
        int start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos)
        {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }

#define ENUM_VALUE_STRING(NAME, ...)\
std::string NAME##Values = #__VA_ARGS__;

#define ENUM_VALUE_NAME_VECTOR(NAME, ...)std::vector<std::string> NAME##Map = split(NAME##Values, ',');

#define ENUM(NAME, UNDERLYING_TYPE, ...)\
    enum class NAME : UNDERLYING_TYPE\
    {\
    __VA_ARGS__\
    };

    template <typename NAME>
    std::string ToString(const NAME v)
    {
        return trim(NAME
##Map.at(static_cast<UNDERLYING_TYPE>(v)));
    }

} //namespace sEnum
