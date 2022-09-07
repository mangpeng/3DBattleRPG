#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"



IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	//return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObject), 0);
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, 0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	// timeoutMs없이 계속 GetQueue~~~를 호출하면
	// 스레드는 잠들게 된다. 처리할 completion i/o가 없기 때문에
	// 타임아웃을 지정하면 잠들다가 시간이 되면 빠져 나온다.
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT: // 시간을 무한대로 설정하지 않은 경우 타임 아웃 에러가 발생할 수 있음.
			return false;
		default:
			// TODO : 에러 핸들
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
