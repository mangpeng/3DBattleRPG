#pragma once
#include "Allocator.h"


template<typename Type, typename... Args>
Type* Xnew(Args&&... args)
{
	Type* memory = static_cast<Type*>(Xalloc(sizeof(Type)));

	// �޸𸮰� �Ҵ�� ��ü�� �����ڸ� ȣ�� �ϴ� ���. placement new
	new(memory)Type(std::forward<Args>(args)...);

	return memory;
}


template<typename Type>
void Xdelete(Type* obj)
{
	obj->~Type();
	Xrelease(obj);
}