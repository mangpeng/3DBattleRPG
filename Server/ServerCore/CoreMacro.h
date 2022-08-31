#pragma once


#define CRASH(cause)                                 \
{                                                    \
	uint32* crash = nullptr;                         \
	__analysis_assert(crash != nullptr);             \
	*crash = 0xDEADBEEF;                             \
}                                                    \

#define ASSERT_CRASH(expr)                           \
{                                                    \
	if(!(expr))                                      \
	{                                                \
		CRASH("ASSERT_CRASH");                       \
		__analysis_assert(expr);                     \
	}                                                \
}                                                    \