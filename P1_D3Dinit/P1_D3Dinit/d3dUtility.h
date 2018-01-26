#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include<d3d9.h>
#include<string>

namespace d3d{
	bool InitD3D(
		HINSTANCE hInstance, //HINSTANCE 是Windows里的一中数据类型，其实就是一个无符号的长整形，是32位的，是用于标示（记录）一个程序的实例。
		int width, int height,//后台缓存的长宽
		bool windowed,			//	是否窗口显示
		D3DDEVTYPE deviceType,	//设备类型
		IDirect3DDevice9 ** device);	//被创建的设备指针
	
	//该函数封装了应用程序的消息循环，接受一个函数指针。
	int EnterMsgLoop(
		bool (*ptr_display)(float timeDelta));

	LRESULT CALLBACK WndProc(	//应用程序主窗口的窗口过程函数
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);

	template<class T> void Release(T t)//方便释放COM接口
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
