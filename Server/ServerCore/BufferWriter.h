#pragma once


class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferWriter();

	BYTE*		Buffer() { return _buffer; }
	uint32		Size() { return _size; }
	uint32		WriteSize() { return _pos; }
	uint32		FreeSize() { return _size - _pos; }

	template<typename T>
	bool		Write(T* src) { return Read(src, sizeof(T)); }
	bool		Write(void* src, uint32 len);

	template<typename T>
	T*			 Reserve();

	template<typename T>
	BufferWriter& operator<<(T&& src);

private:
	BYTE*		_buffer = nullptr;
	uint32		_size = 0;
	uint32		_pos = 0;
};

template<typename T>
inline T* BufferWriter::Reserve()
{
	if (FreeSize() < sizeof(T))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);

	return ret;
}


template<typename T>
inline BufferWriter& BufferWriter::operator<<(T&& src)
{
	// 전달참조이기 때문에
	// 왼값이면 const int&
	// 오른값이면 int&& 형태로 전달됨
	// 때문에 &를 떄야 한다.
	using DataType = std::remove_reference_t<T>;

	*reinterpret_cast<DataType*>(&_buffer[_pos]) = std::forward<DataType>(src);
	_pos += sizeof(DataType);

	return *this;
}
