#ifndef __BMP180_H__
#define __BMP180_H__

#define	BMP180_SlaveAddress     0xee	//��ѹ������������ַ

#define   OSS 0	 //	BMP085ʹ��
#define   REG_Pressure 0x34 // oss=0 4.5


void Init_BMP180(void);//��ʼ��IO   ��ȡУ׼����
void bmp180Convert(void);	//У׼�¶Ⱥ���ѹ ��������ֱ���� result_UP result_UT

#endif
