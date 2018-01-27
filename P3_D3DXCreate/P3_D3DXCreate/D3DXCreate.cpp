//����һЩD3DX�Դ������񴴽�ͼ��

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0;
const int Width = 640;
const int Height = 480;

ID3DXMesh* Objects[5] = { 0, 0, 0, 0, 0 };
D3DXMATRIX ObjWorldMatrices[5];//��������Ӧ������ת������

//
// Framework Functions
//

bool Setup()
{
	//��������
	D3DXCreateTeapot(Device, &Objects[0], 0);//������
	D3DXCreateBox(Device,
		2.0f, 2.0f, 2.0f, //box�Ĳ���
		&Objects[1], 0);
	D3DXCreateCylinder(
		Device,
		1.0f, 1.0f, 3.0f, 10, 10,//���׵Ĳ���
		&Objects[2], 0);
	D3DXCreateTorus(
		Device,
		1.0f, // inner radius
		3.0f, // outer radius
		10,   // sides
		10,   // rings
		&Objects[3],
		0);

	D3DXCreateSphere(
		Device,
		1.0f, // radius
		10,   // slices
		10,   // stacks
		&Objects[4],
		0);
	
	//���������ֱ������������ת������
	D3DXMatrixTranslation(&ObjWorldMatrices[0], 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[1], -5.0f, 0.0f, 5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[2], 5.0f, 0.0f, 5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[3], -5.0f, 0.0f, -5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[4], 5.0f, 0.0f, -5.0f);

	//ͶӰ
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj, D3DX_PI*0.5f, (float)Width / (float)Height, 1, 1000);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	return true;
}

void Cleanup()
{
	for (int i = 0; i < 5; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}

bool Display(float timeDelta)
{
	if (Device) // Only use Device methods if we have a valid device.
	{
		//���������ʹ֮Χ��Զ����ת���Ҿ��벻�ϱ仯���γ�������ת����Ч
		static float angle = (3.0f * D3DX_PI) / 2.0f;//�������ת�Ƕ�
		static float cameraHeight = 0.0f;//�߶�
		static float cameraHeightDirection = 5.0f;//�߶ȱ仯����

		D3DXVECTOR3 position(cosf(angle) * 10.0f, cameraHeight, sinf(angle) * 10.0f);

		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);

		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);

		// ÿһ֡�ı�ǶȺ͸߶�
		angle += timeDelta;
		if (angle >= 6.28f)
			angle = 0.0f;
		cameraHeight += cameraHeightDirection * timeDelta;
		if (cameraHeight >= 10.0f)
			cameraHeightDirection = -5.0f;
		if (cameraHeight <= -10.0f)
			cameraHeightDirection = 5.0f;

		//��ʼ����
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		for (int i = 0; i < 5; i++)
		{
			Device->SetTransform(D3DTS_WORLD, &ObjWorldMatrices[i]);

			// Draw the object using the previously set world matrix.
			Objects[i]->DrawSubset(0);
		}

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