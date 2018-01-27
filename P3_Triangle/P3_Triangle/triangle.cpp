//练习按照线框模式绘制三角形

#include "d3dUtility.h"

//全局变量
IDirect3DDevice9* Device = 0;
const int Width = 640;
const int Height = 480;
IDirect3DVertexBuffer9 * VB;
IDirect3DIndexBuffer9 * IB;


//顶点结构
struct Vertex{
	float _x, _y, _z;
	static const DWORD FVF;

	Vertex(){}
	Vertex(float x, float y, float z) :_x(x), _y(y), _z(z){}
};
const DWORD Vertex::FVF = D3DFVF_XYZ;



//
// Framework Functions
//

bool Setup()
{
	//1 建立顶点和索引缓冲区
	Device->CreateVertexBuffer(3 * sizeof(Vertex),	D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &VB, 0);
	Device->CreateIndexBuffer(3 * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &IB, 0);

	Vertex* vers;
	VB->Lock(0, 0, (void**)&vers, 0);
	vers[0] = Vertex(-1.0f, 0.0f, 0.0f);
	vers[1] = Vertex(0.0f, 1.0f, 0.0f);
	vers[2] = Vertex(1.0f, 0.0f, 0.0f);
	VB->Unlock();

	WORD* indices;
	IB->Lock(0, 0, (void**)&indices, 0);
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	IB->Unlock();

	//设置摄像机
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	//投影
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5, (float)Width / (float)(Height), 1, 1000);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	//切换到线框模式
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
}

bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		//绕y轴旋转
		/*D3DXMATRIX Ry;
		static float y = 0;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;
		if (y > 6.28f)
			y = 0;
		Device->SetTransform(D3DTS_WORLD,&Ry);*/

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();
		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(Vertex::FVF);
		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
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