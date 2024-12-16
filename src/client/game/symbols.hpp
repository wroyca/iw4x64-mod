#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{ 0x1401F8FD0 };

	WEAK symbol<void()> Com_Quit_f{ 0x1401FB240 };

	/***************************************************************
	 * Variables
	 **************************************************************/

	
}
