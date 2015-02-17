/*********************************************************************
*
*文件：hpfloat.c
*功能：实现高精度浮点仿真
*备注：
*
*********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**********************************常数*******************************/
#define HP_LENGTH			100
#define HP_POWER_LIMIT		100

#define NO_ERROR			0	// 没有错误
#define OVER_FLOW			1	// 溢出
#define DIVIDE_ZERO			2	// 除0错误
#define FUN_ERROR			3	// 函数调用错误
#define MEM_ERROR			4	// 内存分配错误

/**********************************类型*******************************/
// 高精度浮点数类型
typedef struct hpfloat
{
	int bits[HP_LENGTH];	// 底数的各个数位
	int sign;				// 符号位 0 --非负  -1 --负数
	int power;				// 指数
	int error;				// 错误位 0 --无错
} hpfloat;


/***********************************全局变量*************************/

/***********************************函数********************************/
/*****************************************************************
*函数：hp_zero()
*功能：将高精度数清零
******************************************************************/
void hp_zero(hpfloat *result)
{
	memset(result, 0, sizeof(hpfloat));
}


/****************************************************************
*函数：hp_int_to_hp()
*功能：把int转换为高精度浮点数
******************************************************************/
hpfloat *hp_int_to_hp(hpfloat *hp_num, int num)
{
	memset(hp_num, 0, sizeof(hpfloat));
	if (num < 0)
	{
		num = -num;
		hp_num->sign = -1;
	}
	if (num == 0)
	{
		
	}
	else if (num < 10000)
	{
		hp_num->bits[0] = num;
		hp_num->power = 1;
	}
	else if (num < 10000 * 10000)
	{
		hp_num->bits[1] = num % 10000;
		hp_num->bits[0] = num / 10000;
		hp_num->power = 2;
	}
	else
	{
		hp_num->bits[2] = num % 10000;
		num /= 10000;
		hp_num->bits[1] = num % 10000;
		hp_num->bits[0] = num / 10000;
		hp_num->power = 3;
	}
	return hp_num;
}


/****************************************************************
*函数：hp_cmp()
*功能：比较两个高精度数的大小
******************************************************************/
int hp_cmp(const hpfloat *num1, const hpfloat *num2)
{
	int i;

	if (num1->sign ^ num2->sign)
	{
		// 两个数的符号不同
		if (num1->sign == -1)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		// 两个数的符号相同
		if (num1->power > num2->power)
		{
			if (num1->sign == -1)
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
		else if (num1->power < num2->power)
		{
			if (num1->sign == -1)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			for (i = 0; (i < HP_LENGTH) && (num1->bits[i] == num2->bits[i]); i++)
			{
			}
			if (i == HP_LENGTH)
			{
				return 0;
			}
			else
			{
				if (num1->bits[i] > num2->bits[i])
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
		}
	}
}



/****************************************************************
*函数：hp_abs_cmp()
*功能：比较两个高精度数的绝对值的大小
******************************************************************/
int hp_abs_cmp(const hpfloat *num1, const hpfloat *num2)
{
	int i;

	if (num1->bits[0] == 0)
	{
		if (num2->bits[0] == 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else if (num2->bits[0] == 0)
	{
		return 1;
	}
	else
	{
		if (num1->power > num2->power)
		{
			return 1;
		}
		else if (num1->power < num2->power)
		{
			return -1;
		}
		else
		{
			for (i = 0; (i < HP_LENGTH) && (num1->bits[i] == num2->bits[i]); i++)
			{
			}
			if (i == HP_LENGTH)
			{
				return 0;
			}
			else
			{
				if (num1->bits[i] > num2->bits[i])
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
		}
	}
}



/****************************************************************
*函数：s_abs_add()
*功能：绝对值相加
******************************************************************/
static void s_abs_add(const hpfloat *num1, const hpfloat *num2, hpfloat *result)
{
	int carry;		// 进位标志
	int power_diff;
	const hpfloat *temp;
	int i;

	if (num1->power < num2->power)
	{
		temp = num1;
		num1 = num2;
		num2 = temp;
	}
	power_diff = num1->power - num2->power;
	result->power = num1->power;
	if (num1->bits[0] == 0)
	{
		// num1 等于 0，resutl = num2
		*result = *num2;
	}
	else
	{
		for (carry = 0, i = HP_LENGTH - 1; i >= power_diff; i--)
		{
			result->bits[i] = num1->bits[i] + num2->bits[i - power_diff] + carry;
			if (result->bits[i] >= 10000)
			{
				result->bits[i] -= 10000;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
		}
		if (power_diff > 0)
		{
			for (; i >= 0; i--)
			{
				result->bits[i] = num1->bits[i] + carry;
				if (result->bits[i] >= 10000)
				{
					result->bits[i] -= 10000;
					carry = 1;
				}
				else
				{
					carry = 0;
				}
			}
		}
		if(carry != 0)
		{
			//将数组整体向后移一位
			memmove(result->bits + 1, result->bits, sizeof(int) * (HP_LENGTH - 1));
			result->bits[0] = 1;
			result->power++;
		}
	}
}


/***********************************************************************
*函数：s_abs_sub()
*功能：绝对值相减（要求minuend的绝对值比subtrahend大）
**********************************************************************/
static void s_abs_sub(const hpfloat *minuend, const hpfloat *subtrahend, hpfloat *result)
{
	int i;
	int power_diff;
	int carry = 0;		// 借位标志
	power_diff = minuend->power - subtrahend->power;

	result->power = minuend->power;

	for (carry = 0, i = HP_LENGTH - 1; i >= power_diff; i--)
	{
		result->bits[i] = minuend->bits[i] - subtrahend->bits[i - power_diff] - carry;
		if (result->bits[i]  < 0)
		{
			result->bits[i] += 10000;
			carry = 1;
		}
		else
		{
			carry = 0;
		}
	}
	if (power_diff > 0)
	{
		for (; i >= 0; i--)
		{
			result->bits[i] = minuend->bits[i] - carry;
			if (result->bits[i] < 0)
			{
				result->bits[i] += 10000;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
		}
	}
	for (i = 0; i < HP_LENGTH; i++)
	{
		if (result->bits[i] != 0)
		{
			break;
		}
	}
	if (i != 0)
	{
		//将数组整体向前移i位
		memmove(result->bits, result->bits + i, sizeof(int) * (HP_LENGTH - i));
		memset(result->bits + HP_LENGTH - i, 0, sizeof(int) * i);
		result->power -= i;
	}
}


/***********************************************************************
*函数：hp_add()
*功能：高精度加法
**********************************************************************/
hpfloat *hp_add(const hpfloat *num1, const hpfloat *num2, hpfloat *result)
{
	if (num1->error || num2->error)
	{
		// 如果输入参数有错，直接返回
		result->error = num1->error | num2->error;
		return result;
	}

	if (num1->bits[0] == 0)
	{
		// 如果num1 == 0, result = num2
		*result = *num2;
		return result;
	}
	if (num2->bits[0] == 0)
	{
		// 如果num2 == 0, result = num1
		*result = *num1;
		return result;
	}
	if (num1->sign == num2->sign)
	{
		hpfloat temp_result;
		memset(&temp_result, 0, sizeof(hpfloat));
		// 两个数的符号相同，做加法
		s_abs_add(num1, num2, &temp_result);
		if (temp_result.power > HP_POWER_LIMIT)
		{
			// 溢出
			memset(result, 0, sizeof(hpfloat));
			result->error = OVER_FLOW;
			return result;
		}
		else
		{
			// 未溢出
			int sign = num1->sign;
			*result = temp_result;
			result->sign = sign;
			result->error = NO_ERROR;
			return result;
		}
	}
	else
	{
		// 两个数的符号不相同，做减法
		int cmp;
		hpfloat temp_result;
		memset(&temp_result, 0, sizeof(hpfloat));
		cmp = hp_abs_cmp(num1, num2);
		if (cmp > 0)
		{
			memset(&temp_result, 0, sizeof(hpfloat));
			s_abs_sub(num1, num2, &temp_result);
			if (result->power < -HP_POWER_LIMIT)
			{
				// 下溢，设置为0
				memset(result, 0, sizeof(hpfloat));
				return result;
			}
			else
			{
				int sign = num1->sign;
				*result = temp_result;
				result->sign = sign;
				result->error = NO_ERROR;
				return result;
			}
		}
		else if (cmp < 0)
		{
			memset(&temp_result, 0, sizeof(hpfloat));
			s_abs_sub(num2, num1, &temp_result);
			if (result->power < -HP_POWER_LIMIT)
			{
				// 下溢，设置为0
				memset(result, 0, sizeof(hpfloat));
				return result;
			}
			else
			{
				int sign = num2->sign;
				*result = temp_result;
				result->sign = sign;
				result->error = NO_ERROR;
				return result;
			}
		}
		else
		{
			// 两个数相等，结果为0
			memset(result, 0, sizeof(hpfloat));
			return result;
		}
	}
}



/***********************************************************************
*函数：hp_sub()
*功能：高精度减法
**********************************************************************/
hpfloat *hp_sub(const hpfloat *num1, const hpfloat *num2, hpfloat *result)
{
	if (num1->error || num2->error)
	{
		// 如果输入参数有错，直接返回
		result->error = num1->error | num2->error;
		return result;
	}

	if (num2->bits[0] == 0)
	{
		// 如果num2 == 0, result = num1
		*result = *num1;
		return result;
	}
	if (num1->bits[0] == 0)
	{
		// num1 == 0, num2 != 0, result = -num2;
		*result = *num2;
		result->sign ^= -1;
		return result;
	}
	if (num1->sign != num2->sign)
	{
		hpfloat temp_result;
		memset(&temp_result, 0, sizeof(hpfloat));
		// 两个数的符号不相同，做加法
		s_abs_add(num1, num2, &temp_result);
		if (temp_result.power > HP_POWER_LIMIT)
		{
			// 溢出
			memset(result, 0, sizeof(hpfloat));
			result->error = OVER_FLOW;
			return result;
		}
		else
		{
			// 未溢出
			int sign = num1->sign;
			*result = temp_result;
			result->sign = sign;
			result->error = NO_ERROR;
			return result;
		}
	}
	else
	{
		// 两个数的符号相同，做减法
		int cmp;

		hpfloat temp_result;
		memset(&temp_result, 0, sizeof(hpfloat));
		cmp =hp_abs_cmp(num1, num2);
		if (cmp > 0)
		{
			s_abs_sub(num1, num2, &temp_result);
			if (temp_result.power < -HP_POWER_LIMIT)
			{
				// 下溢，设置为0
				memset(result, 0, sizeof(hpfloat));
				return result;
			}
			else
			{
				int sign = num1->sign;
				*result = temp_result;
				result->sign = sign;
				result->error = NO_ERROR;
				return result;
			}
		}
		else if (cmp < 0)
		{
			s_abs_sub(num2, num1, &temp_result);
			if (temp_result.power < -HP_POWER_LIMIT)
			{
				// 下溢，设置为0
				memset(result, 0, sizeof(hpfloat));
				return result;
			}
			else
			{
				int sign = num1->sign ^ -1;
				*result = temp_result;
				result->sign = sign;
				result->error = NO_ERROR;
				return result;
			}
		}
		else
		{
			// 两个数相等，结果为0
			memset(result, 0, sizeof(hpfloat));
			return result;
		}
	}
}


/******************************************************************
*函数：hp_mul()
*功能：实现乘法运算
*****************************************************************/
hpfloat *hp_mul(const hpfloat *multiplicand, const hpfloat *multiplier, hpfloat *result)
{
	int  temp_result[2 * HP_LENGTH] = {0};		// 用于暂时存放结果
	int  carry;
	int  i, j;	

	if (multiplicand->error || multiplier->error)
	{
		// 如果输入参数有错，直接返回
		result->error = multiplicand->error | multiplier->error;
		return result;
	}

	if ((multiplicand->bits[0] == 0) || (multiplier->bits[0] == 0))
	{
		// 被乘数和乘数任意一个为0，结果为0
		memset(result, 0, sizeof(hpfloat));
		return result;
	}
	
	// 计算相乘后的和，作为临时值存放于temp_result中
	for (i = HP_LENGTH - 1; i >= 0 ; i--)		
	{
		for (j = HP_LENGTH - 1; j >= 0; j--)
		{
			temp_result[i+j+1] += (multiplicand->bits[j] * multiplier->bits[i]);
		}
		for (carry = 0, j = 2 * HP_LENGTH - 1; j >= 0; j--)
		{
			temp_result[j] += carry;
			carry = temp_result[j] / 10000;
			temp_result[j] %= 10000;
		}
	}

	result->power = multiplicand->power + multiplier->power;
	if (temp_result[0] == 0)
	{
		result->power--;
	}
	if (result->power > HP_POWER_LIMIT)
	{
		// 上溢
		result->error = OVER_FLOW;
		return result;
	}
	else if (result->power < -HP_POWER_LIMIT)
	{
		// 下溢
		memset(result, 0, sizeof(hpfloat));
		return result;
	}
	else
	{
		if (temp_result[0] == 0)
		{
			memmove(result->bits, temp_result + 1, sizeof(int) * HP_LENGTH);
		}
		else
		{
			memmove(result->bits, temp_result, sizeof(int) * HP_LENGTH);
		}
		// 设置符号位
		result->sign = multiplicand->sign ^ multiplier->sign;
		result->error = NO_ERROR;
		return result;
	}
}



/****************************************************************
****函数：hp_div()
功能：实现除法运算
**************************************************************/
hpfloat *hp_div(const hpfloat *divider, const hpfloat *divisor, hpfloat *result)
{
	int search;						//定义search，采用二分法寻找商
	int temp_result[HP_LENGTH + 1];
	int carry;
	int up_range;						//试探法的上边界
	int down_range;						//试探法的下边界
	int temp_divider[HP_LENGTH + 1];	//被除数的副本
	int i, j;

	if (divider->error || divisor->error)
	{
		// 如果输入参数有错，直接返回
		result->error = divider->error | divisor->error;
		return result;
	}

	if (divisor->bits[0] == 0)
	{
		// 除数为0，溢出
		result->error = DIVIDE_ZERO;
		return result;
	}
	if (divider->bits[0] == 0)
	{
		// 被除数为0，结果为0
		memset(result, 0, sizeof(hpfloat));
		return result;
	}

	// 创建被除数的副本
	memmove(temp_divider + 1, divider->bits, sizeof(int) * HP_LENGTH);
	temp_divider[0] = 0;
		
	for (i = 0; i < HP_LENGTH; i++)
	{
		up_range = 10000;
		down_range = 0;
		for (;;)
		{
			search = (up_range + down_range) / 2;
				
			// 计算search * divisor,结果放在temp_result中
			for (carry = 0, j = HP_LENGTH; j >= 1; j--)
			{
				temp_result[j] = divisor->bits[j - 1] * search + carry;
				carry = temp_result[j] /10000;
				temp_result[j] %= 10000;
			}
			temp_result[0] = carry;
				
			// 比较temp_result和temp_divider的大小
			for (j = 0; j <= HP_LENGTH; j++)
			{
				if (temp_divider[j] != temp_result[j])
				{
					break;
				}
			}
			if (j == HP_LENGTH + 1)
			{
				// temp_result和temp_divider相等
				break;
			}
			else if(temp_result[j] > temp_divider[j])
			{
				// temp_result比temp_divider大，search偏大
				up_range = search;
			}
			else
			{
				down_range = search;
			}
			if ((search == down_range) && (up_range - down_range <= 1))
			{
				break;
			}
		}
			
		// 把结果放在result中
		result->bits[i] = search;

		// temp_divider -= temp_reault;
		for (carry = 0, j = HP_LENGTH; j >= 0; j--)
		{
			temp_divider[j] = temp_divider[j] - temp_result[j] - carry;
			if (temp_divider[j] < 0)
			{
				temp_divider[j] += 10000;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
		}
			
		// temp_divider *= 10000;
		memmove(temp_divider, temp_divider + 1, sizeof(int) * HP_LENGTH);
		temp_divider[HP_LENGTH] = 0;
	}

	result->power = divider->power - divisor->power;
	if (result->bits[0] == 0)
	{
		memmove(result->bits, result->bits + 1, sizeof(int) * (HP_LENGTH - 1));
		result->bits[HP_LENGTH - 1] = 0;
	}
	else
	{
		result->power++;
	}
	result->sign = divider->sign ^ divisor->sign;
	result->error = NO_ERROR;
	return result;

}



/******************************************************************
*函数：hp_add_int()
*功能：实现高精度数和int的加法运算
*****************************************************************/
hpfloat *hp_add_int(hpfloat *hp_num, int num)
{
	hpfloat temp;

	hp_int_to_hp(&temp, num);
	hp_add(hp_num, &temp, hp_num);
	return hp_num;
}

/******************************************************************
****函数：hp_sub_int()
****功能：实现高精度数和int的加法运算
*****************************************************************/
hpfloat *hp_sub_int(hpfloat *hp_num, int num)
{
	hpfloat temp;

	hp_int_to_hp(&temp, num);
	hp_sub(hp_num, &temp, hp_num);
	return hp_num;
}


/******************************************************************
****函数：hp_mul_int()
****功能：实现高精度数和int的乘法运算
*****************************************************************/
hpfloat *hp_mul_int(hpfloat *hp_num, int num)
{
	hpfloat temp;

	hp_int_to_hp(&temp, num);
	hp_mul(hp_num, &temp, hp_num);
	return hp_num;
}


/****************************************************************
****函数：hp_div_int()
功能：实现高精度除以int运算
**************************************************************/
hpfloat *hp_div_int(hpfloat *hp_num, int num)
{
	hpfloat temp;

	hp_int_to_hp(&temp, num);
	hp_div(hp_num, &temp, hp_num);
	return hp_num;
}



/****************************************************************
*函数：s_calc_pi()
*功能：计算pi(圆周率)
**************************************************************/
static void s_calc_pi(hpfloat *hp_num)
{
	int temp[2 * HP_LENGTH + 1000 + 1];
	int i, j;

	memset(temp, 0, sizeof(int) * (HP_LENGTH + 40 + 1));

	temp[0] = 2;
	for(i = 10 * HP_LENGTH + 2000; i > 0; i--)
	{
		for(j = HP_LENGTH + 1000; j >= 0; j--)
		{
			temp[j] *= i;
		}
		for(j = HP_LENGTH + 1000; j > 0; j--)
		{
			temp[j - 1] += temp[j] / 10000;
			temp[j] %= 10000;
		}
		for(j = 0;j < HP_LENGTH + 1000; j++)
		{
			temp[j + 1] += temp[j] % (i * 2 + 1) * 10000;
			temp[j] /= (i * 2 + 1);
		}
		temp[0] += 2;
	}

	memmove(hp_num->bits, temp, sizeof(int) * HP_LENGTH);
	hp_num->error = NO_ERROR;
	hp_num->power = 1;
	hp_num->sign = 0;
}



/****************************************************************
*函数：s_calc_e()
*功能：计算e(自然指数的底)
**************************************************************/
static void s_calc_e(hpfloat *hp_num)
{
	int temp[2 * HP_LENGTH + 1000 + 1];
	int i, j;

	memset(temp, 0, sizeof(int) * (HP_LENGTH + 40 + 1));
	
	temp[0]=1;
	for(i = 10 * HP_LENGTH + 2000; i >= 2; i--)
	{
		for(j = 0; j < 2 * HP_LENGTH + 1000; j++)
		{
			temp[j + 1] += temp[j] % i * 10000;
			temp[j] /= i;
		}
		temp[0] += 1;
	}
	temp[0] +=1;

	memmove(hp_num->bits, temp, sizeof(int) * HP_LENGTH);
	hp_num->error = NO_ERROR;
	hp_num->power = 1;
	hp_num->sign = 0;
}


/****************************************************************
*函数：hp_pi()
*功能：得到值为pi(圆周率)的高精度浮点数
**************************************************************/
const hpfloat *hp_pi(void)
{
	static hpfloat pi = {{0}, 0, 0, 0};

	// pi 会在该函数第一次调用时初始化
	if (pi.bits[0] == 0)
	{
		// pi未初始化
		s_calc_pi(&pi);
	}
	return &pi;
}




/*************************************************************
*函数：hp_e()
*功能：得到值为e(自然指数的底)的高精度浮点数
**************************************************************/
const hpfloat *hp_e(void)
{
	static hpfloat e = {{0}, 0, 0, 0};

	// e 会在该函数第一次调用时初始化
	if (e.bits[0] == 0)
	{
		// pi未初始化
		s_calc_e(&e);
	}
	return &e;
}


/****************************************************************
*函数：s_int_to_str
*功能：把整数数转化为字符串
**************************************************************/
static int s_int_to_str(char *str, int num)
{
	char *temp = str;
	char buf[32];
	int i;
	if (num < 0)
	{
		num = -num;
		*str++ = '-';
	}

	*str = '0';
	i = 0;
	do
	{
		buf[i++] = num % 10 + '0';
		num /= 10;
	}while (num > 0);
	for (i--; i >= 0; i--)
	{
		*str++ = buf[i];
	}
	*str++ = '\0';
	return (int)(str - temp - 1);
}


/****************************************************************
*函数：hp_to_str
*功能：把高精度数转化为字符串
**************************************************************/
char *hp_to_str(const hpfloat *hp_num, char *str)
{
	if (hp_num->error == OVER_FLOW)
	{
		memmove(str, "结果溢出", 9);
		return str;
	}
	else if (hp_num->error == DIVIDE_ZERO)
	{
		memmove(str, "除0错误", 8);
	}
	else if (hp_num->error == FUN_ERROR)
	{
		memmove(str, "函数定义域错误！", 17);
	}
	else
	{
		char *temp = str;
		int power;
		int count;	// hp_num->bits中的前count个数字应该显示
		int i;

		if (hp_num->bits[0] == 0)
		{
			// hp_num等于0
			*temp++ = '0';
			*temp++ = '\0';
			return str;
		}
		else 
		{
			// hp_num不等于0
			hpfloat temp_num = *hp_num;
			if (temp_num.power <= -90)
			{
				memset(&temp_num, 0, sizeof(hpfloat));
			}
			// 对最后10位舍入
			if (temp_num.bits[HP_LENGTH - 10] >= 5000)
			{
				int carry;
				for( i = HP_LENGTH - 11, carry = 1; i >= 0; i--)
				{
					temp_num.bits[i] += carry;
					if (temp_num.bits[i] >= 10000)
					{
						temp_num.bits[i] -= 10000;
						carry = 1;
					}
					else
					{
						carry = 0;
					}
				}
				if (carry == 1)
				{
					memmove(temp_num.bits + 1, temp_num.bits, sizeof(int) * (HP_LENGTH - 1));
					temp_num.bits[0] = 1;
					temp_num.power++;
				}
			}

			if (temp_num.sign != 0)
			{
				*temp++ = '-';
			}
			for (count = HP_LENGTH - 11; count >= 1; count--)
			{
				if (temp_num.bits[count - 1] != 0)
				{
					break;
				}
			}
			if ((temp_num.power > 20) || (temp_num.power < -20))
			{
				// 当数字的指数太大或太小时，使用科学计数法
				power = temp_num.power * 4;
				if (temp_num.bits[0] >= 1000)
				{
					power--;
					*temp++ = temp_num.bits[0] / 1000 + '0';
					*temp++ = '.';
					*temp++ = temp_num.bits[0] / 100 % 10 + '0';
					*temp++ = temp_num.bits[0] / 10 % 10 + '0';
					*temp++ = temp_num.bits[0] % 10 + '0';
				}
				else if (temp_num.bits[0] >= 100)
				{
					power -= 2;
					*temp++ = temp_num.bits[0] / 100 + '0';
					*temp++ = '.';
					*temp++ = temp_num.bits[0] / 10 % 10 + '0';
					*temp++ = temp_num.bits[0] % 10 + '0';
				}
				else if (temp_num.bits[0] >= 10)
				{
					power -= 3;
					*temp++ = temp_num.bits[0] / 10 + '0';
					*temp++ = '.';
					*temp++ = temp_num.bits[0] % 10 + '0';
				}
				else
				{
					power -= 4;
					*temp++ = temp_num.bits[0] + '0';
					if (count > 1)
					{
						*temp++ = '.';
					}
				}
				for (i = 1; i < count; i++)
				{
					*temp++ = temp_num.bits[i] / 1000 + '0';
					*temp++ = temp_num.bits[i] / 100 % 10 + '0';
					*temp++ = temp_num.bits[i] / 10 % 10 + '0';
					*temp++ = temp_num.bits[i] % 10 + '0';
				}
				*temp++ = '*';
				*temp++ = '1';
				*temp++ = '0';
				*temp++ = '^';
				s_int_to_str(temp, power);
			}
			else
			{
				// 使用小数（整数）
				if (temp_num.power > 0)
				{
					power = temp_num.power * 4;
					temp += s_int_to_str(temp, temp_num.bits[0]);
					i = 1;
					while (power > 4)
					{
						*temp++ = temp_num.bits[i] / 1000 + '0';
						*temp++ = temp_num.bits[i] / 100 % 10 + '0';
						*temp++ = temp_num.bits[i] / 10 % 10 + '0';
						*temp++ = temp_num.bits[i] % 10 + '0';
						i++;
						count--;
						power -= 4;
					}
					if (count > 1)
					{
						*temp++ = '.';
						while (count > 1)
						{
							*temp++ = temp_num.bits[i] / 1000 + '0';
							*temp++ = temp_num.bits[i] / 100 % 10 + '0';
							*temp++ = temp_num.bits[i] / 10 % 10 + '0';
							*temp++ = temp_num.bits[i] % 10 + '0';
							i++;
							count--;
						}
					}
					*temp ='\0';
				}
				else
				{
					
					power = temp_num.power * 4;
					*temp++ = '0';
					*temp++ = '.';
					if (temp_num.bits[0] == 0)
					{
						temp--;
					}
					i = 0;
					while (power < 0)
					{
						*temp++ = '0';
						power++;
					}
					while (count >= 1)
					{
						*temp++ = temp_num.bits[i] / 1000 + '0';
						*temp++ = temp_num.bits[i] / 100 % 10 + '0';
						*temp++ = temp_num.bits[i] / 10 % 10 + '0';
						*temp++ = temp_num.bits[i] % 10 + '0';
						i++;
						count--;
					}
					*temp = '\0';
				}
			}
		}
	}
	return str;
}