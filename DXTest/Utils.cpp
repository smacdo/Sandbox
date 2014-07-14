#include "Utils.h"

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <Windows.h>

using namespace Utils;

// http://stackoverflow.com/a/6691829
std::wstring Utils::ConvertUtf8ToWString(const std::string& input)
{
	// Deal with the trivial case of being passed an empty string.
	if (input.empty())
	{
		return std::wstring();
	}

	// Determine the length required to hold the new wstring.
	size_t requiredLength = ::MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), 0, 0);

	// Instantiate a wstring that is correctly presized, and then convert the UTF8 string to wstring
	// in place.
	std::wstring output(requiredLength, L'\0');
	::MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), &output[0], (int)output.length());

	return output;
}

// http://stackoverflow.com/a/874160
bool Utils::EndsWith(const std::string& fullString, const std::string& ending)
{
    if (fullString.length() >= ending.length())
    {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else
    {
        return false;
    }
}

float Utils::RandFloat()
{
    return (float)rand() / RAND_MAX;
}

float Utils::RandFloat(float min, float max)
{
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float difference = min - max;
    float r = random * difference;

    return min + r;
}