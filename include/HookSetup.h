#pragma once
#include <stdio.h>
#include "minhook/include/MinHook.h"

class HookSetup {
public:
	static bool Init();
	static bool CreateHook(void* function_ptr, void* detour_function, void** detour_function_original);
	static bool EnableHooks();
};