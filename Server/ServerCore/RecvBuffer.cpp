#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize)
	:_bufferSize(bufferSize)
{
	_capacity = _bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();

	if (dataSize == 0) // read, write 커서가 동일한 위치인 경우
		_readPos = _writePos = 0;
	else
	{
		// 여유 공간이 단일 버퍼 크기보다 작은 경우, 데이터를 앞으로 땡긴다.
		if (FreeSize() < _bufferSize)
		{
			::memcpy(_buffer.data(), &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

// 데이터를 읽었으면 read 커서를 옮긴다.
bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;

	return true;
}

// 데이터를 썼으면 write 커서를 옮긴다.
bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;

	return true;
}
