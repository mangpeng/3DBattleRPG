#pragma once


class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	BYTE*	Buffer() { return _buffer; }
	uint32	AllocSize() { return _allocSize; }
	uint32	WriteSize() { return _writeSize; }
	void	Close(uint32 writeSize);

private:
	BYTE*				_buffer;
	uint32				_allocSize = 0;
	uint32				_writeSize = 0;

	// sendbuffer가 살아 있는 동안 sendbufferChunk도 살아 있어야 하므로 ref를 시킨다.
	SendBufferChunkRef	_owner;
};


/// <summary>
/// SendBufferManager에서 TLS 변수에 할당해서 SendBufferChunk를 사용하기 떄문에
/// SendBufferChunk는 thread-safe 함
/// </summary>
class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000,
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void				Reset();
	SendBufferRef		Open(uint32 allocSize);
	void				Close(uint32 writeSize);

	bool				IsOpen() { return _open; }
	BYTE*				Buffer() { return &_buffer[_usedSize]; }
	uint32				FreeSize() { return static_cast<uint32>(_buffer.size() - _usedSize); }



private:
	Xarray<BYTE, SEND_BUFFER_CHUNK_SIZE>	_buffer = {};
	bool									_open = false;
	uint32									_usedSize = 0;

};

class SendBufferManager
{
public:
	SendBufferRef		Open(uint32 size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	Xvector<SendBufferChunkRef> _sendBufferChunks;
};
