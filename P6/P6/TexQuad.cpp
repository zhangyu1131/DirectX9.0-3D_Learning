
#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0;
IDirect3DVertexBuffer9 * Quad = 0;
IDirect3DTexture9 * Tex = 0;//将要映射的纹理
const int Width = 640;
const int Height = 480;

//顶点结构
struct Vertex
{
	Vertex(){}
	Vertex(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v; // 纹理坐标对

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
//
// Framework Functions
//



bool Setup()
{
	Device->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &Quad, 0);

	Vertex* v;
	Quad->Lock(0, 0, (void **)&v, 0);
	//例子里给了六个点，但一个四边形四个顶点足以，六个点中有两个是重复的，如果只用四个点，则需要设置索引缓存
	//不过建议还是设置6个点，因为在研究TexCube例子时发现cube也用了24个顶点，因为对于一个立体图形来说一个点在属于不同面的时候法线方向不一样
	v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);//同0
	v[4] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);//同2
	v[3] = Vertex(1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	Quad->Unlock();

	//启用纹理并设置纹理过滤器
	D3DXCreateTextureFromFile(
		Device,
		"dx5_logo.bmp",	//纹理图像
		&Tex);			//纹理接口对象
	Device->SetTexture(0, Tex);//设置当前纹理，设置在第一层，D3D中一共可以设置8层纹理
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);//线性纹理最大过滤
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);//线性纹理最小过滤
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);//多级渐进纹理过滤器
	//关闭光照
	Device->SetRenderState(D3DRS_LIGHTING, false);
	//投影矩阵
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9 *>(Quad);
	d3d::Release<IDirect3DTexture9* >(Tex);
}

bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, Quad, 0, sizeof(Vertex));
		Device->SetFVF(Vertex::FVF);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc窗口过程函数
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
// WinMain，主函数入口
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	//初始化显示窗口和direct3d
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

	//将display函数作为显示函数，进入消息循环
	d3d::EnterMsgLoop(Display);

	//执行清屏操作并释放IDirect3DDevice9接口对象
	Cleanup();

	Device->Release();

	return 0;
}