#pragma once
#include <string>
#include <sstream>          // MakeString
#include <vector>           // WinStringBuffer

namespace Utils
{
	std::wstring ConvertUtf8ToWString(const std::string& input);
    std::string ConvertUtf16ToUtf8(const std::wstring& input);

    std::wstring GetErrorMessageFromHResult(unsigned long errorCode);
    std::wstring GetErrorMessageFromWinApiErrorCode(unsigned long windowsApiErrorCode);
    std::wstring GetErrorMessageFromErrno(errno_t errorCode);

    bool StartsWith(const std::string& fullString, const std::string& prefix);
    bool StartsWith(const std::wstring& fullString, const std::wstring& prefix);
    bool EndsWith(const std::string& fullString, const std::string& ending);
    bool EndsWith(const std::wstring& fullString, const std::wstring& ending);

    std::string RightTrim(const std::string &text);
    std::wstring RightTrim(const std::wstring &text);
    std::string LeftTrim(const std::string &text);
    std::wstring LeftTrim(const std::wstring &text);
    std::string Trim(const std::string &text);
    std::wstring Trim(const std::wstring &text);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // make_string()
    // [+33] [2009-01-22 21:57:40] David Rodríguez - dribeas 
    // http://www.stackprinter.com/export?service=stackoverflow&question=469696
    //
    //  - Allows you to compose strings inline and then pass the resultant temporary string object
    //    to a function.
    //    EXAMPLE: f( MakeString() << "foo" << 42 );
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class MakeString
    {
    public:
        template <typename T>
        MakeString& operator<<(const T& value)
        {
            mBuffer << value;
            return *this;
        }

        operator std::string() const
        {
            return mBuffer.str();
        }

    private:
        std::ostringstream mBuffer;
    };

    class MakeWideString
    {
    public:
        template <typename T>
        MakeWideString& operator<<(const T& value)
        {
            mBuffer << value;
            return *this;
        }

        operator std::wstring() const
        {
            return mBuffer.str();
        }

    private:
        std::wostringstream mBuffer;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // String buffer for a Win32 API that wants to write into a c string array.
    // [+24][2009 - 01 - 23 17:32 : 23] Daniel Earwicker
    // http://www.stackprinter.com/export?service=stackoverflow&question=469696
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class C>
    class WinStringBufferImpl
    {
        typename std::basic_string<C>& mOutputString;
        typename std::vector<C> mBuffer;

    public:
        WinStringBufferImpl(std::basic_string<C>& outputString, size_t maxSize)
            : mOutputString(outputString),
              mBuffer(maxSize + 1)
        {
            Get()[maxSize] = static_cast<C>(0);
        }

        ~WinStringBufferImpl()
        {
            Commit();
        }

        operator C*()
        {
            return Get();
        }

        void Abort()
        {
            mBuffer.resize(0);
        }

    private:
        void Commit()
        {
            if (mBuffer.size() != 0)
            {
                size_t length = std::char_traits<C>::length(Get());
                mOutputString.assign(Get(), length);
                mBuffer.resize(0);
            }
        }

        C *Get()
        {
            return &(mBuffer[0]);
        }
    };

    template <class C>
    inline WinStringBufferImpl<C> WinStringBuffer(typename std::basic_string<C> &outputString, size_t maxSize)
    {
        return WinStringBufferImpl<C>(outputString, maxSize);
    }

    /**
     * \brief Generate a random floating point value between [0.0, 1.0].
     */
    float RandFloat();

    /**
     * \brief Generate a random floating point value between [min, max].
     * TODO: Prove that values are inclusive.
     */
    float RandFloat(float min, float max);
}