#pragma once

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "Container.h"

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <iostream>
#include <Windows.h>

#include "RWLock.h"

//using namespace std;

#include "CoreMacro.h"
#include "Memory.h"

#pragma comment(lib, "ws2_32.lib")

#include <windows.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

void HelloWorld();
