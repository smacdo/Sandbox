#include "BinaryBlob.h"
#include "DXTest.h"
#include "DXTestException.h"

#include <string>
#include <fstream>
#include <algorithm>

BinaryBlob::BinaryBlob()
: mpBuffer(nullptr),
  mSize(0)
{
}

BinaryBlob::BinaryBlob(const char * pBuffer, std::streamsize size)
: mpBuffer(nullptr),
  mSize(pBuffer != nullptr ? size : 0)
{
	if (pBuffer != nullptr)
	{
		// TODO: Assert size > 0.
		// Allocate a new buffer and copy the passed in buffer's contents.
		mpBuffer = new char[static_cast<unsigned int>(size)];
		std::copy(&pBuffer[0], &pBuffer[0] + size, &mpBuffer[0]);
	}
}

BinaryBlob::BinaryBlob(const BinaryBlob& blob)
: mpBuffer(nullptr),
  mSize(blob.mSize)
{
	if (blob.mpBuffer != nullptr)
	{
		// TODO: Assert size > 0.
		// Allocate a new buffer and copy the passed in buffer's contents.
		mpBuffer = new char[static_cast<unsigned int>(blob.mSize)];
		std::copy(&(blob.mpBuffer[0]), &(blob.mpBuffer[0]) + blob.mSize, &mpBuffer[0]);
	}
}

BinaryBlob::~BinaryBlob()
{
	SafeDeleteArray(mpBuffer);
}

BinaryBlob& BinaryBlob::operator =(const BinaryBlob& rhs)
{
	if (this != &rhs)
	{
		// Release the current buffer if this binary blob instance has one allocated.
		if (mpBuffer != nullptr)
		{
			SafeDelete(mpBuffer);
			mSize = 0;
		}

		// If the right hand binary blob instance (the source) has a valid buffer, copy its contents.
		if (rhs.mpBuffer != nullptr)
		{
			mpBuffer = new char[static_cast<unsigned int>(rhs.mSize)];
			std::copy(&(rhs.mpBuffer[0]), &(rhs.mpBuffer[0]) + rhs.mSize, &mpBuffer[0]);
		}
		else
		{
			mpBuffer = nullptr;
			mSize = 0;
		}
	}

	return *this;
}

const char* BinaryBlob::BufferPointer() const
{
	return mpBuffer;
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
		throw FileLoadException(filepath);
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