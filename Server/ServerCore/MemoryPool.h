#pragma once


// [MemoryHeader][Data]
struct MemoryHeader
{
	MemoryHeader(int32 size) : allocSize(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); // placement new. 메모리가 할당된 객체의 생성자 호출
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
};


class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr); // 메모리풀에 메모리 반납시
	MemoryHeader*	Pop();				     // 메모리풀에서 메모리 가져올 때

private:
	int32 _allocSize = 0;
	atomic<int32> _useCount = 0; // 디버그용
	atomic<int32> _reserveCount = 0; // 디버그용


	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

