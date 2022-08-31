#include "pch.h"
#include "PlayerManager.h"
#include "AccountManager.h"

PlayerManager GPlayerManager;

void PlayerManager::PlayerThenAccount()
{
	WRITE_LOCK;
	//this_thread::sleep_for(1s); // dead lock 강제로 발생
	GAccountManager.Lock();
}

void PlayerManager::Lock()
{
	WRITE_LOCK;
}
