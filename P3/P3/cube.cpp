#include"d3dUtility.h"

//��ʼ��ȫ�ֱ���������Device�Լ����������嶥����������ݵ���������
IDirect3DDevice9 * Device = 0;
IDirect3DVertexBuffer9 *VB = 0;
IDirect3DIndexBuffer9 *IB = 0;
//Ԥ����Ļ�ֱ���
const int Width = 640;
const int Height = 480;

//���嶥��ṹ�͸ýṹ��FVF��������FVF������λ����Ϣ
struct Vertex{
	//�����FVF
	float _x, _y, _z;
	static const DWORD FVF;//DWORD,�൱��unsigned long
	//��Ҫ�Ĺ��캯��
	Vertex(){}
	Vertex(float x,float y,float z):_x(x),_y(y),_z(z){}
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

//��ܺ�������setup������display�����Լ�cleanup����

bool Setup()
{
	//1 �������㻺�����������
	Device->CreateVertexBuffer(
		8*sizeof(Vertex),		//�������ֽ���
		0,						//ָ�����ڻ����һЩ��������
		Vertex::FVF,			//FVF
		D3DPOOL_MANAGED,		//���ɻ�����ڴ��
		&VB,					//������ָ��
		0);						//���ã�Ϊ0����

	Device->CreateIndexBuffer(
		36*sizeof(WORD),
		D3DUSAGE_WRITEONLY,		//�������ԣ�ֻд
		D3DFMT_INDEX16,			//������С
		D3DPOOL_MANAGED,
		&IB,
		0);

	//2 ����������������cube������仺����
	Vertex* vertices;//�洢���㼯
	VB->Lock(0, 0, (void **)&vertices, 0);
	//����һ������cube�İ˸�����,�ֱ�����±�0~7�У��������������õ�
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
	vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
	vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
	vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
	vertices[7] = Vertex(1.0f, -1.0f, 1.0f);
	//��䶥�㻺������ϣ�����
	VB->Unlock();

	//����cube�ϵ������Σ����IB���������������ͬ
	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);
	//����cubeǰ���棬����vertices���±꣬ÿ�������Ϊ����������
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;
	//�����
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;
	//�����
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;
	//�ұ���
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;
	//�ϱ���
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;
	//�±���
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;

	IB->Unlock();
	
	//3 ���������Z���򸺷���ƽ�ƣ�ʹ��cube����������������ڣ�����ȡ���任����������������תΪ�۲�����
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);//�����λ��
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);//Ŀ��λ��
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);//���ϵ�λ����
	D3DXMATRIX V;//ȡ���任����
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	//4 ͶӰ�任
	D3DXMATRIX proj; //ͶӰ�任����
	D3DXMatrixPerspectiveFovLH(
		&proj,				//ͶӰ����
		D3DX_PI*0.5f,		//����������
		(float)Width/(float)Height,		//��Ļ���ݱ�
		1.0f,				//�����ƽ�����
		1000.0f);			//����Զƽ�����
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	//5 �����߿�ģʽ��������
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

bool Display(float timeDelta)//������ºͻ��Ƴ���
{
	if (Device)
	{
		//��ת������
		D3DXMATRIX Rx, Ry;
		//��x����ת45��
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);
		//����ÿһ֡��y�����ת�Ƕ�
		static float y = 0;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;
		if (y > 6.28f)
			y = 0.0f;
		//��Rx��Ry��ϳ�һ����ת����
		D3DXMATRIX p = Rx*Ry;
		Device->SetTransform(D3DTS_WORLD, &p);
		//���Ƴ���
		Device->Clear(
			0,			//�����о��ε���Ŀ
			0,			//��Ҫִ�������������Ļ��������
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,		//ָ����Ҫ����ı���
			0xffffffff,	//��ɫ
			1.0f,		//��Ȼ����ֵ
			0);			//ģ�建���ֵ

		//һ��׼����������ʼ��begin��end֮����û��Ʒ���
		Device->BeginScene();
		//1 ���ö�����������Դ
		Device->SetStreamSource(
			0,				//��ʶ�붥�㻺�潨�����ӵ������������ڲ����ö��������Ϊ0
			VB,				//���㻺��ָ��
			0,				//������������ʼ�������һ��ƫ��������λ���ֽ�
			sizeof(Vertex));//���㻺����ÿ��Ԫ�صĴ�С
		//2 ���ö����ʽ
		Device->SetFVF(Vertex::FVF);
		//3 ������������
		Device->SetIndices(IB);
		//4 ʹ����������
		Device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,//��Ҫ���Ƶ�ͼԪ����
			0,					//Ϊ�������ӵĻ���
			0,					//����ʹ�õ���С����ֵ
			8,					//������
			0,					//���㻺���б�ʶ�����Ķ�ȡ��ʼ�������
			12);				//��Ҫ���Ƶ�ͼԪ�������ڱ����У�cubeһ��6���棬ÿ��������������

		Device->EndScene();
		Device->Present(0, 0, 0, 0);//������̨�����ǰ̨���棬���ύ����Ⱦ�ܵ�������̨����������Ⱦ������
	}
	return true;
}

//�����ڴ�
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
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