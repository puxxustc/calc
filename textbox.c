/*********************************************************************
*
*文件：textbox.c
*功能：实现自定义的输入文本框
*备注：
*
*********************************************************************/

#include <windows.h>
#include <stdio.h>
#include "global.h"
#include "button.h"
#include "calc_exp.h"

/******************************常数**********************************/
// 文本框的最大字符数
#define  MAX_LENGTH  1024
// 文本框一行显示的字符的最大总宽度
#define  MAX_WIDTH   300

// 自定义的特殊字符
#define CHAR_COS		('A'+ 1)
#define CHAR_COSH		('A'+ 2)
#define CHAR_DIV		('A'+ 3)
#define CHAR_LG			('A'+ 4)
#define CHAR_LN			('A'+ 5)
#define CHAR_MUL		('A'+ 6)
#define CHAR_PI			('A'+ 7)
#define CHAR_SIN		('A'+ 8)
#define CHAR_SINH		('A'+ 9)
#define CHAR_SQRT		('A'+10)
#define CHAR_SQRT3		('A'+11)
#define CHAR_TAN		('A'+12)
#define CHAR_TANH		('A'+13)
#define CHAR_POW2		('A'+14)
#define CHAR_POW3		('A'+15)
#define CHAR_POW		('A'+16)

// 各种字符的颜色
#define COLOR_FUN		RGB(  10, 36, 106)
#define COLOR_OP		RGB(176,   0, 255)
#define COLOR_NUM		RGB(255,   0,   0)
#define COLOR_CONST		RGB(  0, 128, 100)



/***********************************************************
*
*函数：get_char_width
*功能：得到一个字符的宽度
*备注：
*返回：
*
***********************************************************/
static int  get_char_width(char ch)
{
	switch (ch)
	{
		case CHAR_COS :
			return 40;
			break;

		case CHAR_COSH :
			return 40;
			break;

		case CHAR_DIV :
			return 20;
			break;

		case CHAR_LG :
			return 30;
			break;

		case CHAR_LN :
			return 30;
			break;

		case CHAR_MUL :
			return 20;
			break;

		case CHAR_PI :
			return 20;
			break;

		case CHAR_SIN :
			return 40;
			break;

		case CHAR_SINH :
			return 40;
			break;

		case CHAR_SQRT :
			return 20;
			break;

		case CHAR_SQRT3 :
			return 20;
			break;

		case CHAR_TAN :
			return 40;
			break;

		case CHAR_TANH :
			return 40;
			break;

		case CHAR_POW2 :
			return 10;
			break;

		case CHAR_POW3 :
			return 10;
			break;

		case CHAR_POW :
			return 10;
			break;

		default :
			return 10;
			break;
	}
}


/***********************************************************
*
*函数：draw_text
*功能：显示文本的函数
*备注：
*返回：
*
***********************************************************/
static void draw_text(HDC hdc)
{
	HFONT  font_big;			// 大字体
	HFONT  font_normal;			// 中等字体
	HFONT  font_small;			// 小字体
	HFONT  cur_font = NULL;		// DC中当前选入的DC
	COLORREF  cur_color = 0;	// 当前的颜色
	HPEN   pen;					// 画笔
	HPEN   old_pen;
	int  i;
	int  x = 5;					// 输出字符的x坐标

	// 创建大字体
	font_big = CreateFont(20, 10, 0, 0, FW_NORMAL, 0, 0, 0, 
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			"宋体");
	// 创建中等字体
	font_normal = CreateFont(16, 8, 0, 0, FW_NORMAL, 0, 0, 0, 
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		"宋体");
	// 创建小字体
	font_small = CreateFont(10, 5, 0, 0, FW_NORMAL, 0, 0, 0, 
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		"宋体");

	i = __text.draw_start;
	while ((__text.string[i] != '\0') && (x <= MAX_WIDTH+5-10))
	{
		switch(__text.string[i])
		{
			case CHAR_DIV :
				// 除法，宽度20
				if (cur_color != COLOR_OP)
				{
					SetTextColor(hdc, COLOR_OP);
					cur_color = COLOR_OP;
				}
				if (cur_font != font_big)
				{
					SelectObject(hdc, font_big);
					cur_font = font_big;
				}
				TextOutW(hdc, x, 0, (LPCWSTR)L"÷", 1);
				x += 20;
				break;


			case CHAR_MUL :
				// 乘法，宽度20
				if (cur_color != COLOR_OP)
				{
					SetTextColor(hdc, COLOR_OP);
					cur_color = COLOR_OP;
				}
				if (cur_font != font_big)
				{
					SelectObject(hdc, font_big);
					cur_font = font_big;
				}
				TextOutW(hdc, x, 0, (LPCWSTR)L"×", 1);
				x += 20;
				break;

			case CHAR_PI :
			case 'e' :
				if (cur_color != COLOR_CONST)
				{
					SetTextColor(hdc,COLOR_CONST);
					cur_color = COLOR_CONST;
				}
				if (cur_font != font_big)
				{
					SelectObject(hdc, font_big);
					cur_font = font_big;
				}
				if (__text.string[i] == CHAR_PI)
				{
					// 圆周率，宽度20
					TextOutW(hdc, x, 0, (LPCWSTR)L"π", 1);
					x += 20;
				}
				else
				{
					// 'e'，宽度10
					TextOut(hdc, x, 0, (LPCSTR)"e", 1);
					x+= 10;
				}
				break;


			case CHAR_SQRT :
				// 开平方，宽度20
				pen = CreatePen(PS_SOLID, 2, COLOR_OP);
				old_pen = (HPEN)SelectObject(hdc, pen);
				MoveToEx(hdc, x + 19, 5, NULL);
				LineTo(hdc, x + 10, 5);
				LineTo(hdc, x + 7, 17);
				LineTo(hdc, x + 4, 14);
				SelectObject(hdc, old_pen);
				DeleteObject(pen);
				x += 20;
				break;

			case CHAR_SQRT3 :
				// 开立方，宽度20
				pen = CreatePen(PS_SOLID, 2, COLOR_OP);
				old_pen = (HPEN)SelectObject(hdc, pen);
				MoveToEx(hdc, x + 19, 5, NULL);
				LineTo(hdc, x + 10, 5);
				LineTo(hdc, x + 7, 17);
				LineTo(hdc, x + 4, 14);
				SelectObject(hdc, old_pen);
				DeleteObject(pen);
				if (cur_color != COLOR_OP)
				{
					SetTextColor(hdc, COLOR_OP);
					cur_color = COLOR_OP;
				}
				if (cur_font != font_small)
				{
					SelectObject(hdc, font_small);
					cur_font = font_small;
				}
				TextOut(hdc, x + 2, 2, (LPCSTR)"3", 1);
				x += 20;
				break;


			case CHAR_LN :
			case CHAR_LG :
			case CHAR_SIN :
			case CHAR_COS :
			case CHAR_TAN :
				if (cur_color != COLOR_FUN)
				{
					SetTextColor(hdc, COLOR_FUN);
					cur_color = COLOR_FUN;
				}
				if (cur_font != font_big)
				{
					SelectObject(hdc, font_big);
					cur_font = font_big;
				}
				switch (__text.string[i])
				{
					case CHAR_LN :
						TextOut(hdc, x + 5, 0, (LPCSTR)"ln", 2);
						x += 30;
						break;

					case CHAR_LG :
						TextOut(hdc, x + 5, 0, (LPCSTR)"lg", 2);
						x += 30;
						break;

					case CHAR_SIN :
						TextOut(hdc, x + 5, 0, (LPCSTR)"sin", 3);
						x += 40;
						break;

					case CHAR_COS :
						TextOut(hdc, x + 5, 0, (LPCSTR)"cos", 3);
						x += 40;
						break;

					case CHAR_TAN :
						TextOut(hdc, x + 5, 0, (LPCSTR)"tan", 3);
						x += 40;
						break;
				}
				break;

			case CHAR_SINH :
			case CHAR_COSH :
			case CHAR_TANH :
				if (cur_color != COLOR_FUN)
				{
					SetTextColor(hdc, COLOR_FUN);
					cur_color = COLOR_FUN;
				}
				if (cur_font != font_normal)
				{
					SelectObject(hdc, font_normal);
					cur_font = font_normal;
				}
				switch (__text.string[i])
				{
					case CHAR_SINH :
						TextOut(hdc, x + 4, 2, (LPCSTR)"sinh", 4);
						x += 40;
						break;

					case CHAR_COSH :
						TextOut(hdc, x + 4, 2, (LPCSTR)"cosh", 4);
						x += 40;
						break;

					case CHAR_TANH :
						TextOut(hdc, x + 4, 2, (LPCSTR)"tanh", 4);
						x += 40;
						break;
				}
				break;

			case CHAR_POW2 :
			case CHAR_POW3 :
				if (cur_color != COLOR_OP)
				{
					SetTextColor(hdc, COLOR_OP);
					cur_color = COLOR_OP;
				}
				if (cur_font != font_small)
				{
					SelectObject(hdc, font_small);
					cur_font = font_small;
				}
				if (__text.string[i] == CHAR_POW2)
				{
					// 平方，宽度10
					TextOut(hdc, x+4, 2, (LPCSTR)"2", 1);
				}
				else
				{
					// 立方，宽度10
					TextOut(hdc, x+4, 2, (LPCSTR)"3", 1);
				}
				x += 10;
				break;

			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
			case '.' :
				// 数字，宽度10
				if (cur_color != COLOR_NUM)
				{
					SetTextColor(hdc, COLOR_NUM);
					cur_color = COLOR_NUM;
				}
				if (cur_font != font_big)
				{
					SelectObject(hdc, font_big);
					cur_font = font_big;
				}
				TextOut(hdc, x, 0, (LPCSTR)__text.string+i, 1);
				x += 10;
				break;

			case '+' :
			case '-' :
			case '(' :
			case ')' :
			case '^' :
				// 加法，减法，右括号，左括号，乘方宽度10
				if (cur_color != COLOR_OP)
				{
					SetTextColor(hdc, COLOR_OP);
					cur_color = COLOR_OP;
				}
				if (cur_font != font_big)
				{
					SelectObject(hdc, font_big);
					cur_font = font_big;
				}
				TextOut(hdc, x, 0, (LPCSTR)__text.string+i, 1);
				x += 10;
				break;
		}
		i++;

	}
	__text.draw_end = i;

	// 销毁字体
	DeleteObject(font_big);
	DeleteObject(font_normal);
	DeleteObject(font_small);
}


/***********************************************************
*
*函数：update_window
*功能：刷新一个窗体
*备注：
*返回：
*
***********************************************************/
void update_window(HWND hwnd)
{
	RECT rect;

	GetClientRect(hwnd, &rect);
	InvalidateRect(hwnd, &rect, TRUE);
	UpdateWindow(hwnd);
}


/***********************************************************
*
*函数：set_caret_pos
*功能：设置光标位置
*备注：
*返回：
*
***********************************************************/
void set_caret_pos(void)
{
	 int x = 4;
	 int i;
	 
	 for (i = __text.draw_start; i < __text.caret_position; i++)
	 {
		 x += get_char_width(__text.string[i]);
	 }
	 SetCaretPos(x, 3);
}


/***********************************************************
*
*函数：insert_char
*功能：在文本框中插入字符
*备注：
*返回：
*
***********************************************************/
int insert_char(char ch)
{
	int width;
	int i;

	if (__text.length >= MAX_LENGTH)
	{
		// 字符串过长
		return -1;
	}
	else
	{
		MoveMemory(__text.string + __text.caret_position + 1,
				__text.string + __text.caret_position,
				__text.length - __text.caret_position + 1);
		__text.string[__text.caret_position] = ch;
		__text.length ++;
		__text.caret_position++;
		width = 0;
		for (i = __text.draw_start; i < __text.caret_position; i++)
		{
			width += get_char_width(__text.string[i]);
		}
		if (width > MAX_WIDTH)
		{
			width = 40;
			while (width > 0)
			{
				width -= get_char_width(__text.string[__text.draw_start]);
				__text.draw_start++;
			}
		}
		// 告知paintboard文本已经改变
		SendMessage(__paintboard, WM_TEXT_CHANGE, 0, 0);
		// 刷新窗体
		update_window(__textbox);
		// 将焦点设置到文本框上
		SetFocus(__textbox);
		// 设置光标位置
		set_caret_pos();
		return 0;
	}
}


/***********************************************************
*
*函数：textbox_proc
*功能：文本框的窗口处理函数
*备注：
*返回：
*
***********************************************************/
static LRESULT CALLBACK textbox_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT  ps;	// 画刷
	HDC  hdc;			// 窗口DC
	RECT  rect;
	TCHAR  code;
	int  x;
	int  width;
	int  i;
	hpfloat value;
	char *str = NULL;
	int  result;

	switch (message)
	{
		case WM_COMMAND :
			// 自定义按钮发来的消息
			switch (wParam)
			{
				
				case BUTTON_ADD :
					// 加法
					insert_char('+');
					break;

				case BUTTON_COS :
					// 余弦
					insert_char(CHAR_COS);
					break;

				case BUTTON_COSH :
					// 双曲余弦
					insert_char(CHAR_COSH);
					break;

				case BUTTON_DIV :
					// 除法
					insert_char(CHAR_DIV);
					break;

				case BUTTON_DOT :
					// 小数点
					insert_char('.');
					break;

				case BUTTON_E :
					// e （自然对数的底）
					insert_char('e');
					break;

				case BUTTON_L_BRACKET :
					// 左括号
					insert_char('(');
					break;

				case BUTTON_LG :
					// lg （常用对数)
					insert_char(CHAR_LG);
					break;

				case BUTTON_LN :
					// ln （自然对数)
					insert_char(CHAR_LN);
					break;

				case BUTTON_MUL :
					// 乘法
					insert_char(CHAR_MUL);
					break;

				case BUTTON_NUM0 :
					// 数字0
					insert_char('0');
					break;

				case BUTTON_NUM1 :
					// 数字1
					insert_char('1');
					break;

				case BUTTON_NUM2 :
					// 数字2
					insert_char('2');
					break;

				case BUTTON_NUM3 :
					// 数字3
					insert_char('3');
					break;

				case BUTTON_NUM4 :
					// 数字4
					insert_char('4');
					break;

				case BUTTON_NUM5 :
					// 数字5
					insert_char('5');
					break;

				case BUTTON_NUM6 :
					// 数字6
					insert_char('6');
					break;

				case BUTTON_NUM7 :
					// 数字7
					insert_char('7');
					break;

				case BUTTON_NUM8 :
					// 数字8
					insert_char('8');
					break;

				case BUTTON_NUM9 :
					// 数字9
					insert_char('9');
					break;

				case BUTTON_PI :
					// PI 圆周率
					insert_char(CHAR_PI);
					break;

				case BUTTON_R_BRACKET :
					// 右括号
					insert_char(')');
					break;

				case BUTTON_SIN :
					// 正弦
					insert_char(CHAR_SIN);
					break;

				case BUTTON_SINH :
					// 双曲正弦
					insert_char(CHAR_SINH);
					break;

				case BUTTON_SQRT :
					// 开平方
					insert_char(CHAR_SQRT);
					break;

				case BUTTON_SQRT3 :
					// 开立方
					insert_char(CHAR_SQRT3);
					break;

				case BUTTON_SUB :
					// 减法
					insert_char('-');
					break;

				case BUTTON_TAN :
					// 正切
					insert_char(CHAR_TAN);
					break;

				case BUTTON_TANH :
					// 双曲正切
					insert_char(CHAR_TANH);
					break;

				case BUTTON_POW2 :
					// 平方
					insert_char(CHAR_POW2);
					break;

				case BUTTON_POW3 :
					// 立方
					insert_char(CHAR_POW3);
					break;

				case BUTTON_POW :
					// 乘方
					insert_char('^');
					break;

				case BUTTON_ARROW :
					// 退格
					SendMessage(hwnd, WM_KEYDOWN, VK_BACK, 0);
					SetFocus(__textbox);
					break;

				case BUTTON_CLEAR :
					// 清除
					if (__text.length != 0)
					{
						// 将__text设置为空字符串
						__text.string[0] = '\0';
						__text.length = 0;
						__text.draw_start = 0;
						__text.draw_end = 0;
						__text.caret_position = 0;
						// 告知paintboard文本已经改变
						SendMessage(__paintboard, WM_TEXT_CHANGE, 0, 0);
						// 刷新窗口
						update_window(hwnd);
						SetFocus(__textbox);
						set_caret_pos();
					}
					break;

				case BUTTON_EQUAL :
					// 开始计算
					if (__text.length == 0)
					{
						SetWindowText(__outbox, (LPCSTR)"请输入表达式！");
					}
					else
					{
						SetWindowText(__outbox, (LPCSTR)"正在计算...");
						result = calc_exp(__text.string, &value);
						if (result == 0)
						{
							if (str == NULL)
							{
								str = (char *)malloc(sizeof(char) * 4000);
							}
							if (str == NULL)
							{
								SetWindowText(__outbox, (LPCSTR)"内存分配错误！");
							}
							else
							{
								hp_to_str(&value, str);
								SetWindowText(__outbox, (LPCSTR)str);
							}
						}
						else if (result == 1)
						{
							SetWindowText(__outbox, (LPCSTR)"语法错误！");
						}
						else
						{
							if (value.error == OVER_FLOW)
							{
								SetWindowText(__outbox, (LPCSTR)"溢出！");
							}
							else if (value.error == DIVIDE_ZERO)
							{
								SetWindowText(__outbox, (LPCSTR)"除0错误！");
							}
							else if (value.error == MEM_ERROR)
							{
								SetWindowText(__outbox, (LPCSTR)"内存分配错误！");
							}
							else
							{
								SetWindowText(__outbox, (LPCSTR)"函数调用错误！");
							}
						}
					}
			}
			return 0;
			break;

		case WM_CREATE :
			// 窗口创建
			// 将焦点设置到窗口上
			SetFocus(hwnd);
			return 0;
			break;

		case WM_PAINT :
			// 窗口重绘
			hdc = BeginPaint(hwnd, &ps);
			// 获取客户区的尺寸
			GetClientRect(hwnd, &rect);	
			// 绘制文本
			draw_text(hdc);
			// 绘制边框
			DrawEdge(hdc, &rect, EDGE_SUNKEN, BF_RECT);
			EndPaint(hwnd, &ps);
			return 0;
			break;

		case WM_SETFOCUS :
			// 获得焦点
			CreateCaret(hwnd, (HBITMAP)NULL, 1, 17);	// 创建光标
			set_caret_pos();
			ShowCaret(hwnd);							// 显示光标
			return 0;
			break;

		case WM_KILLFOCUS :
			// 失去焦点
			HideCaret(hwnd);			// 隐藏光标
			DestroyCaret();				// 销毁光标
			return 0;
			break;

		case WM_CHAR :
			// 字符消息
			code = (TCHAR)wParam;
			switch (code)
			{
				case '0' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM0, 0);
					break;
				
				case '1' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM1, 0);
					break;

				case '2' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM2, 0);
					break;

				case '3' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM3, 0);
					break;

				case '4' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM4, 0);
					break;

				case '5' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM5, 0);
					break;

				case '6' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM6, 0);
					break;

				case '7' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM7, 0);
					break;

				case '8' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM8, 0);
					break;

				case '9' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_NUM9, 0);
					break;

				case '+' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_ADD, 0);
					break;

				case '-' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_SUB, 0);
					break;

				case '.' :
					// 小数点
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_DOT, 0);
					break;

				case '*' :
					//乘法
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_MUL, 0);
					break;

				case '/' :
				case '\\' :
					// 除法
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_DIV, 0);
					break;

				case '(' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_L_BRACKET, 0);
					break;

				case ')' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_R_BRACKET, 0);
					break;

				case '=' :
				case '\r' :
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_EQUAL, 0);
					break;

				case '@' :
					// Shift+2 平方
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_POW2, 0);
					break;

				case '#' :
					// Shift+3 立方
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_POW3, 0);
					break;

				case '^' :
					// 乘方
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_POW, 0);
					break;

				case 'p' :
					// PI 圆周率
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_PI, 0);
					break;

				case 'e' :
					// e 自然指数的底
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_E, 0);
					break;

				case 'c' :
					// Clear 清空
					SendMessage(hwnd, WM_COMMAND, (WPARAM)BUTTON_CLEAR, 0);
					break;
			}
			return 0;
			break;

		case WM_KEYDOWN :
			// 键盘按下
			code = (TCHAR)wParam;
			switch (code)
			{
				case VK_LEFT:
					// 左方向键
					if (__text.caret_position >= 1)
					{
						// 光标向前移动一个字符
						__text.caret_position--;
						if (__text.caret_position < __text.draw_start)
						{
							// 光标被移动到起始字符之前
							// 需要重画文本框
							__text.draw_start--;
							update_window(hwnd);
						}
					}
					break;
 
				case VK_RIGHT:
					// 右方向键
					if (__text.caret_position < __text.length)
					{
						// 光标后移一个字符
						__text.caret_position++;
						// 判断是否需要重画文本框
						width = 0;
						for (i = __text.draw_start; i < __text.caret_position; i++)
						{
							width += get_char_width(__text.string[i]);
						}
						if (width > MAX_WIDTH)
						{
							width = 40;
							while (width > 0)
							{
								width -= get_char_width(__text.string[__text.draw_start]);
								__text.draw_start++;
							}
							update_window(__textbox);
						}
					}
					break;
 
				case VK_HOME:
					// HOME 键
					// 光标移到最前
					__text.caret_position = 0;
					__text.draw_start = 0;
					update_window(hwnd);
					break;
 
				case VK_END:
					// END 键
					// 光标移到最后
					__text.caret_position = __text.length;
					__text.draw_start = __text.length-1;
					if (__text.draw_start < 0)
					{
						__text.draw_start = 0;
					}
					update_window(hwnd);
					break;

				case VK_BACK:
					// 退格键
					if (__text.caret_position >= 1)
					{
						// 当光标前面有字符时删除光标前面的字符
						MoveMemory(__text.string + __text.caret_position - 1,
								__text.string + __text.caret_position,
								__text.length - __text.caret_position + 1);
						__text.length--;
						// 光标前移一个字符
						__text.caret_position--;
						if ((__text.caret_position <= __text.draw_start) && (__text.draw_start > 0))
						{
							__text.draw_start --;
						}
						// 告知paintboard文本已经改变
						SendMessage(__paintboard, WM_TEXT_CHANGE, 0, 0);
						// 刷新窗口
						update_window(hwnd);
					}
					break;

				case VK_DELETE:
					// 删除键
					if (__text.caret_position < __text.length)
					{
						// 当光标后面有字符时删除光标后的字符
						MoveMemory(__text.string + __text.caret_position,
								__text.string + __text.caret_position + 1,
								__text.length - __text.caret_position + 1);
						__text.length--;
						// 告知paintboard文本已经改变
						SendMessage(__paintboard, WM_TEXT_CHANGE, 0, 0);
						update_window(hwnd);
					}
					break;
			}
			set_caret_pos();
			return 0;
			break;
 
		case WM_LBUTTONDOWN:
			// 鼠标左键按下
			// 设置焦点
			SetFocus(__textbox);
			// 计算光标的位置
			x = LOWORD(lParam);
			width = 0;
			i = 0;
			for (i = __text.draw_start; i < __text.draw_end; i++)
			{
				width += get_char_width(__text.string[i]);
				if (width > x)
				{
					break;
				}
			}
			__text.caret_position = i;
			// 设置光标位置
			set_caret_pos();
			return 0;
			break;

		case WM_DESTROY :
			if (str != NULL)
			{
				free(str);
			}
			result = DefWindowProc(hwnd, message, wParam, lParam);
			break;

		default :
			result = DefWindowProc(hwnd, message, wParam, lParam);
			break;
	}

	return result;
}


int init_textbox(void)
{
	WNDCLASSEX wc;									// 窗口类

	// 初始化窗口类
	wc.cbSize = sizeof(wc);							// 结构的大小
	wc.style = CS_HREDRAW | CS_VREDRAW;				// 窗口风格
	wc.lpfnWndProc = textbox_proc;					// 窗口函数指针
	wc.cbClsExtra = 0;								// 额外的类内存
	wc.cbWndExtra = 0;								// 额外的窗口内存
	wc.hInstance = __instance;						// 实例句柄
	wc.hIcon = NULL;								// 窗口图标
	wc.hCursor = LoadCursor(NULL, IDC_IBEAM);		// 窗口光标
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// 背景刷子
	wc.lpszClassName = "mytextbox";					// 窗口类名
	wc.lpszMenuName = NULL;							// 菜单
	wc.hIconSm = NULL;								// 小图标
	
	// 注册窗口类
	RegisterClassEx(&wc);

	// 初始化__text
	__text.string[0] = '\0';
	__text.length = 0;
	__text.caret_position = 0;
	__text.draw_start = 0;
	__text.draw_end = 0;

	// 创建文本框
	__textbox = CreateWindowEx(
			0,							// 扩展样式
			"mytextbox",				// 类名
			NULL,						// 窗口标题
			WS_CHILD | WS_VISIBLE		// 窗口风格: 子窗体，可见
			,
			40,							// 初始 X 坐标
			185,						// 初始 Y 坐标
			320,						// 宽度
			24,							// 高度
			__wnd_main,					// 父窗口句柄
			NULL,						// 菜单句柄
			__instance,					// 程序实例句柄
			NULL);						// 用户数据

	if ( __textbox == NULL )
	{
		return -1;
	}
	else
	{
		ShowWindow(__textbox, SW_SHOW);
		UpdateWindow(__textbox);
		return 0;
	}
}