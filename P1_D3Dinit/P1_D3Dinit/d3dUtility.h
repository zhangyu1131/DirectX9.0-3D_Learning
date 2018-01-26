#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include<d3d9.h>
#include<string>

namespace d3d{
	bool InitD3D(
		HINSTANCE hInstance, //HINSTANCE ��Windows���һ���������ͣ���ʵ����һ���޷��ŵĳ����Σ���32λ�ģ������ڱ�ʾ����¼��һ�������ʵ����
		int width, int height,//��̨����ĳ���
		bool windowed,			//	�Ƿ񴰿���ʾ
		D3DDEVTYPE deviceType,	//�豸����
		IDirect3DDevice9 ** device);	//���������豸ָ��
	
	//�ú�����װ��Ӧ�ó������Ϣѭ��������һ������ָ�롣
	int EnterMsgLoop(
		bool (*ptr_display)(float timeDelta));

	LRESULT CALLBACK WndProc(	//Ӧ�ó��������ڵĴ��ڹ��̺���
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

	template<class T> void Release(T t)//�����ͷ�COM�ӿ�
	{
		if (t)
		{
			t->Release();
			t = 0;
		}
	}

	template<class T> void Delete(T t)
	{
		if (t)
		{
			delete t;
			t = 0;
		}
	}
}

#endif
