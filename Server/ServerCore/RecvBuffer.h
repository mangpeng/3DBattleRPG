#pragma once

// 1.recv는 멀티스레드 고려 하지 않아도 됨. reisger -> check cp -> processRecv -> reigster로 진행 되기 떄문에
// 2.tcp 특성상 데이터의 경계가 있지 않기 때문에 recv한 데이터를 보관하다가 packet으로 조립 가능한 사이즈 일때 
//	 패킷으로 조립하여 이후 로직을 처리해야 하므로 별도로 RecvBuffer가 필요하다.



// [r][][][w][][][][][][][][][][]
// if(r==w) r,w를 offset 0으로 이동.
// if(w==마지막인덱스), r, w를 앞으로 복사

class RecvBuffer
{
	enum { BUFFER_COUNT = 10 }; // buffer Clean시 read, write 커서가 동일한 확률을 높이기 위해

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

