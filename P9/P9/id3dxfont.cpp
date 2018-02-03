#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 640;
const int Height = 480;

ID3DXFont* Font = 0;
DWORD FrameCnt = 0;
float TimeElapsed = 0;
float FPS = 0;
char FPSString[20];
//
// Framework Functions
//

bool Setup()
{
	D3DXFONT_DESC df;
	ZeroMemory(&df, sizeof(D3DXFONT_DESC));
	df.Height = 25;
	df.Width = 12;
	df.Weight = 500;//boldness，中文魄力大力，还不太懂，需要从程序中体会一下
	df.MipLevels = D3DX_DEFAULT;
	df.Italic = false;//斜体
	df.CharSet = DEFAULT_CHARSET;
	df.OutputPrecision = 0;
	df.Quality = 0;
	df.PitchAndFamily = 0;
	strcpy_s(df.FaceName, "Times New Roman");

	if (FAILED(D3DXCreateFontIndirect(Device, &df, &Font)))
	{
		::MessageBox(0, "D3DXCreateFontIndirect() - FAILED", 0, 0);
		::PostQuitMessage(0);
	}
	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXFont*>(Font);
}

bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		FrameCnt++;
		TimeElapsed += timeDelta;
		if (TimeElapsed >= 1.0f)
		{
			FPS = (float)FrameCnt / TimeElapsed;
			sprintf_s(FPSString, "%f",FPS);
			FPSString[19] = '\0';
			TimeElapsed = 0;
			FrameCnt = 0;
		}
		//绘制
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();
		RECT rect = { 0, 0, Width, Height };
		Font->DrawTextA(0, FPSString, -1, &rect, DT_TOP | DT_LEFT, 0xff000000);
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		640, 480, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}