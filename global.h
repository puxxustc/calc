/*********************************************************************
*
*文件：global.h
*功能：声明整个程序范围内的常量、全局变量等
*备注：
*
*********************************************************************/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <windows.h>

#ifdef __GLOBAL_GLOBAL_VARIABLES_HERE__
	#define EXTERN 
#else
	#define EXTERN  extern
#endif

/*********************************常数*******************************/
// textbox 发给paintboard、主窗口的消息
#define WM_TEXT_CHANGE (WM_USER+1)

// 文本框的最大字符数
#define  MAX_LENGTH  1024


/********************************类型******************************/
// 文本框中显示的字符串
typedef struct string
{
	char string[MAX_LENGTH+1];	// 字符串
	int  length;				// 字符串长度
	int  draw_start;			// 输出的起始位置
	int  draw_end;				// 输出的结束位置
	int  caret_position;		// 光标插入点位置
} string;


/********************************全局变量**************************/
EXTERN  HINSTANCE  __instance;	// 程序实例句柄
EXTERN  HWND  __wnd_main;		// 主窗口句柄
EXTERN  HWND  __textbox;		// 文本框句柄
EXTERN  HWND  __outbox;			// 显示结果的文本框的句柄
EXTERN  HWND  __paintboard;		// 画板的窗口句柄

EXTERN  string __text;			// 文本框的文本


/********************************************************************/
#endif