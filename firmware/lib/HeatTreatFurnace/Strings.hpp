#pragma once

#include <string>

namespace HeatTreatFurnace::Strings
{
    #define MAX_VALUE_NAME_LENGTH 16
    #define MAX_STRING_LENGTH 256

    // http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
    template< size_t S>
    std::vector<std::string> split(const std::string& text, char sep)
    {
        std::array<std::string, S> tokens{};
        std::string::size_type start = 0;
        std::string::size_type end = 0;
        while ((end = text.find(sep, start)) != std::string::npos)
        {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }

    inline const char* ws = " \t\n\r\f\v";

    // trim from end of string (right)
    inline std::string& rtrim(std::string& s, const char* t = ws)
    {
        return s.erase(s.find_last_not_of(t) + 1);
    }

    // trim from beginning of string (left)
    inline std::string& ltrim(std::string& s, const char* t = ws)
    {
        return s.erase(0, s.find_first_not_of(t));
    }

    // trim from both ends of string (right then left)
    inline std::string& trim(std::string& s, const char* t = ws)
    {
        return ltrim(rtrim(s, t), t);
    }
} //namespace HeatTreatFurnace::Strings