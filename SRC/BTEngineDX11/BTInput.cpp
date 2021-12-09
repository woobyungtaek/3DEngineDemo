#include <windows.h>
#include "BTInput.h"

HWND BTInput::CurrentHWnd;
KeyState BTInput::mKeyStateArr[256];
POINT BTInput::MousePosScreen;
POINT BTInput::MousePosClient;

void BTInput::Init(HWND hWnd)
{
	CurrentHWnd = hWnd;
}

void BTInput::KeyUpdate()
{
	for (int index = 0; index < 255; index++)
	{
		mKeyStateArr[index].prev = mKeyStateArr[index].current;
		mKeyStateArr[index].current = HIWORD(GetAsyncKeyState(index));
	}
}

bool BTInput::InputKey(int vk)
{
	return mKeyStateArr[vk].current;
}

bool BTInput::InputKeyDown(int vk)
{
	return !mKeyStateArr[vk].prev && mKeyStateArr[vk].current;
}

bool BTInput::InputKeyUp(int vk)
{
	return mKeyStateArr[vk].prev && !mKeyStateArr[vk].current;
}

POINT BTInput::GetMousePosCT()
{
	MousePosClient = MousePosScreen;
	ScreenToClient(CurrentHWnd, &MousePosClient);
	return MousePosClient;
}

POINT BTInput::GetMousePosSC()
{
	return MousePosScreen;
}

void BTInput::MouseUpdate()
{
	GetCursorPos(&MousePosScreen);	
}