#pragma once

#include <string>
#include <memory>   // unique_ptr
#include <ios>

// Represents a binary chunk of bytes.
//  TODO: Maybe call it BinaryBuffer? maybe not.
//  TODO: Improve mechanics, use shared_ptr or unique_ptr

/**
 * \brief Represents a binary chunk of bytes.
 *
 * TODO: Rename this to BinaryBuffer.
 */
class BinaryBlob
{
public:
	BinaryBlob();
	BinaryBlob(const char* pBuffer, std::streamsize size);
	BinaryBlob(const BinaryBlob& blob);
	virtual ~BinaryBlob();

	BinaryBlob& operator =(const BinaryBlob& rhs);

    bool IsNull() const;

	// Get a temporary readonly pointer to the buffer. DO NOT STORE THIS POINTER.
	const char* BufferPointer() const;

	std::streamsize BufferSize() const;

	static BinaryBlob LoadFromFile(const std::string& filepath);    // TODO: Remove this.
    static BinaryBlob LoadFromFile(const std::wstring& filepath);
	
private:
	std::unique_ptr<char> mpBuffer;
	std::streamsize mSize;
};

