#include "stdafx.h"
#include "BinaryBlob.h"
#include "DXSandbox.h"
#include "DXTestException.h"
#include "Utils.h"

#include <string>
#include <fstream>
#include <algorithm>
#include <memory>

BinaryBlob::BinaryBlob()
: mpBuffer(nullptr),
  mSize(0)
{
}

BinaryBlob::BinaryBlob(const char * pBuffer, std::streamsize size)
    : mpBuffer(),
      mSize(0)
{
    if (pBuffer != nullptr && size > 0)
	{
        mSize = size;
        mpBuffer.reset(new char[static_cast<unsigned int>(mSize)]);

        std::copy(
            &pBuffer[0],
            &pBuffer[0] + size,
            stdext::checked_array_iterator<char *>(mpBuffer.get(), size));
	}
}

BinaryBlob::BinaryBlob(const BinaryBlob& blob)
    : mpBuffer(),
      mSize(0)
{
	if (!blob.IsNull())
	{
        mSize = blob.mSize;
        mpBuffer.reset(new char[static_cast<unsigned int>(mSize)]);

		std::copy(
            blob.mpBuffer.get(), 
            blob.mpBuffer.get() + blob.mSize,
            stdext::checked_array_iterator<char *>(mpBuffer.get(), mSize));
	}
}

BinaryBlob::~BinaryBlob()
{
}

BinaryBlob& BinaryBlob::operator =(const BinaryBlob& rhs)
{
	if (this != &rhs)
	{
        if (rhs.IsNull())
        {
            mpBuffer.reset();
            mSize = 0;
        }
        else
        {
            mSize = rhs.mSize;
			mpBuffer.reset(new char[static_cast<unsigned int>(mSize)]);

			std::copy(
                rhs.mpBuffer.get(),
                rhs.mpBuffer.get() + rhs.mSize,
                stdext::checked_array_iterator<char *>(mpBuffer.get(), rhs.mSize));
		}
	}

	return *this;
}

bool BinaryBlob::IsNull() const
{
    return mSize == 0;
}

const char* BinaryBlob::BufferPointer() const
{
	return mpBuffer.get();
}

std::streamsize BinaryBlob::BufferSize() const
{
	return mSize;
}

BinaryBlob BinaryBlob::LoadFromFile(const std::string& filepath)
{
	// Open the shader file.
	std::ifstream inputStream(filepath.c_str(), std::ios::binary);

	if (!inputStream.is_open())
	{
        throw FileLoadException(Utils::ConvertUtf8ToWString(filepath));
	}

	// Get number of bytes in binary file.
	inputStream.seekg(0, std::ios_base::end);
	std::streampos size = inputStream.tellg();
	inputStream.seekg(0, std::ios_base::beg);

	// Read the file contents into a memory buffer.
	char * pBufferContents = new char[static_cast<unsigned int>(size)];
	inputStream.read(pBufferContents, size);

	// Create a new binary blob.
	BinaryBlob blob = BinaryBlob(pBufferContents, size);

	// Release temporaries and return the binary blob.
	SafeDeleteArray(pBufferContents);

	return blob;
}