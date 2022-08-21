#pragma once

#define WIN32_LEAN_AND_MEAN      

#ifdef  _DEBUG
#pragma  comment(lib, "Debug\\ServerCore.Lib")
#else
#pragma  comment(lib, "Release\\ServerCore.Lib")
#endif 

#include "CorePch.h"