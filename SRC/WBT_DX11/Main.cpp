#include "pch.h"
#include "D3DApp.h"

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	D3DApp* pApp = D3DApp::GetInstance();

	pApp->Init(hInstance);
	pApp->Run();

	return 0;
}
