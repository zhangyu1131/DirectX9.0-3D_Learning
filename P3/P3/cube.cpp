#include"d3dUtility.h"

//初始化全局变量，包括Device以及保存立方体顶点和索引数据的两个变量
IDirect3DDevice9 * Device = 0;
IDirect3DVertexBuffer9 *VB = 0;
IDirect3DIndexBuffer9 *IB = 0;
//预设屏幕分辨率
const int Width = 640;
const int Height = 480;

//定义顶点结构和该结构的FVF，本例中FVF仅包含位置信息
struct Vertex{
	//坐标和FVF
	float _x, _y, _z;
	static const DWORD FVF;//DWORD,相当于unsigned long
	//必要的构造函数
	Vertex(){}
	Vertex(float x,float y,float z):_x(x),_y(y),_z(z){}
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

//框架函数，即setup函数、display函数以及cleanup函数

bool Setup()
{
	//1 创建顶点缓存和索引缓存
	Device->CreateVertexBuffer(
		8*sizeof(Vertex),		//缓冲区字节数
		0,						//指定关于缓存的一些附加属性
		Vertex::FVF,			//FVF
		D3DPOOL_MANAGED,		//容纳缓存的内存池
		&VB,					//创建的指针
		0);						//不用，为0即可

	Device->CreateIndexBuffer(
		36*sizeof(WORD),
		D3DUSAGE_WRITEONLY,		//附加属性，只写
		D3DFMT_INDEX16,			//索引大小
		D3DPOOL_MANAGED,
		&IB,
		0);

	//2 锁定缓冲区，利用cube数据填充缓冲区
	Vertex* vertices;//存储顶点集
	VB->Lock(0, 0, (void **)&vertices, 0);
	//设置一个独立cube的八个顶点,分别放在下标0~7中，下文索引将会用到
	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
	vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
	vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
	vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
	vertices[7] = Vertex(1.0f, -1.0f, 1.0f);
	//填充顶点缓冲区完毕，解锁
	VB->Unlock();

	//定义cube上的三角形，填充IB，步骤大致与上相同
	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);
	//设置cube前表面，利用vertices的下标，每个表面分为两个三角形
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;
	//后表面
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;
	//左表面
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;
	//右表面
	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;
	//上表面
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;
	//下表面
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;

	IB->Unlock();
	
	//3 将摄像机沿Z轴向负方向平移，使得cube处于摄像机的视域内，利用取景变换矩阵将上述世界坐标转为观察坐标
	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);//摄像机位置
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);//目标位置
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);//向上单位向量
	D3DXMATRIX V;//取景变换矩阵
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	//4 投影变换
	D3DXMATRIX proj; //投影变换矩阵
	D3DXMatrixPerspectiveFovLH(
		&proj,				//投影矩阵
		D3DX_PI*0.5f,		//摄像机视域角
		(float)Width/(float)Height,		//屏幕横纵比
		1.0f,				//距离近平面距离
		1000.0f);			//距离远平面距离
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	//5 设置线框模式绘制物体
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}

bool Display(float timeDelta)//负责更新和绘制场景
{
	if (Device)
	{
		//旋转立方体
		D3DXMATRIX Rx, Ry;
		//以x轴旋转45°
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);
		//增加每一帧中y轴的旋转角度
		static float y = 0;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;
		if (y > 6.28f)
			y = 0.0f;
		//将Rx和Ry结合成一个旋转矩阵
		D3DXMATRIX p = Rx*Ry;
		Device->SetTransform(D3DTS_WORLD, &p);
		//绘制场景
		Device->Clear(
			0,			//数组中矩形的数目
			0,			//所要执行清除操作的屏幕矩形数组
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,		//指定所要清除的表面
			0xffffffff,	//颜色
			1.0f,		//深度缓存的值
			0);			//模板缓存的值

		//一切准备就绪，开始在begin和end之间调用绘制方法
		Device->BeginScene();
		//1 设置顶点数据输入源
		Device->SetStreamSource(
			0,				//标识与顶点缓存建立连接的数据流，由于不适用多个流，设为0
			VB,				//顶点缓存指针
			0,				//自数据流的起始点算起的一个偏移量，单位是字节
			sizeof(Vertex));//顶点缓存中每个元素的大小
		//2 设置顶点格式
		Device->SetFVF(Vertex::FVF);
		//3 设置索引缓存
		Device->SetIndices(IB);
		//4 使用索引绘制
		Device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,//所要绘制的图元类型
			0,					//为索引增加的基数
			0,					//允许使用的最小索引值
			8,					//顶点数
			0,					//顶点缓存中标识索引的读取起始点的索引
			12);				//所要绘制的图元总数，在本例中，cube一共6个面，每个面两个三角形

		Device->EndScene();
		Device->Present(0, 0, 0, 0);//交换后台缓存和前台缓存，并提交到渲染管道，将后台缓存内容渲染出来。
	}
	return true;
}

//清理内存
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
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