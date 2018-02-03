#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 640;
const int Height = 480;

ID3DXMesh* Text = 0;

//
// Framework Functions
//

bool Setup()
{

	HDC hdc = CreateCompatibleDC(0);//该函数创建一个与应用程序的当前显示器兼容的内存设备上下文环境。
	HFONT hFont;
	HFONT hFontOld;

	//设置字体
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfHeight = 5;    
	lf.lfWidth = 5;   
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 500;  
	lf.lfItalic = true;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	strcpy_s(lf.lfFaceName, "Times New Roman"); // font style
	//创建字体
	hFont = CreateFontIndirect(&lf);
	hFontOld = (HFONT)SelectObject(hdc, hFont);//该函数选择一对象到指定的设备上下文环境中，该新对象替换先前的相同类型的对象。
	
	//创建文本
	D3DXCreateText(
		Device,//设备
		hdc,//一个设备环境句柄
		"Hello",//要显示的字符串指针
		0.001f,//最大弦偏差，非负，相当于原是字体的一个设计单位
		0.4f,//沿z轴负方向度量的字体深度
		&Text,//返回所创建的网格
		0,//所创建网格的邻接信息，暂且为0
		0);

	SelectObject(hdc, hFontOld);
	DeleteObject(hFont);
	DeleteDC(hdc);

	//
	//灯光
	//

	D3DXVECTOR3 dir(0.0f, -0.5f, 1.0f);
	D3DXCOLOR col = d3d::WHITE;
	D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// 摄像机
	//

	D3DXVECTOR3 pos(0.0f, 1.5f, -3.3f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(
		&V,
		&pos,
		&target,
		&up);

	Device->SetTransform(D3DTS_VIEW, &V);

	//
	// 投影
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.25f, // 45 - degree
		(float)Width / (float)Height,
		0.01f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Text);
}

bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		//摄像机每一帧旋转
		D3DXMATRIX yRot, T;

		static float y = 0.0f;

		D3DXMatrixRotationY(&yRot, y);
		y += timeDelta;

		if (y >= 6.28f)
			y = 0.0f;

		D3DXMatrixTranslation(&T, -1.6f, 0.0f, 0.0f);
		T = T * yRot;

		Device->SetTransform(D3DTS_WORLD, &T);

		//
		// 绘制
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();
		//开启融合,将字体透明化
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		D3DMATERIAL9 m = d3d::InitMtrl(d3d::WHITE, d3d::WHITE, d3d::WHITE, d3d::WHITE, 0.0f);
		m.Diffuse.a = 0.3f;
		Device->SetMaterial(&m);
		Text->DrawSubset(0);
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
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