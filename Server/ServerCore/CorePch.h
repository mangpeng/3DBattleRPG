#pragma once

// ServerCore 프로젝트 내부의 pch.h 파일은 외부 프로젝트에서
// 직접 참조할 수 없기 때문에
// 별도의 CorePch를 생성

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "Container.h"

#include <windows.h>
#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "ObjectPool.h"
#include "Memory.h"
#include "SendBuffer.h"
