#pragma once

class KeyState
{
public:
	KeyState() : current(false), prev(false) {};
	~KeyState() {};

public:
	bool current;
	bool prev;
};

class BTInput
{
private :
	static HWND CurrentHWnd;
	static KeyState mKeyStateArr[256];
	static POINT MousePosScreen;
	static POINT MousePosClient;

public :
	static void Init(HWND hWnd);

	static void KeyUpdate();
	static bool InputKey(int vk);
	static bool InputKeyDown(int vk);
	static bool InputKeyUp(int vk);

	static POINT GetMousePosCT();
	static POINT GetMousePosSC();

	static void MouseUpdate();
};

