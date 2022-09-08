#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "GlobalQueue.h"
#include "JobTimer.h"

ThreadManager*				GThreadManager = nullptr;
Memory*						GMemory = nullptr;
SendBufferManager*			GSendBufferManager = nullptr;
DeadLockProfiler*			GDeadLockProfiler = nullptr;
GlobalQueue*				GGlobalQueue = nullptr;
JobTimer*					GJobTimer = nullptr;

uint64						GPlayerId = 0;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GSendBufferManager = new SendBufferManager();
		GDeadLockProfiler = new DeadLockProfiler();
		GGlobalQueue = new GlobalQueue();
		GJobTimer = new JobTimer();

		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GSendBufferManager;
		delete GDeadLockProfiler;
		delete GGlobalQueue;
		delete GJobTimer;

		SocketUtils::Clear();
	}
} GCoreGlobal;



