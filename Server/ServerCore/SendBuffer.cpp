#include "pch.h"
#include "SendBuffer.h"



SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize)
	:_owner(owner), _buffer(buffer), _allocSize(allocSize)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);
}





SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	_open = false;
	_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;

	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;
}








/// <summary>
/// SendBufferChunk 에서 사용 요청한 공간을 반환.
/// 실제로 사용했음을 의미 하진 않음.
/// </summary>
/// <param name="size"></param>
/// <returns></returns>
SendBufferRef SendBufferManager::Open(uint32 size)
{
	if (LSendBufferChunk == nullptr) // tls이기 때문에 경합 없음
	{
		LSendBufferChunk = Pop(); // 내부에 write_lock
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 요청한 공간이 없다면 새롭게 SendBufferChunk버퍼를 받는다.
	// 기존에 사용하던 SendBufferChunk는 ref count가 0이되면 자동으로
	// SendBufferChunk vector에 반납된다.
	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	cout << "Free " << LSendBufferChunk->FreeSize() << endl;
	return LSendBufferChunk->Open(size);
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
SendBufferChunkRef SendBufferManager::Pop()
{
	WRITE_LOCK;
	cout << "SendBufferManager pop " << endl;
	if (_sendBufferChunks.empty() == false)
	{
		SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
		_sendBufferChunks.pop_back();
		return sendBufferChunk;
	}

	// 스마트 포인터 매개변수로 동적할당된 대상과, 소멸자를 지정하게 된다.
	// 여기서는 소멸자로 PushGlobal을 지정함으로 sendBufferChunk vector에 다시 반납 되도록 한다.
	return SendBufferChunkRef(Xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);
}

/// <summary>
/// 사용이 끝난(ref count가 0인) SendBufferChunk는 소멸자 대신
/// 해당 함수가 호출되어 SendBUfferChunk vector에 push 된다.
/// </summary>
/// <param name="buffer"></param>
void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	cout << "PushGlobal " << endl;
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}

