/*********************************************************************
*
*文件：button.c
*功能：实现自定义的按钮
*备注：
*
*********************************************************************/

#include <windows.h>
#include "global.h"
#include "resource.h"


// 所有按钮的ID
#define BUTTON_ADD			0
#define BUTTON_ARROW		1
#define BUTTON_COS			2
#define BUTTON_COSH			3
#define BUTTON_DIV			4
#define BUTTON_DOT			5
#define BUTTON_E			6
#define BUTTON_EQUAL		7
#define BUTTON_L_BRACKET	8
#define BUTTON_LG			9
#define BUTTON_LN			10
#define BUTTON_MUL			11
#define BUTTON_NUM0			12
#define BUTTON_NUM1			13
#define BUTTON_NUM2			14
#define BUTTON_NUM3			15
#define BUTTON_NUM4			16
#define BUTTON_NUM5			17
#define BUTTON_NUM6			18
#define BUTTON_NUM7			19
#define BUTTON_NUM8			20
#define BUTTON_NUM9			21
#define BUTTON_PI			22
#define BUTTON_R_BRACKET	23
#define BUTTON_SIN			24
#define BUTTON_SINH			25
#define BUTTON_SQRT			26
#define BUTTON_SQRT3		27
#define BUTTON_SUB			28
#define BUTTON_TAN			29
#define BUTTON_TANH			30
#define BUTTON_POW2			31
#define BUTTON_POW3			32
#define BUTTON_CLEAR		33
#define BUTTON_POW			34

// 按钮的状态
#define BUTTON_NORMAL		0		// 一般
#define BUTTON_MOUSE_ON		1		// 鼠标悬停在按钮上
#define BUTTON_MOUSE_DOWN	2		// 鼠标在按钮上按下

// 按钮的总个数
#define BUTTON_COUNT 35

// 存储按钮所有参数的类型
typedef struct button
{
	int   id;		// 按钮的ID
	int   x;		// 按钮左上角x坐标
	int   y;		// 按钮左上角y坐标
	int   width;	// 按钮宽度
	int   height;	// 按钮高度
	HWND  hwnd;		// 按钮句柄
	int   status;	// 按钮的状态
	struct
	{
		int normal;			// normal     状态下位图的资源ID
		int mouse_on;		// mouse_on   状态下位图的资源ID
		int mouse_down;		// mouse_down 状态下位图的资源ID
	} bmp_id;
	struct
	{
		HBITMAP normal;		// normal     状态下位图的句柄
		HBITMAP mouse_on;	// mouse_on   状态下位图的句柄
		HBITMAP mouse_down;	// mouse_down 状态下位图的句柄
	} bmp_handle;
} button;

// 所有的按钮
static button _buttons[BUTTON_COUNT] = 
	{
		// 加法
		{
			BUTTON_ADD, 302, 380, 35, 25, NULL, 0,
			{ IDB_ADD_NORMAL, IDB_ADD_MOUSE_ON, IDB_ADD_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 退格（箭头）
		{
			BUTTON_ARROW, 302, 320, 35, 25, NULL, 0,
			{ IDB_ARROW_NORMAL, IDB_ARROW_MOUSE_ON, IDB_ARROW_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// cos 余弦
		{
			BUTTON_COS, 62, 320, 35, 25, NULL, 0,
			{ IDB_COS_NORMAL, IDB_COS_MOUSE_ON, IDB_COS_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// cosh 双曲余弦
		{
			BUTTON_COSH, 62, 350, 35, 25, NULL, 0,
			{ IDB_COSH_NORMAL, IDB_COSH_MOUSE_ON, IDB_COSH_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 除法
		{
			BUTTON_DIV, 342, 350, 35, 25, NULL, 0,
			{ IDB_DIV_NORMAL, IDB_DIV_MOUSE_ON, IDB_DIV_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 小数点
		{
			BUTTON_DOT, 222, 410, 35, 25, NULL, 0,
			{ IDB_DOT_NORMAL, IDB_DOT_MOUSE_ON, IDB_DOT_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "e" （自然指数的底）
		{
			BUTTON_E, 302, 410, 35, 25, NULL, 0,
			{ IDB_E_NORMAL, IDB_E_MOUSE_ON, IDB_E_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "=" （等号）
		{
			BUTTON_EQUAL, 342, 410, 35, 25, NULL, 0,
			{ IDB_EQUAL_NORMAL, IDB_EQUAL_MOUSE_ON, IDB_EQUAL_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "(" 左括号
		{
			BUTTON_L_BRACKET, 62, 410, 35, 25, NULL, 0,
			{ IDB_L_BRACKET_NORMAL, IDB_L_BRACKET_MOUSE_ON, IDB_L_BRACKET_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// lg 常用对数
		{
			BUTTON_LG, 62, 380, 35, 25, NULL, 0,
			{ IDB_LG_NORMAL, IDB_LG_MOUSE_ON, IDB_LG_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// ln 自然对数
		{
			BUTTON_LN, 22, 380, 35, 25, NULL, 0,
			{ IDB_LN_NORMAL, IDB_LN_MOUSE_ON, IDB_LN_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "*" 乘法
		{
			BUTTON_MUL, 302, 350, 35, 25, NULL, 0,
			{ IDB_MUL_NORMAL, IDB_MUL_MOUSE_ON, IDB_MUL_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "0" 数字0
		{
			BUTTON_NUM0, 182, 410, 35, 25, NULL, 0,
			{ IDB_NUM0_NORMAL, IDB_NUM0_MOUSE_ON, IDB_NUM0_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "1" 数字1
		{
			BUTTON_NUM1, 182, 380, 35, 25, NULL, 0,
			{ IDB_NUM1_NORMAL, IDB_NUM1_MOUSE_ON, IDB_NUM1_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "2" 数字2
		{
			BUTTON_NUM2, 222, 380, 35, 25, NULL, 0,
			{ IDB_NUM2_NORMAL, IDB_NUM2_MOUSE_ON, IDB_NUM2_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "3" 数字3
		{
			BUTTON_NUM3, 262, 380, 35, 25, NULL, 0,
			{ IDB_NUM3_NORMAL, IDB_NUM3_MOUSE_ON, IDB_NUM3_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "4" 数字4
		{
			BUTTON_NUM4, 182, 350, 35, 25, NULL, 0,
			{ IDB_NUM4_NORMAL, IDB_NUM4_MOUSE_ON, IDB_NUM4_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "5" 数字5
		{
			BUTTON_NUM5, 222, 350, 35, 25, NULL, 0,
			{ IDB_NUM5_NORMAL, IDB_NUM5_MOUSE_ON, IDB_NUM5_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "6" 数字6
		{
			BUTTON_NUM6, 262, 350, 35, 25, NULL, 0,
			{ IDB_NUM6_NORMAL, IDB_NUM6_MOUSE_ON, IDB_NUM6_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "7" 数字7
		{
			BUTTON_NUM7, 182, 320, 35, 25, NULL, 0,
			{ IDB_NUM7_NORMAL, IDB_NUM7_MOUSE_ON, IDB_NUM7_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "8" 数字8
		{
			BUTTON_NUM8, 222, 320, 35, 25, NULL, 0,
			{ IDB_NUM8_NORMAL, IDB_NUM8_MOUSE_ON, IDB_NUM8_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "9" 数字9
		{
			BUTTON_NUM9, 262, 320, 35, 25, NULL, 0,
			{ IDB_NUM9_NORMAL, IDB_NUM9_MOUSE_ON, IDB_NUM9_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "π" 圆周率
		{
			BUTTON_PI, 262, 410, 35, 25, NULL, 0,
			{ IDB_PI_NORMAL, IDB_PI_MOUSE_ON, IDB_PI_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// ")" 右括号
		{
			BUTTON_R_BRACKET, 102, 410, 35, 25, NULL, 0,
			{ IDB_R_BRACKET_NORMAL, IDB_R_BRACKET_MOUSE_ON, IDB_R_BRACKET_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// sin 正弦
		{
			BUTTON_SIN, 22, 320, 35, 25, NULL, 0,
			{ IDB_SIN_NORMAL, IDB_SIN_MOUSE_ON, IDB_SIN_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// sinh 双曲正弦
		{
			BUTTON_SINH, 22, 350, 35, 25, NULL, 0,
			{ IDB_SINH_NORMAL, IDB_SINH_MOUSE_ON, IDB_SINH_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 开根号
		{
			BUTTON_SQRT,142, 350, 35, 25, NULL, 0,
			{ IDB_SQRT_NORMAL, IDB_SQRT_MOUSE_ON, IDB_SQRT_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 开三次方
		{
			BUTTON_SQRT3,142, 320, 35, 25, NULL, 0,
			{ IDB_SQRT3_NORMAL, IDB_SQRT3_MOUSE_ON, IDB_SQRT3_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// "-" 减法
		{
			BUTTON_SUB, 342, 380, 35, 25, NULL, 0,
			{ IDB_SUB_NORMAL, IDB_SUB_MOUSE_ON, IDB_SUB_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// tan 正切
		{
			BUTTON_TAN, 102, 320, 35, 25, NULL, 0,
			{ IDB_TAN_NORMAL, IDB_TAN_MOUSE_ON, IDB_TAN_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 双曲正切
		{
			BUTTON_TANH, 102, 350, 35, 25, NULL, 0,
			{ IDB_TANH_NORMAL, IDB_TANH_MOUSE_ON, IDB_TANH_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 平方
		{
			BUTTON_POW2, 102, 380, 35, 25, NULL, 0,
			{ IDB_POW2_NORMAL, IDB_POW2_MOUSE_ON, IDB_POW2_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 立方
		{
			BUTTON_POW3, 142, 380, 35, 25, NULL, 0,
			{ IDB_POW3_NORMAL, IDB_POW3_MOUSE_ON, IDB_POW3_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 清除
		{
			BUTTON_CLEAR, 342, 320, 35, 25, NULL, 0,
			{ IDB_CLEAR_NORMAL, IDB_CLEAR_MOUSE_ON, IDB_CLEAR_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		},
		// 指数
		{
			BUTTON_POW, 142, 410, 35, 25, NULL, 0,
			{ IDB_POW_NORMAL, IDB_POW_MOUSE_ON, IDB_POW_MOUSE_DOWN },
			{ NULL, NULL, NULL }
		}
	};


/***********************************************************
*
*函数：hwnd_to_id
*功能：根据按钮的句柄找到按钮的ID
*备注：
*返回：
*
***********************************************************/
static int hwnd_to_id(HWND hwnd)
{
	int i;

	for (i = 0; i < BUTTON_COUNT; i++)
	{
		if (_buttons[i].hwnd == hwnd)
		{
			break;
		}
	}
	return i;
}


/***********************************************************
*
*函数：is_mouse_on_window
*功能：判断鼠标是否在指定窗口的范围矩形内
*备注：
*返回：
*
***********************************************************/
static BOOL is_mouse_on_window(HWND hwnd)
{
	POINT point;
	RECT rect;

	GetCursorPos(&point);
	GetWindowRect(hwnd, &rect);
	return PtInRect(&rect, point);
}


/***********************************************************
*
*函数：button_proc
*功能：按钮的消息处理函数
*备注：
*返回：
*
***********************************************************/
static LRESULT CALLBACK button_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT  ps;
	HDC  hdc;
	HDC  mdc;
	LRESULT  result;
	RECT  rect;
	int  button_id;


	switch (message)  
	{
		case WM_PAINT :
			//窗口重绘消息   
			button_id = hwnd_to_id(hwnd);
			hdc = BeginPaint(hwnd, &ps); 
			// 创建内存DC
			mdc = CreateCompatibleDC(hdc);
			// 根据按钮的状态选择边框的颜色，装载位图
			if (_buttons[button_id].status == BUTTON_NORMAL)
			{
				SelectObject(mdc, _buttons[button_id].bmp_handle.normal);
			}
			else if (_buttons[button_id].status == BUTTON_MOUSE_ON)
			{
				SelectObject(mdc, _buttons[button_id].bmp_handle.mouse_on);
			}
			else
			{
				SelectObject(mdc, _buttons[button_id].bmp_handle.mouse_down);
			}
			GetClientRect(hwnd, &rect);
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, mdc, 0, 0, SRCCOPY);
			DeleteDC(mdc);
			EndPaint(hwnd, &ps);
			result = 0;
			break;

		case WM_MOUSEMOVE :
			// 鼠标移动
			button_id = hwnd_to_id(hwnd);
			if (is_mouse_on_window(hwnd))
			{
				// 鼠标在按钮上
				SetCapture(hwnd);	// 开始捕获鼠标输入
				if (GetAsyncKeyState(VK_LBUTTON))	// 判断鼠标左键的状态
				{
					// 鼠标左键被按下
					if (_buttons[button_id].status != BUTTON_MOUSE_DOWN)
					{
						_buttons[button_id].status = BUTTON_MOUSE_DOWN;
						InvalidateRect(hwnd, NULL, TRUE);
					}
				}
				else
				{
					// 鼠标左键未被按下
					if (_buttons[button_id].status != BUTTON_MOUSE_ON)
					{
						_buttons[button_id].status = BUTTON_MOUSE_ON;
						InvalidateRect(hwnd, NULL, TRUE);
					}
				}
			}
			else
			{
				// 鼠标不在按钮上
				ReleaseCapture();	// 释放鼠标捕获
				if (_buttons[button_id].status != BUTTON_NORMAL)
				{
					_buttons[button_id].status = BUTTON_NORMAL;
					InvalidateRect(hwnd, NULL, TRUE);
				}
			}
			result = 0;
			break;

		case WM_LBUTTONDOWN :
			// 鼠标左键按下
			SendMessage(hwnd, WM_MOUSEMOVE, 0, 0);
			result = 0;
			break;

		case WM_LBUTTONUP :
			// 鼠标左键抬起
			SendMessage(hwnd, WM_MOUSEMOVE, 0, 0);
			button_id = hwnd_to_id(hwnd);
			if (is_mouse_on_window(hwnd))
			{
				// 鼠标在按钮上
				SendMessage(__textbox, WM_COMMAND, (WPARAM)button_id, 0);
			}
			result = 0;
			break;

		default:
			//其他消息
			result = DefWindowProc(hwnd, message, wParam, lParam);
			break;
	}  
	return result;
}


/***********************************************************
*
*函数：create_button
*功能：创建按钮
*备注：
*返回：
*
***********************************************************/
static int create_button(int button_id)
{
	//创建窗口
	_buttons[button_id].hwnd = CreateWindowEx(
			0,								// 扩展样式
			"mybutton",						// 类名
			NULL,							// 窗口标题			
			WS_CHILDWINDOW | WS_VISIBLE		// 窗口风格: 子窗体，可见
			,
			_buttons[button_id].x,			// 初始 X 坐标
			_buttons[button_id].y,			// 初始 Y 坐标
			_buttons[button_id].width,		// 宽度
			_buttons[button_id].height,		// 高度
			__wnd_main,						// 父窗口句柄
			NULL,							// 菜单句柄
			__instance,						// 程序实例句柄
			NULL);							// 用户数据

	if (_buttons[button_id].hwnd == NULL)
	{
		return -1;
	}
	else
	{
		// 装载按钮所需的位图
		_buttons[button_id].bmp_handle.normal = LoadBitmap(__instance, (LPCSTR)_buttons[button_id].bmp_id.normal);
		_buttons[button_id].bmp_handle.mouse_on = LoadBitmap(__instance, (LPCSTR)_buttons[button_id].bmp_id.mouse_on);
		_buttons[button_id].bmp_handle.mouse_down = LoadBitmap(__instance, (LPCSTR)_buttons[button_id].bmp_id.mouse_down);
		// 显示按钮
		ShowWindow(_buttons[button_id].hwnd, SW_SHOW);
		UpdateWindow(_buttons[button_id].hwnd);
		return 0;
	}
}


/***********************************************************
*
*函数：init_button
*功能：初始化所有的按钮
*备注：
*返回：0 --成功 -1 --出错
*
***********************************************************/
int init_button(void)
{
	WNDCLASSEX wc;								// 窗口类
	int  i;

	// 初始化窗口类
	wc.cbSize = sizeof(wc);						// 结构的大小
	wc.style = CS_HREDRAW | CS_VREDRAW;			// 窗口风格
	wc.lpfnWndProc = button_proc;				// 窗口函数指针
	wc.cbClsExtra = 0;							// 额外的类内存
	wc.cbWndExtra = 0;							// 额外的窗口内存
	wc.hInstance = __instance;					// 实例句柄
	wc.hIcon = NULL;							// 窗口图标
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);	// 窗口光标
	wc.hbrBackground = NULL;					// 背景刷子
	wc.lpszClassName = "mybutton";				// 窗口类名
	wc.lpszMenuName = NULL;						// 菜单
	wc.hIconSm = NULL;							// 小图标


	// 注册窗口类
	RegisterClassEx(&wc);

	// 创建所有按钮
	for (i = 0; i < BUTTON_COUNT; i++)
	{
		if (create_button(i) != 0)
		{
			return -1;
		}
	}
	return 0; 
}


/***********************************************************
*
*函数：destroy_button
*功能：销毁所有按钮
*备注：
*返回：
*
***********************************************************/
int destroy_button(void)
{
	int i;

	for (i = 0; i < BUTTON_COUNT; i++)
	{
		// 删除所有已装载的位图
		DeleteObject(_buttons[i].bmp_handle.normal);
		DeleteObject(_buttons[i].bmp_handle.mouse_on);
		DeleteObject(_buttons[i].bmp_handle.mouse_down);
		// 给每个按钮发送关闭消息
		SendMessage(_buttons[i].hwnd, WM_CLOSE, 0, 0);
	}
	return 0;
}