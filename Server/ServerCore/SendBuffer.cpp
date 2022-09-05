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
/// SendBufferChunk ���� ��� ��û�� ������ ��ȯ.
/// ������ ��������� �ǹ� ���� ����.
/// </summary>
/// <param name="size"></param>
/// <returns></returns>
SendBufferRef SendBufferManager::Open(uint32 size)
{
	if (LSendBufferChunk == nullptr) // tls�̱� ������ ���� ����
	{
		LSendBufferChunk = Pop(); // ���ο� write_lock
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// ��û�� ������ ���ٸ� ���Ӱ� SendBufferChunk���۸� �޴´�.
	// ������ ����ϴ� SendBufferChunk�� ref count�� 0�̵Ǹ� �ڵ�����
	// SendBufferChunk vector�� �ݳ��ȴ�.
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

	// ����Ʈ ������ �Ű������� �����Ҵ�� ����, �Ҹ��ڸ� �����ϰ� �ȴ�.
	// ���⼭�� �Ҹ��ڷ� PushGlobal�� ���������� sendBufferChunk vector�� �ٽ� �ݳ� �ǵ��� �Ѵ�.
	return SendBufferChunkRef(Xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(buffer);
}

/// <summary>
/// ����� ����(ref count�� 0��) SendBufferChunk�� �Ҹ��� ���
/// �ش� �Լ��� ȣ��Ǿ� SendBUfferChunk vector�� push �ȴ�.
/// </summary>
/// <param name="buffer"></param>
void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	cout << "PushGlobal " << endl;
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}

