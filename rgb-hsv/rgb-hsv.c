#include <stdio.h>

unsigned int Rgb2Hsv(unsigned int u32RGB);

int main(int argc, char *argv[])
{
	printf("Hello World %u, %u\n",-65497, -15859457);
	
	printf("trgb = 0x%x, 0x%x, 0x%x\n",0xFF0017FF, 0xFFFF0027, 0xFF0E00FF);
	printf("HSV = 0x%x, 0x%x, 0x%x\n", Rgb2Hsv(0xFF0017FF), Rgb2Hsv(0xFFFF0027), Rgb2Hsv(0xFF0E00FF));
	return 0;
}

#define MODE754HSV 9
#define MODE655HSV 10
unsigned int Rgb2Hsv(unsigned int u32RGB)
{
	int iR = (u32RGB&0xff0000)>>16;
	int iG = (u32RGB&0xff00)>>8;
	int iB = u32RGB & 0xff;
	
	float H = 0;
	float R = (float)iR;
	float G = (float)iG;
	float B = (float)iB;
	
	float max = 0;
	max = (R>G)?R:G;
	max = (max>B)?max:B;
	
	float min = 0;
	min = (R>G)?G:R;
	min = (min>B)?B:min;
	
	printf("max = %f, min = %f\n", max, min);
	if(max == R)
	{
		H = (G-B)/(max-min);
	}
	else if(max == G)
	{
		H = 2 + (B-R)/(max-min);
	}
	else if(max == B)
	{
		H = 4 + (R-G)/(max - min);
	}
	
	H *= 60;
	if(H < 0)
	{
		H += 360;
	}
	printf("H = %f\n",H);
	
	float S = 0;
	if(0 == max)
	{
		S = 0;
	}
	else
	{
		S = (max - min)/max;	
	}
	printf("S = %f\n",S);
	float V = max/255;
	printf("V = %f\n",V);
	
	return ((unsigned int)H<<8)*10 | 0xff;
}

