#pragma once

// 1.recv�� ��Ƽ������ ��� ���� �ʾƵ� ��. reisger -> check cp -> processRecv -> reigster�� ���� �Ǳ� ������
// 2.tcp Ư���� �������� ��谡 ���� �ʱ� ������ recv�� �����͸� �����ϴٰ� packet���� ���� ������ ������ �϶� 
//	 ��Ŷ���� �����Ͽ� ���� ������ ó���ؾ� �ϹǷ� ������ RecvBuffer�� �ʿ��ϴ�.



// [r][][][w][][][][][][][][][][]
// if(r==w) r,w�� offset 0���� �̵�.
// if(w==�������ε���), r, w�� ������ ����

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 }; // buffer Clean�� read, write Ŀ���� ������ Ȯ���� ���̱� ����

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void	Clean();
	bool	OnRead(int32 numOfBytes);
	bool	OnWrite(int32 numOfBytes);

	BYTE*	ReadPos() { return &_buffer[_readPos]; }
	BYTE*	WritePos() { return &_buffer[_writePos]; }
	int32	DataSize() { return _writePos - _readPos; }
	int32	FreeSize() { return _capacity - _writePos; }

private:
	int32				_capacity = 0;
	int32				_bufferSize = 0;
	int32				_readPos = 0;
	int32				_writePos = 0;
	Xvector<BYTE>		_buffer;
};

