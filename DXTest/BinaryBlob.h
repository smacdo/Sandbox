#pragma once

#include <string>
#include <ios>

// Represents a binary chunk of bytes.
//  TODO: Maybe call it BinaryBuffer? maybe not.
//  TODO: Improve mechanics, use shared_ptr or unique_ptr

class BinaryBlob
{
public:
	BinaryBlob();
	BinaryBlob(const char* pBuffer, std::streamsize size);
	BinaryBlob(const BinaryBlob& blob);
	virtual ~BinaryBlob();

	BinaryBlob& operator =(const BinaryBlob& rhs);

	// Get a temporary readonly pointer to the buffer. DO NOT STORE THIS POINTER.
	const char* BufferPointer() const;

	std::streamsize BufferSize() const;

	static BinaryBlob LoadFromFile(const std::string& filepath);
	
private:
	char* mpBuffer;
	std::streamsize mSize;
};

