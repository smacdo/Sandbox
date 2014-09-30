#pragma once
#include <string>
#include <sstream>          // make_string
#include <algorithm>        // string trimming
#include <cctype>           // string trimming

namespace Utils
{
	std::wstring ConvertUtf8ToWString(const std::string& input);
    std::string ConvertUtf16ToUtf8(const std::wstring& input);

    std::wstring GetErrorMessageFromHResult(unsigned long errorCode);
    std::wstring GetErrorMessageFromWinApiErrorCode(unsigned long windowsApiErrorCode);

    bool EndsWith(const std::string&fullString, const std::string& ending);
    float RandFloat();
    float RandFloat(float min, float max);
}

/*
///////////////////////////////////////////////////////////////////////////////////////////////////
// Get the number of elements in an array, C++11 style.
// [+69] [2009-01-22 18:04:31] Josh Kelley 
//  http://www.stackprinter.com/export?service=stackoverflow&question=469696
///////////////////////////////////////////////////////////////////////////////////////////////////
template<int n>
struct char_array_wrapper{
    char result[n];
};

template<typename T, int s>
char_array_wrapper<s> the_type_of_the_variable_is_not_an_array(const T(&array)[s]){
}

#define ITEMSOF(v) sizeof(the_type_of_the_variable_is_not_an_array(v).result)

///////////////////////////////////////////////////////////////////////////////////////////////////
// make_string()
// [+33] [2009-01-22 21:57:40] David Rodríguez - dribeas 
// http://www.stackprinter.com/export?service=stackoverflow&question=469696
///////////////////////////////////////////////////////////////////////////////////////////////////
class make_string
{
public:
    template <typename T>
    make_string& operator<<(T const & datum)
    {
        buffer_ << datum;
        return *this;
    }
    operator std::string() const
    {
        return buffer_.str();
    }
private:
    std::ostringstream buffer_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// string trimming functions
// [+29] [2009-01-22 16:00:07] Evan Teran 
// http://www.stackprinter.com/export?service=stackoverflow&question=469696
///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// String buffer for a Win32 API that wants to write into a c string array.
// [+24][2009 - 01 - 23 17:32 : 23] Daniel Earwicker
// http://www.stackprinter.com/export?service=stackoverflow&question=469696
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class C>
class WinStringBuffer
{
    typename std::basic_string<C> &m_str;
    typename std::vector<C> m_buffer;

public:
    WinStringBuffer(std::basic_string<C> &str, size_t nSize)
        : m_str(str), m_buffer(nSize + 1) {
        get()[nSize] = (C)0;
    }

    ~WinStringBuffer()
    {
        commit();
    }

    C *get()
    {
        return &(m_buffer[0]);
    }

    operator C *()
    {
        return get();
    }

    void commit()
    {
        if (m_buffer.size() != 0)
        {
            size_t l = std::char_traits<C>::length(get());

            m_str.assign(get(), l);

            m_buffer.resize(0);
        }
    }

    void abort()
    {
        m_buffer.resize(0);
    }
};

template <class C>
inline WinStringBuffer<C> WinStringBuffer(typename std::basic_string<C> &str, size_t nSize)
{
    return WinStringBuffer<C>(str, nSize);
}

// // Example: 
// //  StringBuffer gives a writeable buffer to the API, and then on destruction it copies it into str.
// std::string str;
// GetWindowsDirectory(StringBuffer(str, MAX_PATH), MAX_PATH);
*/