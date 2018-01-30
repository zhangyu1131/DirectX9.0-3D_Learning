
#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0;
IDirect3DVertexBuffer9 * Quad = 0;
IDirect3DTexture9 * Tex = 0;//��Ҫӳ�������
const int Width = 640;
const int Height = 480;

//����ṹ
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
	float _u, _v; // ���������

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
	//��������������㣬��һ���ı����ĸ��������ԣ������������������ظ��ģ����ֻ���ĸ��㣬����Ҫ������������
	//�������黹������6���㣬��Ϊ���о�TexCube����ʱ����cubeҲ����24�����㣬��Ϊ����һ������ͼ����˵һ���������ڲ�ͬ���ʱ���߷���һ��
	v[0] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);//ͬ0
	v[4] = Vertex(1.0f, 1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);//ͬ2
	v[3] = Vertex(1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	Quad->Unlock();

	//���������������������
	D3DXCreateTextureFromFile(
		Device,
		"dx5_logo.bmp",	//����ͼ��
		&Tex);			//����ӿڶ���
	Device->SetTexture(0, Tex);//���õ�ǰ���������ڵ�һ�㣬D3D��һ����������8������
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);//��������������
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);//����������С����
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);//�༶�������������
	//�رչ���
	Device->SetRenderState(D3DRS_LIGHTING, false);
	//ͶӰ����
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
// WndProc���ڹ��̺���
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
// WinMain�����������
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	//��ʼ����ʾ���ں�direct3d
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

	//��display������Ϊ��ʾ������������Ϣѭ��
	d3d::EnterMsgLoop(Display);

	//ִ�������������ͷ�IDirect3DDevice9�ӿڶ���
	Cleanup();

	Device->Release();

	return 0;
}