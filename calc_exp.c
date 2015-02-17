/*********************************************************************
*
*文件：calc_exp.c
*功能：计算表达式的值
*备注：
*
*********************************************************************/

#include <math.h>
#include "global.h"
#include "hpfloat.h"
#include "math_fun.h"

/**********************常数************************/
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
#define OP_POW2			((0<<16) + (7<<8) + 11)		// 平方
#define OP_POW3			((0<<16) + (7<<8) + 12)		// 立方
#define OP_POW			((1<<16) + (5<<8) + 11)		// 乘方

#define OP_L_BARCKET	((2<<8) +  3)		// 左括号
#define OP_R_BARCKET	((1<<8) +  4)		// 右括号

#define OP_NUM			-1				// 不是运算符，只是用来保持一致
#define OP_CONST		-2				// 不是运算符，只是用来保持一致


/************************类型**********************/
typedef struct stack_op
{
	int  top;							// 栈顶索引
	int  ops[STACK_LENGTH];				// 元素
} stack_op;

typedef struct stack_num
{
	int  top;							// 栈顶索引
	hpfloat  *nums[STACK_LENGTH];		// 元素
} stack_num;

/***********************************全局变量*******************************/


/*****************************函数*******************************/

/***********************************************************
*
*函数：free_stack
*功能：释放操作数栈
*备注：
*返回：
*
***********************************************************/
static void free_stack(stack_num *stack)
{
	int i;

	for (i = 0; i < STACK_LENGTH; i++)
	{
		if (stack->nums[i] != NULL)
		{
			free(stack->nums[i]);
		}
	}
}


/***********************************************************
*
*函数：is_syntax_error
*功能：检查一个字符串形式的表达式是否有语法错误
*备注：
*返回：
*       0--无语法错误
*      -1--出错
*
***********************************************************/
static int is_syntax_error(const char *str)
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
		return -1;
	}
	else
	{
		return 0;
	}
}


/***********************************************************
*
*函数：calc_exp()
*功能：计算表达式的值
*返回：0--未出错
*      1--语法错误
*      2--计算时出错
*      表达式的值存储在*result中
***********************************************************/
int calc_exp(const char *str, hpfloat *result)
{
	stack_op   s_op;			// 运算符栈
	stack_num  s_num;			// 操作数栈
	
	int  end = 0;				// 0--表达式还未结束，1--表达式结束
	int  dot_met;				// 0--未遇见小数点 1--遇到小数点
	int  cur_op;				// 当前运算符
	hpfloat  pow;
	hpfloat temp;

	if (is_syntax_error(str))
	{
		return 1;
	}
	
	// 将两个栈置为空栈
	memset(&s_op, 0, sizeof(s_op));
	memset(&s_num, 0, sizeof(s_num));
	s_op.top = -1;
	s_num.top = -1;
	
	// 向运算符栈中压入开始运算符
	s_op.top++;
	s_op.ops[s_op.top] = OP_BEGIN;
	
	cur_op = OP_BEGIN;

	for (; !end; str++)
	{
		if ((*str == '.') || ((*str >= '0') && (*str <= '9'))) 
		{
			if (cur_op != OP_NUM)
			{	
				// 上一个字符是运算符，压入操作数栈
				s_num.top++;
				if (s_num.nums[s_num.top] == NULL)
				{
					s_num.nums[s_num.top] = (hpfloat *)malloc(sizeof(hpfloat));
					if (s_num.nums[s_num.top] == NULL)
					{
						free_stack(&s_num);
						result->error = MEM_ERROR;
						return 2;
					}
				}
				if (*str == '.')
				{
					dot_met = 1;
					hp_int_to_hp(&pow, 1);
					hp_zero(s_num.nums[s_num.top]);
				}
				else
				{
					dot_met = 0;
					hp_int_to_hp(s_num.nums[s_num.top], (*str - '0'));
				}
				cur_op = OP_NUM;
			}
			else
			{
				// 上一个字符也是数字，修改操作数栈的栈顶元素
				if (*str == '.')
				{
					dot_met = 1;
					hp_int_to_hp(&pow, 1);
				}
				else
				{
					if (dot_met == 1)
					{
						hp_div_int(&pow, 10);
						hp_int_to_hp(&temp, (*str - '0'));
						hp_mul(&temp, &pow, &temp);
						hp_add(s_num.nums[s_num.top], &temp, s_num.nums[s_num.top]);
					}
					else
					{
						hp_mul_int(s_num.nums[s_num.top], 10);
						hp_add_int(s_num.nums[s_num.top], (*str - '0'));
					}
				}
			}
		}
		else if (*str == CHAR_PI)
		{
			s_num.top++;
			if (s_num.nums[s_num.top] == NULL)
			{
				s_num.nums[s_num.top] = (hpfloat *)malloc(sizeof(hpfloat));
				if (s_num.nums[s_num.top] == NULL)
				{
					free_stack(&s_num);
					result->error = MEM_ERROR;
					return 2;
				}
			}
			*s_num.nums[s_num.top] = *hp_pi();
			cur_op = OP_CONST;
		}
		else if (*str == 'e')
		{
			// 是常量，当成数字处理
			// 压入表达式栈
			s_num.top++;
			if (s_num.nums[s_num.top] == NULL)
			{
				s_num.nums[s_num.top] = (hpfloat *)malloc(sizeof(hpfloat));
				if (s_num.nums[s_num.top] == NULL)
				{
					free_stack(&s_num);
					result->error = MEM_ERROR;
					return 2;
				}
			}
			*s_num.nums[s_num.top] = *hp_e();
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

					switch (s_op.ops[s_op.top])
					{
						case OP_ADD :
							hp_add(s_num.nums[s_num.top - 1], s_num.nums[s_num.top], s_num.nums[s_num.top - 1]);
							s_op.top--;
							s_num.top--;
							break;

						case OP_SUB :
							hp_sub(s_num.nums[s_num.top - 1], s_num.nums[s_num.top], s_num.nums[s_num.top - 1]);
							s_op.top--;
							s_num.top--;
							break;

						case OP_MUL :
							hp_mul(s_num.nums[s_num.top - 1], s_num.nums[s_num.top], s_num.nums[s_num.top - 1]);
							s_op.top--;
							s_num.top--;
							break;

						case OP_DIV :
							hp_div(s_num.nums[s_num.top - 1], s_num.nums[s_num.top], s_num.nums[s_num.top - 1]);
							s_op.top--;
							s_num.top--;
							break;

						case OP_POW :
							hp_pow(s_num.nums[s_num.top - 1], s_num.nums[s_num.top], s_num.nums[s_num.top - 1]);
							s_op.top--;
							s_num.top--;
							break;

						case OP_SIN :
							hp_sin(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_COS :
							hp_cos(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_TAN :
							hp_tan(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_SINH :
							hp_sinh(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_COSH :
							hp_cosh(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_TANH :
							hp_tanh(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_LN :
							hp_ln(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_LG :
							hp_lg(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_POW2 :
							hp_mul(s_num.nums[s_num.top], s_num.nums[s_num.top], s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_POW3 :
							temp = *s_num.nums[s_num.top];
							hp_mul(s_num.nums[s_num.top], &temp, s_num.nums[s_num.top]);
							hp_mul(s_num.nums[s_num.top], &temp, s_num.nums[s_num.top]);
							s_op.top--;
							break;
							
						case OP_SQRT :
							hp_sqrt(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_SQRT3 :
							hp_sqrt3(s_num.nums[s_num.top]);
							s_op.top--;
							break;

						case OP_NEG :
							s_num.nums[s_num.top]->sign ^= -1;
							s_op.top--;

					}
					if (s_num.nums[s_num.top]->error != NO_ERROR)
					{
						result->error = s_num.nums[s_num.top]->error;
						free_stack(&s_num);
						return 2;
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
	*result = *s_num.nums[s_num.top];
	free_stack(&s_num);
	return 0;
}