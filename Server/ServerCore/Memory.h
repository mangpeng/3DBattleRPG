#pragma once
#include "Allocator.h"


template<typename Type, typename... Args>
Type* Xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(Xalloc(sizeof(Type)));

	// 메모리가 할당된 객체의 생성자를 호출 하는 방법. placement new
	new(memory)Type(std::forward<Args>(args)...);

	return memory;
}


template<typename Type>
void Xdelete(Type* obj)
{
	obj->~Type();
	Xrelease(obj);
}