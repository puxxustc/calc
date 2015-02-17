/*********************************************************************
*
*文件：paintboard.c
*功能：实现画板，绘制表达式
*备注：
*
*********************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"


/**************************常数****************************/
#define STACK_LENGTH	1024	// 栈的最大长度

// 自定义的特殊字符
#define CHAR_COS		('A' +  1)
#define CHAR_COSH		('A' +  2)
#define CHAR_DIV		('A' +  3)
#define CHAR_LG			('A' +  4)
#define CHAR_LN			('A' +  5)
#define CHAR_MUL		('A' +  6)
#define CHAR_PI			('A' +  7)
#define CHAR_SIN		('A' +  8)
#define CHAR_SINH		('A' +  9)
#define CHAR_SQRT		('A' + 10)
#define CHAR_SQRT3		('A' + 11)
#define CHAR_TAN		('A' + 12)
#define CHAR_TANH		('A' + 13)
#define CHAR_POW2		('A' + 14)
#define CHAR_POW3		('A' + 15)
#define CHAR_ELLIPSIS	('A' + 16)

// 各种字符的颜色
#define COLOR_FUN		RGB(  10, 36, 106)
#define COLOR_OP		RGB(176,   0, 255)
#define COLOR_NUM		RGB(255,   0,   0)
#define COLOR_CONST		RGB(  0, 128, 100)

// 运算符				(结合性 + 优先级 + ID)
#define OP_BEGIN		((1<<16) + (1<<8)  + 1)		// 开始
#define OP_END			((1<<16) + (0<<8)  + 2)		// 结束

#define OP_ADD			((0<<16) + (3<<8)  + 0)		// 加法
#define OP_SUB			((0<<16) + (3<<8)  + 1)		// 减法

#define OP_MUL			((0<<16) + (4<<8) +  0)		// 乘法
#define OP_DIV			((0<<16) + (4<<8) +  1)		// 除法

#define OP_SIN			((1<<16) + (5<<8) +  0)		// 正弦
#define OP_COS			((1<<16) + (5<<8) +  1)		// 余弦
#define OP_TAN			((1<<16) + (5<<8) +  2)		// 正切
#define OP_SINH			((1<<16) + (5<<8) +  3)		// 双曲正弦
#define OP_COSH			((1<<16) + (5<<8) +  4)		// 双曲余弦
#define OP_TANH			((1<<16) + (5<<8) +  5)		// 双曲正切
#define OP_LN			((1<<16) + (5<<8) +  6)		// 自然对数
#define OP_LG			((1<<16) + (5<<8) +  7)		// 常用对数
#define OP_SQRT			((1<<16) + (5<<8) +  8)		// 开平方
#define OP_SQRT3		((1<<16) + (5<<8) +  9)		// 开立方
#define OP_NEG			((1<<16) + (5<<8) + 10)		// 负号
#define OP_POW2			((0<<16) + (7<<8) +  1)		// 平方
#define OP_POW3			((0<<16) + (7<<8) +  2)		// 立方
#define OP_POW			((1<<16) + (5<<8) + 11)		// 乘方

#define OP_L_BARCKET	((2<<8) +  3)		// 左括号
#define OP_R_BARCKET	((1<<8) +  4)		// 右括号

#define OP_NUM			-1				// 不是运算符，只是用来保持一致
#define OP_CONST		-2				// 不是运算符，只是用来保持一致

/***************************类型**************************/
// 图元类型
typedef struct primitive
{
	char ch;
	int  x;				// 左上角x坐标
	int  y;				// 左上角y坐标
	int  width;			// 宽度
	int  height;		// 高度
} primitive;

// 链表节点类型
typedef struct node
{
	primitive prim;
	struct node *next;
} node;

// 表达式类型
typedef  struct expression
{
	int  width;			// 总高度
	int  height;		// 总宽度
	struct
	{
		node *head;		// 链表头指针
		node *end;		// 链表尾指针
		int  count;		// 图元总数
	} prims;			// 图元链
	int  weak_op;		// exp中未被括号括住的优先级最低的运算符
} expression;

typedef struct stack_op
{
	int  top;							// 栈顶索引
	int  ops[STACK_LENGTH];				// 元素
} stack_op;

typedef struct stack_exp
{
	int  top;							// 栈顶索引
	expression  exps[STACK_LENGTH];		// 元素
} stack_exp;


/***************************全局变量***************************/
static  int  _text_changed = 0;		// 1--文本已改变  0--文本未改变
static  HDC  _mdc;					// 内存DC
static  RECT _mdc_rect;				// 内存DC的范围矩形
static  HBITMAP  _bitmap;			// 位图

/*************************函数**********************************/
/*************************************************************
*
*函数：empty_exp
*功能：清空表达式
*
*
*************************************************************/
static void empty_exp(expression *exp)
{
	node *current;
	node *temp;

	current = exp->prims.head;
	while (current != NULL)
	{
		temp = current;
		current = current->next;
		free(temp);
	}
	exp->prims.head = NULL;
	exp->prims.end = NULL;
	exp->prims.count = 0;
	exp->height = 0;
	exp->width = 0;
}


/*************************************************************
*
*函数：add_barcket
*功能：在表达式两端加上括号
*
*
*************************************************************/
static int add_barcket(expression *exp)
{
	node *node_l, *node_r;	// 左右括号
	node *current;			// 用于遍历链表
	int offset_x;
	
	// 新建节点
	node_l = (node *)malloc(sizeof(node));
	if (node_l == NULL)
	{
		return -1;
	}
	node_r = (node *)malloc(sizeof(node));
	if (node_r == NULL)
	{
		return -1;
	}
	
	// 初始化node_l
	node_l->prim.ch = '(';
	node_l->prim.height = exp->height;
	node_l->prim.width = exp->height / 2;
	node_l->prim.x = 0;
	node_l->prim.y = 0;
	
	// 初始化node_r
	node_r->prim.ch = ')';
	node_r->prim.height = exp->height;
	node_r->prim.width = exp->height / 2;
	node_r->prim.x = node_l->prim.width + exp->width;
	node_r->prim.y = 0;
	
	// 计算exp中图元的位移
	offset_x = exp->height / 2;

	// 平移exp的图元
	current = exp->prims.head;
	while (current != NULL)
	{
		current->prim.x += offset_x;
		current = current->next;
	}

	// 连接node_l--exp--node_r
	node_l->next = exp->prims.head;
	exp->prims.head = node_l;
	exp->prims.end->next = node_r;
	exp->prims.end = node_r;
	node_r->next = NULL;
	
	// 设置exp的宽度
	exp->width = exp->width + exp->height; 
	// 设置exp的weak_op
	exp->weak_op = OP_POW2;
	// 设置exp的图元个数
	exp->prims.count += 2;
	return 0;
}


/*************************************************************
*
*函数：combine_exp
*功能：将两个（或一个）表达式、一个运算符组合成一个表达式
*
*
*************************************************************/
static int combine_exp(expression *exp_dst, expression *exp_src, int op)
{
	// 求最大值的宏
	#define MAX(a, b) (((a) > (b)) ? (a) : (b))
	
	node  *tmp_node;			// 用于插入运算符的节点
	node  *current;				// 用于遍历链表
	int  max_width;				// exp_dst 和 exp_src 中宽度的最大值
	int  max_height;			// exp_dst 和 exp_src 中高度的最大值
	int  offset_x, offset_y;	// 图元移动的位移

	// 新建节点
	tmp_node = (node *)malloc(sizeof(node));
	if (tmp_node == NULL)
	{
		return -1;
	}
	
	// 判断exp_dst、exp_src两端是否需要加上括号（除法、开平方、开立方不需要）
	if ((op != OP_DIV) && (op !=OP_SQRT) && (op != OP_SQRT3))
	{
		if ((op == OP_POW) & ((exp_dst->weak_op == OP_POW2) || (exp_dst->weak_op == OP_POW3)))
		{
			if (add_barcket(exp_dst) != 0)
				{
					return -1;
			}
		}
		if ((op & 0xff00) > (exp_dst->weak_op & 0xff00))
		{
			if (add_barcket(exp_dst) != 0)
			{
				return -1;
			}
		}
		if ((exp_src != NULL) && ((op & 0xff00) > (exp_src->weak_op & 0xff00)))
		{
			if (add_barcket(exp_src)!= 0)
			{
				return -1;
			}
		}
	}

	// 计算两个表达式的最大高度、最大宽度
	if (exp_src != NULL)
	{
		max_width = MAX(exp_dst->width, exp_src->width);
		max_height = MAX(exp_dst->height, exp_src->height);
	}
	else
	{
		max_width = exp_dst->width;
		max_height = exp_dst->height;
	}
	// 根据运算符连接表达式
	switch (op)
	{
		case OP_ADD :
		case OP_SUB :
		case OP_MUL :
			// 初始化tmp_node
			if (op == OP_ADD)
			{
				tmp_node->prim.ch = '+';
				tmp_node->prim.height = max_height / 2 + 10;
				tmp_node->prim.width = tmp_node->prim.height / 2;
			}
			else if (op == OP_SUB)
			{
				tmp_node->prim.ch = '-';
				tmp_node->prim.height = max_height / 2 + 10;
				tmp_node->prim.width = tmp_node->prim.height;
			}
			else
			{
				tmp_node->prim.ch = CHAR_MUL;
				tmp_node->prim.height = max_height / 2 + 10;
				tmp_node->prim.width = tmp_node->prim.height;
			}
			tmp_node->prim.x = exp_dst->width;
			tmp_node->prim.y = (max_height - tmp_node->prim.height) / 2;
			
			// 计算exp_dst中图元的位移
			offset_y = (max_height - exp_dst->height) / 2;

			// 平移exp_dst中的图元
			current = exp_dst->prims.head;
			while (current != NULL)
			{
				current->prim.y += offset_y;
				current = current->next;
			}

			// 计算exp_src中图元的位移
			offset_x = exp_dst->width + tmp_node->prim.width;
			offset_y = (max_height - exp_src->height) / 2;

			// 平移exp_src中的图元
			current = exp_src->prims.head;
			while (current != NULL)
			{
				current->prim.x += offset_x;
				current->prim.y += offset_y;
				current = current->next;
			}
			
			// 连接 exp_dst--tmp_node--exp_src
			exp_dst->prims.end->next = tmp_node;
			tmp_node->next = exp_src->prims.head;
			exp_dst->prims.end = exp_src->prims.end;
			
			// 设置exp_dst的宽度、高度
			exp_dst->width = exp_dst->width + exp_src->width + tmp_node->prim.width;
			exp_dst->height = max_height;
			// 设置exp_dst的weak_op
			exp_dst->weak_op = op;
			// 设置exp_dst的图元个数
			exp_dst->prims.count = exp_dst->prims.count + exp_src->prims.count + 1;
			break;

		case OP_DIV :
			// 初始化tmp_node
			tmp_node->prim.ch = CHAR_DIV;
			tmp_node->prim.width = max_width + 20;
			tmp_node->prim.height = 20;
			tmp_node->prim.x = 0;
			tmp_node->prim.y = exp_dst->height;
			
			// 计算exp_dst中图元的位移
			offset_x = (tmp_node->prim.width - exp_dst->width) / 2;
			
			// 平移exp_dst的图元
			current = exp_dst->prims.head;
			while (current != NULL)
			{
				current->prim.x += offset_x;
				current = current->next;
			}

			// 计算exp_src中图元的位移
			offset_x = (tmp_node->prim.width - exp_src->width) / 2;
			offset_y = exp_dst->height  + tmp_node->prim.height;

			// 平移exp_src的图元
			current = exp_src->prims.head;
			while (current != NULL)
			{
				current->prim.x += offset_x;
				current->prim.y += offset_y;
				current = current->next;
			}

			// 连接exp_dst--tmp_node--exp_src
			exp_dst->prims.end->next = tmp_node;
			tmp_node->next = exp_src->prims.head;
			exp_dst->prims.end = exp_src->prims.end;

			// 设置exp_dst的宽度、高度
			exp_dst->width = tmp_node->prim.width;
			exp_dst->height = exp_dst->height + 20 + exp_src->height;
			// 设置exp_dst的weak_op
			exp_dst->weak_op = op;
			// 设置exp_dst的图元个数
			exp_dst->prims.count = exp_dst->prims.count + exp_src->prims.count + 1;
			break;

		case OP_SIN :
		case OP_COS :
		case OP_TAN :
		case OP_SINH :
		case OP_COSH :
		case OP_TANH :
		case OP_LN :
		case OP_LG :
		case OP_NEG :
			// 初始化tmp_node
			switch (op)
			{
				case OP_SIN :
				case OP_COS :
				case OP_TAN :
					if (op == OP_SIN)
					{
						tmp_node->prim.ch = CHAR_SIN;
					}
					else if (op == OP_COS)
					{
						tmp_node->prim.ch = CHAR_COS;
					}
					else
					{
						tmp_node->prim.ch = CHAR_TAN;
					}
					tmp_node->prim.height = 20;
					tmp_node->prim.width = 40;
					tmp_node->prim.x = 0;
					tmp_node->prim.y = (max_height - 20) / 2;
					break;

				case OP_SINH :
				case OP_COSH :
				case OP_TANH :
					if (op == OP_SINH)
					{
						tmp_node->prim.ch = CHAR_SINH;
					}
					else if (op == OP_COSH)
					{
						tmp_node->prim.ch = CHAR_COSH;
					}
					else
					{
						tmp_node->prim.ch = CHAR_TANH;
					}
					tmp_node->prim.height = 20;
					tmp_node->prim.width = 50;
					tmp_node->prim.x = 0;
					tmp_node->prim.y = (max_height - 20) / 2;
					break;

				case OP_LN :
				case OP_LG :
					if (op == OP_LN)
					{
						tmp_node->prim.ch = CHAR_LN;
					}
					else
					{
						tmp_node->prim.ch = CHAR_LG;
					}
					tmp_node->prim.height = 20;
					tmp_node->prim.width = 30;
					tmp_node->prim.x = 0;
					tmp_node->prim.y = (max_height - 20) / 2;
					break;

				case OP_NEG :
					tmp_node->prim.ch = '-';
					tmp_node->prim.height = 20;
					tmp_node->prim.width = 20;
					tmp_node->prim.x = 0;
					tmp_node->prim.y = (max_height - 20) / 2;
					break;
			}

			// 计算exp_dst的位移
			offset_x = tmp_node->prim.width;

			// 平移exp_dst的图元
			current = exp_dst->prims.head;
			while (current != NULL)
			{
				current->prim.x += offset_x;
				current = current->next;
			}

			// 连接tmp_node--exp_dst
			tmp_node->next = exp_dst->prims.head;
			exp_dst->prims.head = tmp_node;
			
			// 设置exp_dst的宽度
			exp_dst->width = exp_dst->width + tmp_node->prim.width;
			// 设置exp_dst的weak_op
			exp_dst->weak_op = op;
			// 设置exp_dst的图元个数
			exp_dst->prims.count++;
			break;

		case OP_SQRT :
		case OP_SQRT3 :
			// 初始化tmp_node
			if (op == OP_SQRT)
			{
				tmp_node->prim.ch = CHAR_SQRT;
			}
			else
			{
				tmp_node->prim.ch = CHAR_SQRT3;
			}
			tmp_node->prim.height = exp_dst->height + exp_dst->height / 20 + 3;
			tmp_node->prim.width = exp_dst->width + tmp_node->prim.height;
			tmp_node->prim.x = 0;
			tmp_node->prim.y = 0;

			// 计算exp_dst的位移
			offset_x = exp_dst->height;
			offset_y = exp_dst->height / 20 + 3;

			// 平移exp_dst的图元
			current = exp_dst->prims.head;
			while (current != NULL)
			{
				current->prim.x += offset_x;
				current->prim.y += offset_y;
				current = current->next;
			}

			// 连接tmp_node--exp_dst
			tmp_node->next = exp_dst->prims.head;
			exp_dst->prims.head = tmp_node;

			// 设置exp_dst的宽度、高度
			exp_dst->width = tmp_node->prim.width;
			exp_dst->height = tmp_node->prim.height;
			// 设置exp_dst的weak_op
			exp_dst->weak_op = op;
			// 设置exp_dst的图元个数
			exp_dst->prims.count++;
			break;
		
		case OP_POW2 :
		case OP_POW3 :
			// 初始化tmp_node
			if (op == OP_POW2)
			{
				tmp_node->prim.ch = CHAR_POW2;
			}
			else
			{
				tmp_node->prim.ch = CHAR_POW3;
			}
			tmp_node->prim.height = exp_dst->height;
			tmp_node->prim.width = exp_dst->height / 2;
			tmp_node->prim.x = exp_dst->width;
			tmp_node->prim.y = 0;

			// 连接exp_dst--tmp_node
			exp_dst->prims.end->next = tmp_node;
			exp_dst->prims.end = tmp_node;
			tmp_node->next = NULL;

			// 设置exp_dst的宽度
			exp_dst->width = exp_dst->width + tmp_node->prim.width;
			// 设置exp_dst的weak_op
			exp_dst->weak_op = op;
			// 设置exp_dst的图元个数
			exp_dst->prims.count++;
			break;

		case OP_POW :
			// 释放tmp_node
			free(tmp_node);

			// 如果exp_dst的高度比exp_src高度的2倍小，把exp_dst高度放大成exp_src高度的2倍
			if (exp_dst->height < exp_src->height * 2)
			{
				current = exp_dst->prims.head;
				while (current != NULL)
				{
					current->prim.height = current->prim.height * exp_src->height * 2 / exp_dst->height;
					current->prim.width = current->prim.width * exp_src->height * 2 / exp_dst->height;
					current->prim.x = current->prim.x * exp_src->height * 2 / exp_dst->height;
					current->prim.y = current->prim.y * exp_src->height * 2 / exp_dst->height;
					current = current->next;
				}
				exp_dst->width = exp_dst->width * exp_src->height * 2 / exp_dst->height;
				exp_dst->height = exp_src->height * 2;
				
			}

			// 计算exp_src的位移
			offset_x = exp_dst->width;

			// 平移exp_src
			current = exp_src->prims.head;
			while (current != NULL)
			{
				current->prim.x += offset_x;
				current = current->next;
			}

			// 连接exp_dst--exp_src
			exp_dst->prims.end->next = exp_src->prims.head;
			exp_dst->prims.end = exp_src->prims.end;

			// 设置exp_dst的宽度、高度
			exp_dst->width = exp_dst->width + exp_src->width;
			// 设置exp_dst的weak_op
			exp_dst->weak_op = op;
			// 设置exp_dst的图元个数
			exp_dst->prims.count++;
			break;
	}
	return 0;
}


/***********************************************************
*
*函数：is_syntax_error
*功能：检查一个字符串形式的表达式是否有语法错误
*备注：
*返回：
*      0--无语法错误
*      1--出错，出错前面部分可以组成完整的表达式
*      2--出错，出错前面部分不能组成完整的表达式
*      *position存储出现错误的位置
*
***********************************************************/
static int is_syntax_error(const char *str, int *position)
{
	int  flag = 0;				// 0--未结束  1--发现错误结束  2--无错误结束
	int  dot_met = 0;			// 是否遇到小数点
	int  barcket_count = 0;		// 未匹配的括号个数
	int  last_op_type = 5;		// 上一个运算符的类型
	int  i;

	for (i = 0; flag == 0; i++)
	{
		switch (str[i])
		{
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
				if (last_op_type == 1)
				{
					if (str[i] == '.')
					{
						if (dot_met)
						{
							flag = 1;
						}
						else
						{
							dot_met = 1;
						}
					}
				}
				else
				{
					if ((last_op_type == 4) || (last_op_type == 6) || (last_op_type == 7))
					{
						flag = 1;
					}
					if (str[i] == '.')
					{
						if (dot_met)
						{
							flag = 1;
						}
						else
						{
							dot_met = 1;
						}
					}
					last_op_type = 1;
				}
				break;

			case 'e' :
			case CHAR_PI :
				if ((last_op_type == 1) || (last_op_type == 4) || (last_op_type == 6) || (last_op_type == 7))
				{
					flag = 1;
				}
				last_op_type = 7;
				break;

			case '+' :
			case CHAR_MUL :
			case CHAR_DIV :
			case '^' :
				if ((last_op_type == 2) || (last_op_type == 3) || (last_op_type == 5))
				{
					flag = 1;
				}
				last_op_type = 2;
				break;

			case '-' :
				if ((last_op_type == 2) || (last_op_type == 3) || (last_op_type == 5))
				{
					last_op_type = 3;
				}
				else
				{
					last_op_type = 2;
				}
				break;

			case CHAR_SIN :
			case CHAR_COS :
			case CHAR_TAN :
			case CHAR_SINH :
			case CHAR_COSH :
			case CHAR_TANH :
			case CHAR_LN :
			case CHAR_LG :
			case CHAR_SQRT :
			case CHAR_SQRT3 :
				if ((last_op_type == 1) || (last_op_type == 4) || (last_op_type == 6) || (last_op_type == 7))
				{
						flag = 1;
				}
				last_op_type = 3;
				break;

			case CHAR_POW2 :
			case CHAR_POW3 :
				if ((last_op_type == 2) || (last_op_type == 3) || (last_op_type == 5))
				{
					flag = 1;
				}
				last_op_type = 4;
				break;

			case '(' :
				if ((last_op_type == 1) || (last_op_type == 4) || (last_op_type == 6) || (last_op_type == 7))
				{
					flag = 1;
				}
				last_op_type = 5;
				barcket_count++;
				break;

			case ')' :
				if ((last_op_type == 2) || (last_op_type == 3) || (last_op_type == 5))
				{
					flag = 1;
				}
				last_op_type = 6;
				if (barcket_count == 0)
				{
					flag = 1;
				}
				else
				{
					barcket_count--;
				}
				break;
			
			case '\0' :
				if ((last_op_type == 2) || (last_op_type == 3) || (last_op_type == 5))
				{
					flag = 1;
				}
				else
				{
					flag = 2;
				}
				break;
		}
	}
	if (flag == 1)
	{
		*position = i - 1;
		if ((last_op_type == 1) || (last_op_type == 7))
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		return 0;
	}
}


/***********************************************************
*
*函数：str_to_exp()
*功能：把一个字符串转化成表达式
*
*
***********************************************************/
static int str_to_exp(const char *str, expression *exp)
{
	stack_op   s_op;			// 运算符栈
	stack_exp  s_exp;			// 表达式栈
	
	int  end = 0;				// 0--表达式还未结束，1--表达式结束
	int  cur_op;				// 当前运算符
	int  syntax_error;			// 是否遇到语法错误
	int  position;				// 如果字符串中有语法错误，position记录了第一次出现语法错误的位置
	node *temp;
	char *tmp_str;

	
	syntax_error = is_syntax_error(str, &position);
	
	if (syntax_error == 1)
	{	
		// 表达式出错，出错前面部分可以组成完整的表达式
		// 将出错前面的部分转换到exp中
		tmp_str = (char *)malloc(position + 1);
		memmove(tmp_str, str, position);
		tmp_str[position] = '\0';
		str = tmp_str;
	}
	else if (syntax_error == 2)
	{
		// 表达式出错，出错前面部分不能组成完整的表达式
		// 在出错前面部分后加上省略号后转换到exp中
		tmp_str = (char *)malloc(position + 2);
		memmove(tmp_str, str, position);
		tmp_str[position] = CHAR_ELLIPSIS;
		tmp_str[position + 1] = '\0';
		str = tmp_str;
	}

	// 将两个栈清零
	memset(&s_op, 0, sizeof(s_op));
	memset(&s_exp, 0, sizeof(s_exp));

	s_op.top = -1;
	s_exp.top = -1;
	
	// 向运算符栈中压入开始运算符
	s_op.top++;
	s_op.ops[s_op.top] = OP_BEGIN;
	
	cur_op = OP_BEGIN;

	for (; !end; str++)
	{
		if ((*str == '.') || ((*str >= '0') && (*str <= '9'))) 
		{
			// 是数字，创建节点
			temp = (node *)malloc(sizeof(node));
			if (temp == NULL)
			{
				free(tmp_str);
				return -1;
			}
			// 初始化temp
			temp->prim.ch = *str;
			temp->prim.height = 20;
			temp->prim.width = 10;
			temp->next = NULL;
			if (cur_op != OP_NUM)
			{	
				// 上一个字符是运算符，压入表达式栈
				s_exp.top++;
				temp->prim.x = 0;
				temp->prim.y = 0;
				s_exp.exps[s_exp.top].height = 20;
				s_exp.exps[s_exp.top].width = 10;
				s_exp.exps[s_exp.top].prims.count = 1;
				s_exp.exps[s_exp.top].prims.head = temp;
				s_exp.exps[s_exp.top].prims.end = temp;
				s_exp.exps[s_exp.top].weak_op = OP_NUM;
				cur_op = OP_NUM;
			}
			else
			{	
				// 上一个字符是数字，连接到表达式栈的栈顶的表达式中
				temp->prim.x = s_exp.exps[s_exp.top].width;
				temp->prim.y = 0;
				s_exp.exps[s_exp.top].width += 10;
				s_exp.exps[s_exp.top].prims.count++;
				s_exp.exps[s_exp.top].prims.end->next = temp;
				s_exp.exps[s_exp.top].prims.end = temp;
			}
		}
		else if ( *str == CHAR_ELLIPSIS)
		{
			// 是省略号，当成数字处理
			// 创建节点
			temp = (node *)malloc(sizeof(node));
			if (temp == NULL)
			{
				return -1;
			}
			// 初始化temp
			temp->prim.ch = *str;
			temp->prim.height = 20;
			temp->prim.width = 30;
			temp->next = NULL;
			temp->prim.x = 0;
			temp->prim.y = 0;
			// 压入表达式栈
			s_exp.top++;
			s_exp.exps[s_exp.top].height = 20;
			s_exp.exps[s_exp.top].width = 30;
			s_exp.exps[s_exp.top].prims.count = 1;
			s_exp.exps[s_exp.top].prims.head = temp;
			s_exp.exps[s_exp.top].prims.end = temp;
			s_exp.exps[s_exp.top].weak_op = OP_NUM;
			cur_op = OP_NUM;
		}
		else if ((*str == CHAR_PI) || (*str == 'e'))
		{
			// 是常量，当成数字处理
			// 创建节点
			temp = (node *)malloc(sizeof(node));
			if (temp == NULL)
			{
				free(tmp_str);
				return -1;
			}
			// 初始化temp
			temp->prim.ch = *str;
			temp->prim.height = 20;
			if (*str == CHAR_PI)
			{
				temp->prim.width = 20;
			}
			else
			{
				temp->prim.width = 10;
			}
			temp->next = NULL;
			temp->prim.x = 0;
			temp->prim.y = 0;
			// 压入表达式栈
			s_exp.top++;
			s_exp.exps[s_exp.top].height = 20;
			if (*str == CHAR_PI)
			{
				s_exp.exps[s_exp.top].width = 20;
			}
			else
			{
				s_exp.exps[s_exp.top].width = 10;
			}
			s_exp.exps[s_exp.top].prims.count = 1;
			s_exp.exps[s_exp.top].prims.head = temp;
			s_exp.exps[s_exp.top].prims.end = temp;
			s_exp.exps[s_exp.top].weak_op = OP_CONST;
			cur_op = OP_CONST;
		}
		else
		{
			// 是运算符
			// 根据字符串得到运算符ID
			switch (*str)
			{
				case '+' :
					cur_op = OP_ADD;
					break;
				case '-' :
					// 根据上一个字符的类型判断'-'代表减号还是负号
					if (   (cur_op == OP_NUM)
						|| (cur_op == OP_POW2)
						|| (cur_op == OP_POW3)
						|| (cur_op == OP_R_BARCKET))
					{
						cur_op = OP_SUB;
					}
					else
					{
						cur_op = OP_NEG;
					}
					break;
				case CHAR_MUL :
					cur_op = OP_MUL;
					break;
				case CHAR_DIV :
					cur_op = OP_DIV;
					break;
				case '^' :
					cur_op = OP_POW;
					break;
				case CHAR_SIN :
					cur_op = OP_SIN;
					break;
				case CHAR_COS :
					cur_op = OP_COS;
					break;
				case CHAR_TAN :
					cur_op = OP_TAN;
					break;
				case CHAR_SINH :
					cur_op = OP_SINH;
					break;
				case CHAR_COSH :
					cur_op = OP_COSH;
					break;
				case CHAR_TANH :
					cur_op = OP_TANH;
					break;
				case CHAR_LN :
					cur_op = OP_LN;
					break;
				case CHAR_LG :
					cur_op = OP_LG;
					break;
				case CHAR_SQRT :
					cur_op = OP_SQRT;
					break;
				case CHAR_SQRT3 :
					cur_op = OP_SQRT3;
					break;
				case CHAR_POW2 :
					cur_op = OP_POW2;
					break;
				case CHAR_POW3 :
					cur_op = OP_POW3;
					break;
				case '(' :
					cur_op = OP_L_BARCKET;
					break;
				case ')' :
					cur_op = OP_R_BARCKET;
					break;
				case '\0' :
					cur_op = OP_END;
					break;
			}//switch

			if (cur_op == OP_L_BARCKET)
			{	
				// '(' 左括号直接入栈
				s_op.top++;
				s_op.ops[s_op.top] = cur_op;
			}
			else
			{
				// 当前运算符优先级比前一个优先级低，弹栈，直到当前运算符优先级比前一个优先级高或相等
				while (    ((cur_op & 0xff00) < (s_op.ops[s_op.top] & 0xff00))
						|| (((cur_op & 0xff00) == (s_op.ops[s_op.top] & 0xff00)) && ((cur_op & 0xff0000) == 0)) )
				{
					if ((cur_op == OP_R_BARCKET) && (s_op.ops[s_op.top] == OP_L_BARCKET))
					{
						// 左右括号匹配，消去括号
						s_op.top--;
						break;
					}
					else if ((cur_op == OP_END) && (s_op.ops[s_op.top] == OP_L_BARCKET))
					{
						// OP_END也可以消去括号
						s_op.top--;
					}
					else if ((cur_op == OP_END) && (s_op.ops[s_op.top] == OP_BEGIN))
					{
						// OP_BEGIN与OP_END相遇，表达式结束
						end = 1;
						break;
					}
					else
					{
						if (   (s_op.ops[s_op.top] == OP_ADD)
							|| (s_op.ops[s_op.top] == OP_SUB)
							|| (s_op.ops[s_op.top] == OP_MUL)
							|| (s_op.ops[s_op.top] == OP_DIV)
							|| (s_op.ops[s_op.top] == OP_POW))
						{
							// 双目运算符
							combine_exp(&s_exp.exps[s_exp.top-1], &s_exp.exps[s_exp.top], s_op.ops[s_op.top]);
							s_op.top--;
							s_exp.top--;
						}
						else
						{
							// 单目运算符
							combine_exp(&s_exp.exps[s_exp.top], NULL, s_op.ops[s_op.top]);
							s_op.top--;
						}
					}
				}//while
				if ((cur_op != OP_R_BARCKET) && (cur_op != OP_END))
				{
					// 压栈
					s_op.top++;
					s_op.ops[s_op.top] = cur_op;
				}
			}
		}//if
	}
	*exp = s_exp.exps[s_exp.top];
	return 0;
}


/***********************************************************
*
*函数：draw_primitive
*功能：绘制图元
*备注：
*返回：
*
***********************************************************/
static void draw_primitive(HDC hdc, const primitive *prim)
{
	HFONT  font;
	HFONT  old_font;
	HPEN   pen;
	HPEN   old_pen;

	switch (prim->ch)
	{
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
			// 创建字体
			font =  CreateFont(prim->height, prim->width, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			// 选入字体
			old_font = (HFONT)SelectObject(hdc, font);
			// 设置颜色
			SetTextColor(hdc, COLOR_NUM);
			TextOut(hdc, prim->x, prim->y, (LPCSTR)(&prim->ch), 1);
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case '+' :
			// 创建字体
			font =  CreateFont(prim->height, prim->width, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, COLOR_OP);
			TextOut(hdc, prim->x, prim->y, (LPCSTR)("+"), 1);
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case '-' :
			// 创建字体
			font =  CreateFont(prim->height, prim->height / 2, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, COLOR_OP);
			if (prim->width == prim->height)
			{
				// 是减号
				TextOut(hdc, prim->x + prim->width / 4, prim->y, (LPCSTR)("-"), 1);
			}
			else
			{
				// 是负号
				TextOut(hdc, prim->x, prim->y, (LPCSTR)("-"), 1);
			}
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case CHAR_MUL :
			// 创建字体
			font =  CreateFont(prim->height * 2 / 3, prim->height / 3, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, COLOR_OP);
			TextOutW(hdc, prim->x + prim->height / 5, prim->y + prim->width / 5, (LPCWSTR)L"×", 1);
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case CHAR_DIV :
			pen = CreatePen(PS_SOLID, prim->height / 40 + 1, COLOR_OP);
			old_pen = (HPEN)SelectObject(hdc, pen);
			MoveToEx(hdc, prim->x, prim->y + prim->height / 2, NULL);
			LineTo(hdc, prim->x + prim->width, prim->y + prim->height / 2);
			SelectObject(hdc, old_pen);
			DeleteObject(old_pen);
			break;

		case CHAR_SIN :
		case CHAR_COS :
		case CHAR_TAN :
		case CHAR_SINH :
		case CHAR_COSH :
		case CHAR_TANH :
		case CHAR_LN :
		case CHAR_LG :
			// 创建字体
			font =  CreateFont(prim->height, prim->height / 2, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, COLOR_FUN);
			if (prim->ch == CHAR_SIN)
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"sin", 3);
			}
			else if (prim->ch == CHAR_COS)
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"cos", 3);
			}
			else if (prim->ch == CHAR_TAN)
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"tan", 3);
			}
			else if (prim->ch == CHAR_SINH)
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"sinh", 4);
			}
			else if (prim->ch == CHAR_COSH)
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"cosh", 4);
			}
			else if (prim->ch == CHAR_TANH)
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"tanh", 4);
			}
			else if (prim->ch == CHAR_LN)
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"ln", 2);
			}
			else
			{
				TextOut(hdc, prim->x + prim->height / 4, prim->y, (LPCSTR)"lg", 3);
			}
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case CHAR_POW2 :
		case CHAR_POW3 :
			font =  CreateFont(prim->height / 2, prim->width, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, COLOR_OP);
			if (prim->ch == CHAR_POW2)
			{
				TextOut(hdc, prim->x, prim->y, (LPCSTR)"2", 1);
			}
			else
			{
				TextOut(hdc, prim->x, prim->y, (LPCSTR)"3", 1);
			}
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case CHAR_SQRT :
		case CHAR_SQRT3 :
			pen = CreatePen(PS_SOLID, prim->height / 40 + 1, COLOR_OP);
			old_pen = (HPEN)SelectObject(hdc, pen);
			MoveToEx(hdc, prim->x + prim->width, prim->y + prim->height / 40 + 3, NULL);
			LineTo(hdc, prim->x + prim->height * 20 / 21 - 3, prim->y + prim->height / 40 + 3);
			LineTo(hdc, prim->x + prim->height * 2 / 3, prim->y + prim->height - prim->height / 40 - 3);
			LineTo(hdc, prim->x + prim->height / 3, prim->y + prim->height * 2 / 3);
			LineTo(hdc, prim->x + prim->height / 6, prim->y + prim->height * 4 / 5);
			SelectObject(hdc, old_pen);
			DeleteObject(old_pen);
			if (prim->ch == CHAR_SQRT3)
			{
				font =  CreateFont(prim->height *3 / 5, prim->height / 2, 0, 0, FW_NORMAL, 0, 0, 0, 
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				"宋体");
				old_font = (HFONT)SelectObject(hdc, font);
				SetTextColor(hdc, COLOR_OP);
				TextOut(hdc, prim->x + prim->height / 5, prim->y + 1, (LPCSTR)"3", 1);
				SelectObject(hdc, old_font);
				DeleteObject(font);
			}
			break;

		case '(' :
		case ')' :
			font =  CreateFont(prim->height, prim->width, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, COLOR_OP);
			TextOut(hdc, prim->x, prim->y, (LPCSTR)(&prim->ch), 1);
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case CHAR_ELLIPSIS :
			// 省略号
			font = CreateFont(prim->height, prim->height / 2, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, RGB(255, 0, 0));
			TextOut(hdc, prim->x, prim->y, (LPCSTR)("..."), 3);
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;

		case CHAR_PI :
		case 'e' :
			// 创建字体
			font =  CreateFont(prim->height, prim->height / 2, 0, 0, FW_NORMAL, 0, 0, 0, 
					ANSI_CHARSET,
					OUT_DEFAULT_PRECIS,
					CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY,
					DEFAULT_PITCH | FF_SWISS,
					"宋体");
			old_font = (HFONT)SelectObject(hdc, font);
			SetTextColor(hdc, COLOR_CONST);
			if (prim->ch == 'e')
			{
				TextOut(hdc, prim->x, prim->y, (LPCSTR)"e", 1);
			}
			else
			{
				TextOutW(hdc, prim->x, prim->y, (LPCWSTR)L"π", 1);
			}
			SelectObject(hdc, old_font);
			DeleteObject(font);
			break;
	}

}


/***********************************************************
*
*函数：draw_exp
*功能：绘制表达式
*备注：
*返回：
*
***********************************************************/
static void draw_exp(HDC hdc, RECT *rect, int offset_x, int offset_y)
{
	node  *current;
	HBITMAP  bitmap;
	HDC  temp_dc;
	SCROLLINFO  si;


	if (__text.length == 0)
	{
		HFONT  new_font, old_font;
		// 如果文本为空
		if (_text_changed)
		{
			// 如果文本原本非空，删除内存位图
			DeleteObject(_bitmap);
			EnableScrollBar(__paintboard, SB_HORZ, ESB_DISABLE_BOTH);
			EnableScrollBar(__paintboard, SB_VERT, ESB_DISABLE_BOTH);

		}
		// 打印"Hello!"
		new_font =  CreateFont(80, 40, 0, 0, FW_NORMAL, 1, 0, 0, 
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				"宋体");
		old_font = (HFONT)SelectObject(hdc, new_font);
		TextOut(hdc, (rect->right - 240) / 2, (rect->bottom - 80) / 2, (LPCSTR)"Hello!", 6);
		SelectObject(hdc, old_font);
		DeleteObject(new_font);
	}
	else
	{
		expression  exp = {0, 0, {NULL, NULL, 0}, 0};
		// 文本非空，绘制表达式
		if (_text_changed)
		{
			// 文本改变了，内存DC的图像需要重绘
			_text_changed = 0;
		
			// 重新建立表达式
			str_to_exp(__text.string, &exp);
		
			// 创建新的内存位图
			temp_dc = GetDC(__paintboard);
			bitmap = CreateCompatibleBitmap(temp_dc, exp.width, exp.height);
			ReleaseDC(__paintboard, temp_dc);
			SelectObject(_mdc, bitmap);
			DeleteObject(_bitmap);
			_bitmap = bitmap;
		
			// 用白色填充mdc
			_mdc_rect.left = 0;
			_mdc_rect.right = exp.width;
			_mdc_rect.top = 0;
			_mdc_rect.bottom = exp.height;
			FillRect(_mdc, &_mdc_rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

			// 在mdc中绘制表达式
			current = exp.prims.head;
			while (current != NULL)
			{
				draw_primitive(_mdc, &current->prim);
				current = current->next;
			}

			// 表达式绘制后就没用了，将其清空
			empty_exp(&exp);

			// 设置滚动水平滚动条的参数
			if (_mdc_rect.right > rect->right)
			{
				si.cbSize = sizeof(SCROLLINFO);
				si.nMax = _mdc_rect.right;
				si.nMin = 0;
				si.nPage = rect->right;
				si.nPos = offset_x;
				si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
				SetScrollInfo(__paintboard, SB_HORZ, &si, 1);
				EnableScrollBar(__paintboard, SB_HORZ, ESB_ENABLE_BOTH);
			}
			else
			{
				EnableScrollBar(__paintboard, SB_HORZ, ESB_DISABLE_BOTH);
			}

			// 设置滚动垂直滚动条的参数
			if (_mdc_rect.bottom > rect->bottom)
			{
				si.cbSize = sizeof(SCROLLINFO);
				si.nMax = _mdc_rect.bottom;
				si.nMin = 0;
				si.nPage = rect->bottom;
				si.nPos = offset_x;
				si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
				SetScrollInfo(__paintboard, SB_VERT, &si, 1);
				EnableScrollBar(__paintboard, SB_VERT, ESB_ENABLE_BOTH);
			}
			else
			{
				EnableScrollBar(__paintboard, SB_VERT, ESB_DISABLE_BOTH);
			}
		}

		// 从mdc向hdc贴图
		if (rect->right > _mdc_rect.right)
		{
			if (rect->bottom > _mdc_rect.bottom)
			{
				BitBlt(	hdc, (rect->right - _mdc_rect.right) / 2 , (rect->bottom - _mdc_rect.bottom) / 2,
						_mdc_rect.right, _mdc_rect.bottom, _mdc, 0, 0, SRCCOPY);
			}
			else
			{
				BitBlt(	hdc, (rect->right - _mdc_rect.right) / 2, 0,
						_mdc_rect.right, rect->bottom, _mdc, 0, offset_y, SRCCOPY);
			}
		}
		else
		{
			if (rect->bottom > _mdc_rect.bottom)
			{
				BitBlt(	hdc, 0, (rect->bottom - _mdc_rect.bottom) / 2, rect->right,
						_mdc_rect.bottom, _mdc, offset_x, 0, SRCCOPY);
			}
			else
			{
				BitBlt(hdc, 0, 0, rect->right, rect->bottom, _mdc, offset_x, offset_y, SRCCOPY);
			}
		}
	}
}


/***********************************************************
*
*函数：paintboard_proc
*功能：画板的窗口处理函数
*备注：
*返回：
*
***********************************************************/
static LRESULT CALLBACK paintboard_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT  result;	// 消息处理的结果
	PAINTSTRUCT  ps;
	HDC  hdc;
	RECT  rect;
	SCROLLINFO  si;
	static int  horz_pos = 0;			// 水平滚动条位置
	static int  vert_pos = 0;			// 垂直滚动条位置
	static int  cursor_pre_pos_x = 0;	// 鼠标按下时鼠标的x坐标
	static int  cursor_pre_pos_y = 0;	// 鼠标按下时鼠标的y坐标
	static int  horz_pre_pos = 0;		// 鼠标按下时水平滚动条位置
	static int  vert_pre_pos = 0;		// 鼠标按下时垂直滚动条位置
	static int  is_lbutton_down = 0;	// 鼠标左键状态
	POINT  point;

	switch (message)
	{
		case WM_CREATE :
			// 创建内存DC
			hdc = GetDC(hwnd);
			_mdc = CreateCompatibleDC(hdc);
			ReleaseDC(hwnd, hdc);
			
			// 获取客户区矩形
			GetClientRect(hwnd, &rect);

			// 设置滚动条参数
			SetScrollRange(hwnd, SB_HORZ, 0, rect.right, 1);
			SetScrollRange(hwnd, SB_VERT, 0, rect.bottom, 1);
			EnableScrollBar(hwnd, SB_BOTH, ESB_DISABLE_BOTH);
			result = 0;
			break;

		case WM_PAINT :	// 窗口重绘
			hdc = BeginPaint(hwnd, &ps);

			GetClientRect(hwnd, &rect);
			if (__text.length == 0)
			{
				horz_pos = 0;
				vert_pos = 0;
			}
			// 绘制表达式
			draw_exp(hdc, &rect, horz_pos, vert_pos);
			
			EndPaint(hwnd, &ps);
			result = 0;
			break;

		case WM_TEXT_CHANGE :
			// 文本框的文字改变
			_text_changed = 1;
			GetClientRect(hwnd, &rect);
			InvalidateRect(hwnd, &rect, TRUE);
			UpdateWindow(hwnd);
			result = 0;
			break;

		case WM_HSCROLL :
			// 水平滚动条事件
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_ALL;
			GetScrollInfo(__paintboard, SB_HORZ, &si);
			switch (LOWORD(wParam))
			{
				case SB_LINELEFT :
					si.nPos--;
					break;
				case SB_LINERIGHT :
					si.nPos++;
					break;
				case SB_PAGELEFT :
					si.nPos -= si.nPage;
					break;
				case SB_PAGERIGHT :
					si.nPos += si.nPage;
					break;
				case SB_THUMBTRACK :
					si.nPos = si.nTrackPos;
					break;
			}
			si.fMask = SIF_POS;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
			GetScrollInfo(hwnd, SB_HORZ, &si);
			if (si.nPos != horz_pos)
			{
				horz_pos = si.nPos;
				GetClientRect(hwnd, &rect);
				InvalidateRect(hwnd, &rect, TRUE);
				UpdateWindow(hwnd);
			}
			result = 0;
			break;

		case WM_VSCROLL :
			// 垂直滚动条事件
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_ALL;
			GetScrollInfo(__paintboard, SB_VERT, &si);
			switch (LOWORD(wParam))
			{
				case SB_LINEUP :
					si.nPos--;
					break;
				case SB_LINEDOWN :
					si.nPos++;
					break;
				case SB_PAGEUP :
					si.nPos -= si.nPage;
					break;
				case SB_PAGEDOWN :
					si.nPos += si.nPage;
					break;
				case SB_THUMBTRACK :
					si.nPos = si.nTrackPos;
					break;
			}
			si.fMask = SIF_POS;
			SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
			GetScrollInfo(hwnd, SB_VERT, &si);
			if (si.nPos != vert_pos)
			{
				vert_pos = si.nPos;
				GetClientRect(hwnd, &rect);
				InvalidateRect(hwnd, &rect, TRUE);
				UpdateWindow(hwnd);
			}
			result = 0;
			break;

		case WM_LBUTTONDOWN :
			// 鼠标左键按下
			SetCapture(hwnd);	// 捕获鼠标输入
			GetCursorPos(&point);
			cursor_pre_pos_x = point.x;
			cursor_pre_pos_y = point.y;
			horz_pre_pos = horz_pos;
			vert_pre_pos = vert_pos;
			is_lbutton_down = 1;
			result = 0;
			break;

		case WM_LBUTTONUP :
			// 鼠标左键抬起
			is_lbutton_down = 0;
			ReleaseCapture();	// 释放鼠标输入
			result = 0;
			break;

		case WM_MOUSEMOVE :
			if (is_lbutton_down)
			{
				// 如果鼠标左键按下
				si.cbSize = sizeof(SCROLLINFO);
				si.fMask = SIF_POS;
				
				GetCursorPos(&point);
				// 设置水平滚动条位置
				si.nPos = cursor_pre_pos_x - point.x + horz_pre_pos;
				SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
				SendMessage(hwnd, WM_HSCROLL, (WPARAM)SB_THUMBTRACK, 0);

				// 设置垂直滚动条位置
				si.nPos = cursor_pre_pos_y - point.y + vert_pre_pos;
				SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
				SendMessage(hwnd, WM_VSCROLL, (WPARAM)SB_THUMBTRACK, 0);
			}
			result = 0;
			break;

		case WM_DESTROY :
			DeleteObject(_mdc);
			DeleteObject(_bitmap);
			result = 0;
			break;

		default :
			result = DefWindowProc(hwnd, message, wParam, lParam);
			break;
	}
	return result;
}

// 初始化画板
int init_paintboard(void)
{
	WNDCLASSEX wc;									// 窗口类

	// 初始化窗口类
	wc.cbSize = sizeof(wc);							// 结构的大小
	wc.style = CS_HREDRAW | CS_VREDRAW;				// 窗口风格
	wc.lpfnWndProc = paintboard_proc;				// 窗口函数指针
	wc.cbClsExtra = 0;								// 额外的类内存
	wc.cbWndExtra = 0;								// 额外的窗口内存
	wc.hInstance = __instance;						// 实例句柄
	wc.hIcon = NULL;								// 窗口图标
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);		// 窗口光标
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// 背景刷子
	wc.lpszClassName = "mypaintboard";				// 窗口类名
	wc.lpszMenuName = NULL;							// 菜单
	wc.hIconSm = NULL;								// 小图标
	
	// 注册窗口类
	RegisterClassEx(&wc);

	// 创建文本框
	__paintboard = CreateWindowEx(
			0,							// 扩展样式
			"mypaintboard",				// 类名
			NULL,						// 窗口标题
			// 窗口风格
			  WS_CHILD				// 子窗体
			| WS_VISIBLE			// 可见
			| WS_HSCROLL			// 水平滚动条
			| WS_VSCROLL			// 竖直滚动条
			,
			40,							// 初始 X 坐标
			20,							// 初始 Y 坐标
			320,						// 宽度
			160,						// 高度
			__wnd_main,					// 父窗口句柄
			NULL,						// 菜单句柄
			__instance,					// 程序实例句柄
			NULL);						// 用户数据

	if ( __paintboard == NULL )
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