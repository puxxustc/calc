/*********************************************************************
*
*文件：math_fun.c
*功能：实现数学函数
*备注：
*
*********************************************************************/
#include <string.h>
#include "hpfloat.h"


/*****************************************************************
*函数：hp_sqrt()
*功能：高精度开方
******************************************************************/
hpfloat *hp_sqrt(hpfloat *hp_num)
{
	if (hp_num->bits[0] == 0)
	{
		return hp_num;
	}
	else if (hp_num->sign != 0)
	{
		hp_num->error = FUN_ERROR;
		return hp_num;
	}
	else
	{
		hpfloat temp, x1, x2;
		int i;

		hp_zero(&x1);
		hp_zero(&x2);
		hp_zero(&temp);
		hp_int_to_hp(&x2, 1);
		do
		{
			for (i = 0; i < 5; i++)
			{
				hp_div(hp_num, &x2, &temp);
				hp_add(&x2, &temp, &x1);
				hp_div_int(&x1, 2);

				hp_div(hp_num, &x1, &temp);
				hp_add(&x1, &temp, &x2);
				hp_div_int(&x2, 2);
			}
		} while (hp_cmp(&x1, &x2) != 0);
		*hp_num = x2;
		return hp_num;
	}	
}



/*****************************************************************
*函数：hp_sqrt3()
*功能：高精度开立方
******************************************************************/
hpfloat *hp_sqrt3(hpfloat *hp_num)
{
	if (hp_num->bits[0] == 0)
	{
		return hp_num;
	}
	else
	{
		hpfloat temp1, temp2, x1, x2;
		int i;

		x1 = *hp_num;
		do
		{
			for (i = 0; i < 5; i++)
			{
				hp_add(&x1, &x1, &temp1);		// temp1 = x1 * 2;
				hp_div(hp_num, &x1, &temp2);	// temp2 = hp_num / x1 / x1;
				hp_div(&temp2, &x1, &temp2);	
				hp_add(&temp1, &temp2, &x2);	// x2 = temp1 + temp2;
				hp_div_int(&x2, 3);				// x2 = x / 3;
				
				hp_add(&x2, &x2, &temp1);		// temp1 = x2 * 2;
				hp_div(hp_num, &x2, &temp2);	// temp2 = hp_num / x2 / x2;
				hp_div(&temp2, &x2, &temp2);	
				hp_add(&temp1, &temp2, &x1);	// x1 = temp1 + temp2;
				hp_div_int(&x1, 3);				// x1 = x / 3;

			}
		} while (hp_cmp(&x1, &x2) != 0);
		*hp_num = x1;
		return hp_num;
	}	
}



/*****************************************************************
*函数：hp_sin()
*功能：高精度正弦
******************************************************************/
hpfloat *hp_sin(hpfloat *x)
{
	hpfloat sum;
	hpfloat num;
	hpfloat x_2;
	int i;
	int flag = 0;
	
	memset(&sum, 0, sizeof(hpfloat));	// sum = 0;
	
	{
		// 把x的范围限制到0~2pi
		hpfloat temp;
		memset(&temp, 0, sizeof(hpfloat));
		hp_div(x, hp_pi(), &temp);		// temp = x / pi;
		hp_div_int(&temp, 2);			// temp = x / 2;
		// temp = [temp];
		if (temp.power <= 0)
		{
			memset(&temp, 0, sizeof(hpfloat));
		}
		else
		{
			memset(temp.bits + temp.power, 0, sizeof(int) * (HP_LENGTH - temp.power));
		}
		hp_mul(&temp, hp_pi(), &temp);	// temp = temp * pi;
		hp_sub(x, &temp, x);			// x = x - temp;
		hp_sub(x, &temp, x);
	}

	hp_mul(x, x, &x_2);				// x_2 = x * x;
	for(i = 1, num = *x; num.bits[0] != 0; i += 4)
	{
		//用Taylor展开求sin(x)的值
		hp_add(&sum, &num, &sum);				// sum += num;

		hp_mul(&num, &x_2, &num);				// num = num * x * x
		hp_div_int(&num, (i + 1) * (i + 2));	// num = num / ((i+1)*(i+2))
		
		hp_sub(&sum, &num, & sum);				//sum -= num

		hp_mul(&num, &x_2, &num);				// num = num * x * x
		hp_div_int(&num, (i + 3) * (i + 4));	// num = num / ((i+3)*(i+4))
	}

	if (sum.power < -HP_POWER_LIMIT)
	{
		memset(x, 0, sizeof(hpfloat));
	}
	else
	{
		*x = sum;
	}
	return x;
}

/*****************************************************************
*函数：hp_cos()
*功能：高精度余弦
******************************************************************/
hpfloat *hp_cos(hpfloat *x)
{
	hpfloat temp = *hp_pi();
	hp_div_int(&temp, 2);
	hp_sub(&temp, x, x);

	hp_sin(x);		//用sin函数求cos
	
	return x;
}


/*****************************************************************
*函数：hp_tan()
*功能：高精度正切
******************************************************************/
hpfloat *hp_tan(hpfloat *x)
{
	hpfloat temp = *x;
	hp_sin(x);
	hp_cos(&temp);
	hp_div(x, &temp, x);
	return x;
}



/*****************************************************************
*函数：hp_exp()
*功能：高精度幂函数e^x
******************************************************************/
hpfloat *hp_exp(hpfloat *x)
{
	hpfloat pow1, pow2;
	if(x->power >2)//当指数大于2时，肯定溢出，所以不进行运算
	{
		x->error = OVER_FLOW;
		return x;
	}

	memset(&pow1, 0, sizeof(hpfloat));
	pow1.bits[0] = 1;
	pow1.power = 1;

	// 计算e^[x]
	if (x->power >= 1)	// x > 1 ?
	{
		int int_x;					// x的整数部分
		hpfloat temp = *hp_e();		// temp = e;

		if (x->power == 1)
		{
			int_x = x->bits[0];
		}
		else
		{
			int_x = x->bits[0] * 10000 + x->bits[1];
		}

		if (x->sign == 0)
		{
			while (int_x > 0)
			{
				if (int_x & 1)
				{
					hp_mul(&pow1, &temp, &pow1);	// pow1 *= temp;
				}
				hp_mul(&temp, &temp, &temp);		// temp = temp^2;
				int_x /= 2;
			}
		}
		else
		{
			while (int_x > 0)
			{
				if (int_x & 1)
				{
					hp_div(&pow1, &temp, &pow1);	// pow1 /= temp;
				}
				hp_mul(&temp, &temp, &temp);		// temp = temp^2;
				int_x /= 2;
			}
		}
		
		//x = {x};
		memmove(x->bits, x->bits + x->power, sizeof(int) * (HP_LENGTH - x->power));
		memset(x->bits + (HP_LENGTH - x->power), 0, sizeof(int) * x->power);
		x->power = 0;
	}

	// 计算e^{x}
	{
		hpfloat num;
		int i;

		memset(&pow2, 0, sizeof(hpfloat));
		pow2.bits[0] = 1;
		pow2.power = 1;
		num = *x;

		for (i = 2; num.bits[0] != 0; i++)
		{
			hp_add(&pow2, &num, &pow2);		// pow2 += num;
			hp_mul(&num, x, &num);			// num *= x;
			hp_div_int(&num, i);			// num /= i;
		}
	}

	hp_mul(&pow1, &pow2, x);

	return x;
}



/*****************************************************************
*函数：hp_sinh()
*功能：高精度双曲正弦
******************************************************************/
hpfloat *hp_sinh(hpfloat *x)
{
	hpfloat temp;
	temp = *x;
	temp.sign ^= -1;
	hp_exp(x);				// x = e^x;
	hp_exp(&temp);			// temp = e^(-x);
	hp_sub(x, &temp, x);	// x = x - temp;
	hp_div_int(x, 2);		// x = x / 2;
	return x;
}



/*****************************************************************
*函数：hp_cosh()
*功能：高精度双曲余弦
******************************************************************/
hpfloat *hp_cosh(hpfloat *x)
{
	hpfloat temp;
	temp = *x;
	temp.sign ^= -1;
	hp_exp(x);				// x = e^x;
	hp_exp(&temp);			// temp = e^(-x);
	hp_add(x, &temp, x);	// x = x + temp;
	hp_div_int(x, 2);		// x = x / 2;
	return x;
}



/*****************************************************************
*函数：hp_tanh()
*功能：高精度双曲正切
******************************************************************/
hpfloat *hp_tanh(hpfloat *x)
{
	hpfloat temp1, temp2;
	temp1 = *x;
	hp_exp(&temp1);
	temp2 = *x;
	temp2.sign ^= -1;
	hp_exp(&temp2);
	hp_sub(&temp1, &temp2, x);
	hp_add(&temp1, &temp2, &temp1);
	hp_div(x, &temp1, x);
	return x;
}



/*********************************************************************
***计算hp_ln_10（10）
****
**********************************************************************/
const hpfloat *hp_ln_10()
{
	static hpfloat ln_10 = {0};

	if (ln_10.bits[0] == 0)
	{
		hpfloat num, power_x, x;
		int i;
		hp_int_to_hp(&x, 10);
		hp_div(&x, hp_e(), &x);
		hp_div(&x, hp_e(), &x);
		hp_sub_int(&x, 1);
		power_x = x;
		num = x;
		for (i = 2; num.bits[0] != 0; i += 2)
		{
			hp_add(&ln_10, &num, &ln_10);
			hp_mul(&power_x, &x, &power_x);
			num = power_x;
			hp_div_int(&num, i);
			hp_sub(&ln_10, &num, &ln_10);
			hp_mul(&power_x, &x, &power_x);
			num = power_x;
			hp_div_int(&num, i + 1);
		}
		hp_add_int(&ln_10, 2);
	}
	return &ln_10;
}



/*********************************************************************
***计算hp_ln_10000（10）
****
**********************************************************************/
const hpfloat *hp_ln_10000()
{
	static hpfloat ln_10000 = {0};

	if (ln_10000.bits[0] == 0)
	{
		ln_10000 = *hp_ln_10();
		hp_add(&ln_10000, &ln_10000, &ln_10000);
		hp_add(&ln_10000, &ln_10000, &ln_10000);
	}
	return &ln_10000;
}


/*****************************************************************
*函数：hp_ln()
*功能：高精度自然对数
******************************************************************/
hpfloat *hp_ln(hpfloat *x)
{
	if ((x->sign != 0) || (x->bits[0] == 0))
	{
		x->error = FUN_ERROR;
		return x;
	}
	else
	{
		hpfloat result;
		hpfloat temp_x = *x;
		int count = 0;		// 乘以e的个数
		temp_x.power = 0;
		// 先算底数部分的对数值
		while ((temp_x.power == 0) && (temp_x.bits[0] < 5000))
		{
			hp_mul(&temp_x, hp_e(), &temp_x);
			count++;
		}
		if (count != 0)
		{
			hp_int_to_hp(&result, -count);
		}
		else
		{
			memset(&result, 0, sizeof(hpfloat));
		}
		{
			hpfloat num, power_x;
			int i;
			hp_sub_int(&temp_x, 1);
			power_x = temp_x;
			num = temp_x;
			for (i = 2; num.bits[0] != 0; i += 2)
			{
				hp_add(&result, &num, &result);
				hp_mul(&power_x, &temp_x, &power_x);
				num = power_x;
				hp_div_int(&num, i);
				hp_sub(&result, &num, &result);
				hp_mul(&power_x, &temp_x, &power_x);
				num = power_x;
				hp_div_int(&num, i + 1);
			}
		}
		if (x->power != 0)
		{
			hpfloat temp = *hp_ln_10000();
			hp_mul_int(&temp, x->power);
			hp_add(&result, &temp, &result);
		}

		*x = result;
		return x;
	}
}




/*****************************************************************
*函数：hp_lg(x)
*功能：计算lg(x）的值
*****************************************************************/
hpfloat *hp_lg(hpfloat *x)
{
	int power = x->power;

	x->power = 0;
	hp_ln(x);
	hp_div(x, hp_ln_10(), x);
	hp_add_int(x, power * 4);
	return x;
}



/*****************************************************************
*函数：hp_pow()
*功能：高精度指数函数
******************************************************************/
hpfloat *hp_pow(const hpfloat *x, const hpfloat *power, hpfloat *result)
{
	hpfloat temp;

	temp = *x;
	hp_ln(&temp);
	hp_mul(&temp, power, &temp);
	hp_exp(&temp);
	*result = temp;
	return result;
}