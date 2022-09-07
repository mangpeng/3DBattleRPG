#pragma once

#define WIN32_LEAN_AND_MEAN      

#ifdef  _DEBUG
#pragma  comment(lib, "ServerCore\\Debug\\ServerCore.Lib")
#pragma  comment(lib, "Protobuf\\Debug\\libprotobufd.Lib")
#else
#pragma  comment(lib, "ServerCore\\Release\\ServerCore.Lib")
#pragma  comment(lib, "Protobuf\\Release\\libprotobuf.Lib")
#endif 

#include "CorePch.h"
#include "Enum.pb.h"

using GameSessionRef = shared_ptr<class GameSession>;
using PlayerRef = shared_ptr<class Player>;