#include "GU620.h"
#include "led.h"
#include "stmflash.h"
#include <stdio.h>
#define FLASH_SAVE_ADDR  0X08070000		//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)

rt_uint8_t GU620_ONOFF = 1;
rt_device_t GU620_dev;
int gps_flag= 0;     //gps���ر�־
extern int gps_onoff;
extern int TCP_reconnet_flag;
GU620_IP_t rt_GU620_IP_t={"111.207.18.18",56151};  //��ʼIP���˿�  61650
char IpPort[50];

//���ܣ�GU620�ػ�
void GU620_off(void)
{
	rt_pin_write(pGU620,PIN_LOW);
	LOG(DEBUG,("GU620 TURN OFF......\r\n"));
}
//���ܣ�GU620����
void GU620_on(void)
{
	u8 i=0;
	rt_pin_write(pGU620,PIN_LOW);
	rt_thread_delay(RT_TICK_PER_SECOND*3);
	rt_pin_write(pGU620,PIN_HIGH);
	LOG(DEBUG,("\r\nGU620 TURN ON\r\n"));
//	LOG(DEBUG,("GU620 TURN ON\r\n"));
	while(1)
	{
			LOG(DEBUG,("."));
			rt_thread_delay(RT_TICK_PER_SECOND/5);
			i++;
			if(i>=100)
			{
					i=0;
					LOG(DEBUG,(".\r\n"));
					break;
			}
	}
}
//���ܣ���ȡ�ź�����
void GP620_rss(void)
{
	char tmpbuf[32];
	rt_device_write(GU620_dev,0,"AT+CSQ\r",rt_strlen("AT+CSQ\r"));
	rt_thread_delay(RT_TICK_PER_SECOND>>1);
	rt_memset(tmpbuf,0,sizeof(tmpbuf));
	rt_device_read(GU620_dev, 0, tmpbuf, sizeof(tmpbuf) -1);
	LOG(DEBUG,("[%s]==>read=%s\n","GU620",tmpbuf));
}

static int findNetwork(void)
{
	char tmpbuf[64];	
	char *str = RT_NULL;
	int i ;
	for(i=0;i<RETRY_CONUT;i++)
	{
		LOG(DEBUG,("[%s]==>find networking ....\n","GU620"));
		LOG(DEBUG,("[%s]==>find networking ....\n","GU620"));
		rt_device_write(GU620_dev, 0, "AT+CSTT?\r", rt_strlen("AT+CSTT?\r"));  //������Ӫ��
		rt_thread_delay(RT_TICK_PER_SECOND>>1);
		rt_memset(tmpbuf,0,sizeof(tmpbuf));
		rt_device_read(GU620_dev, 0, tmpbuf, sizeof(tmpbuf) -1 );
		LOG(DEBUG,("[%s]==>read-->%s\n","GU620",tmpbuf));
	}

	return GU620_OK;
}
void itoa(unsigned short i, char *buff)
{
	char flag = 0;

	if((*buff = i / 10000) || flag == 1)
	{
		*buff = *buff + 0x30;
		buff++;
		flag = 1;
	}
	if((*buff = i / 1000 % 10) || (flag == 1))
	{
		*buff = *buff + 0x30;
		buff++;
		flag = 1;
	}
	if((*buff = i / 100 % 10) || (flag == 1))
	{
		*buff = *buff + 0x30;
		buff++;
		flag = 1;
	}
	if((*buff = i / 10 % 10) || (flag == 1))
	{
		*buff = *buff + 0x30;
		buff++;
		flag = 1;
	}
	if((*buff = i % 10) || flag == 1)
		*buff = *buff + 0x30;
}

/*���ܣ�����ATָ�� ���ȽϷ��ؽ���Ƿ���ȷ*******/
/*����ֵ��RT_EOK ��ȷ RT_ERROR ����****/
/*_ATcmdString�����͵�ATָ��****/
/*_delay����ʱ����������****/
/*_response_string���ȶԵķ��ؽ��****/
/*_count�����Դ���****/
extern int sendAtCmdWaitResp(char *atCmdString, rt_uint16_t delay, char *responseString,char count)
{
	char* ch;
	char respBuf[RESP_BUF_LEN +1];
	
	while(count-- >0)
	{
		int len = 0,tol_len = 0;
//		GP620_rss();  //��ȡһ���ź�����	
		
		LOG(DEBUG,("[%s]==>cmd %s\n","GU620",atCmdString));
		rt_device_write(GU620_dev, 0, atCmdString, rt_strlen(atCmdString));
		rt_thread_delay(delay);
		rt_memset(respBuf, 0, sizeof(respBuf));
		
		do
		{	
			len = rt_device_read(GU620_dev, 0, respBuf + tol_len, RESP_BUF_LEN - tol_len);
			if(len < 0)
				break;
			tol_len += len;
			rt_thread_delay(10);//100ms
		}while(len > 0 && tol_len < RESP_BUF_LEN);
		
		LOG(DEBUG,("[%s]==>len =%d,%s\n","GU620",tol_len,respBuf));
		ch = rt_strstr(respBuf, responseString);//responseStringΪrespBuf�Ӵ����򷵻ص�ַ
		
		
		//ֻҪ�е�ַ���أ�������ֵ����������Ҫ���ַ������˳�
		if (ch != RT_NULL)
		{
			return GU620_OK;
		}
	}
	return GU620_ERR;
}
//��������TCP
extern int TCP_ReConnet(void)
{
		u8 *p1;
		u8 err;
		gps_flag = 0;
		rt_pin_write(pLED,PIN_HIGH);
		err = sendAtCmdWaitResp("AT+GPSUART=0\r",RT_TICK_PER_SECOND*5, "OK",10);     //����1���
		if(err != GU620_OK)
		{
				GU620_ONOFF = 1;
				gps_flag = 0;
				return err;
		}
//		err = sendAtCmdWaitResp("AT+GPSPWR=0\r",RT_TICK_PER_SECOND*5,"OK",10);    //GPS�ϵ�
//		if(err != GU620_OK)
//		{
//				GU620_ONOFF = 1;
//				gps_flag = 0;
//				return err;	
//		}
		err = sendAtCmdWaitResp("AT+CIPCLOSE=1\r",RT_TICK_PER_SECOND*5, "OK",30); //���ȿ��ٶϿ�TCP����
		if(err != GU620_OK)
		{
				GU620_ONOFF = 1;
				gps_flag = 0;
				return err;
		}
		STMFLASH_Read(FLASH_SAVE_ADDR,(rt_uint16_t*)IpPort,sizeof(IpPort));    //��flash�ж���ip
		p1 = (u8*)rt_strstr(IpPort,"AT+CIPSTART");
		if(p1 != NULL)
		{
			err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",30);            //���½�������TCP	
		if(err != GU620_OK)
		{
				GU620_ONOFF = 1;
				gps_flag = 0;
				return err;
		}			
			rt_pin_write(pLED,PIN_LOW);
		}
		else
		{
			rt_sprintf(IpPort,"AT+CIPSTART=\"TCP\",\"%s\",%d,2\r",rt_GU620_IP_t.ip,rt_GU620_IP_t.port);	
			err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",30);            //���½�������TCP
			if(err != GU620_OK)
			{
					GU620_ONOFF = 1;
					gps_flag = 0;
					return err;
			}
			rt_pin_write(pLED,PIN_LOW);
		}
		gps_flag = 1;
		return GU620_OK;   //GU620_OK   Ϊ 0 
}
//����IP
int IP_Process(char ip[20],unsigned short port)
{
	unsigned short num = 0;
	u8 err;
	gps_flag = 0;
	sendAtCmdWaitResp("AT+GPSUART=0\r",RT_TICK_PER_SECOND, "OK",10);     //
	rt_pin_write(pLED,PIN_HIGH);
	sendAtCmdWaitResp("AT+GPSPWR=0\r",RT_TICK_PER_SECOND,"OK",5);    //GPS�ϵ�
	
	rt_thread_delay(RT_TICK_PER_SECOND*5);//5s
	rt_kprintf("config.....\r\n");
	
	for(;num<20;num++)
	{
		rt_GU620_IP_t.ip[num] = ip[num];
		if(num>=20)
			num = 0;
	}
	rt_kprintf("Ip : %s\r\n",rt_GU620_IP_t.ip);
	rt_GU620_IP_t.port = port;
	rt_kprintf("Port : %d\r\n",rt_GU620_IP_t.port);
	
	
	err = sendAtCmdWaitResp("AT+CIPCLOSE=1\r",RT_TICK_PER_SECOND*5, "OK",30); //���ȿ��ٶϿ�TCP����
	if(err != GU620_OK)
	{
			GU620_ONOFF = 1;
			gps_flag = 0;
			return 0;
	}
	rt_sprintf(IpPort,"AT+CIPSTART=\"TCP\",\"%s\",%d,2\r",rt_GU620_IP_t.ip,rt_GU620_IP_t.port);
	rt_thread_delay(100);//1s
	
	/**************************************************************/
	
	STMFLASH_Write(FLASH_SAVE_ADDR,(rt_uint16_t*)IpPort,sizeof(IpPort));
	rt_kprintf("IP_Port saved!\r\n");
	/**************************************************************/
	
	err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",50);            //���½�������TCP
	if(err != GU620_OK)
	{
			GU620_ONOFF = 1;
			gps_flag = 0;
			return 0;
	}
	rt_kprintf("%s\r\n",IpPort);
	gps_flag = 1;
}


static int gprsDail(void)
{
	int err;
	u8 *p1;
//	char ip[]="AT+CIPSTART=\"TCP\",\"111.207.18.18\",56152,2";
	
	err = sendAtCmdWaitResp("AT+GPSPWR=0\r",RT_TICK_PER_SECOND*2, "OK",5);     //�ر�GPS��Դ
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT\r",RT_TICK_PER_SECOND>>1, "OK",10);
	if (err != GU620_OK)
	{
		 LOG(DEBUG,("FIND GU620_DEVICE FAILED\r\n"));
		return err;
	}
	else LOG(DEBUG,("FIND GU620_DEVICE\r\n"));
	
	err = sendAtCmdWaitResp("ATE0\r",RT_TICK_PER_SECOND, "OK",5);     //����ע��
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+ESIMS?\r",RT_TICK_PER_SECOND, "+ESIMS: 1",5);     //���SIM��
//	if (err != GU620_OK)
//		return err;	

//	err = sendAtCmdWaitResp("AT+COPS=0\r",RT_TICK_PER_SECOND*5, "OK",5);     //�Զ�ѡ����Ӫ��
//	if (err != GU620_OK)
//		return err;	
	
	err = sendAtCmdWaitResp("AT+CREG=1\r",RT_TICK_PER_SECOND, "OK",5);     //����ע��
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CREG?\r",RT_TICK_PER_SECOND, "OK",5);     //��ѯ�Ƿ�ע��ɹ�
	if (err != GU620_OK)
		return err;	
	
	err = findNetwork();                                   //��������
	if(err != GU620_OK)
		return err;
	
	GP620_rss();                                           //��ȡ�ź�����
	
	err = sendAtCmdWaitResp("AT+CGATT=1\r",RT_TICK_PER_SECOND*10, "OK",30);  //����GPRS����
	if (err != GU620_OK)
		return err;
	
	err = sendAtCmdWaitResp("AT+CIICR\r",RT_TICK_PER_SECOND, "OK",10);  		//��������
	if (err != GU620_OK)
		return err;

	err = sendAtCmdWaitResp("AT+CIPMUX=0\r",RT_TICK_PER_SECOND, "OK",10);  //������
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CIPMODE=0\r",RT_TICK_PER_SECOND, "OK",10);  //���� 1��͸������ 0����͸��
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CIPCFG=0,50,0\r",RT_TICK_PER_SECOND, "OK",10);  //���еȴ�50�����
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CIPPACK=0,\"LYW\"\r",RT_TICK_PER_SECOND, "OK",10);  //����ģ�鷢�͵����������ݸ�ʽΪ�� LYW����
	if (err != GU620_OK)
		return err;	
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(rt_uint16_t*)IpPort,sizeof(IpPort));    //��flash�ж���ip
	p1 = (u8*)rt_strstr(IpPort,"AT+CIPSTART");
	if(p1 != NULL)
	{
		err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",50);            //���½�������TCP
		if (err != GU620_OK)
			return err;
		else rt_pin_write(pLED,PIN_LOW);
	}
	else
	{
		rt_sprintf(IpPort,"AT+CIPSTART=\"TCP\",\"%s\",%d,2\r",rt_GU620_IP_t.ip,rt_GU620_IP_t.port);	
		err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",50);            //���½�������TCP
		if (err != GU620_OK)
			return err;
		else rt_pin_write(pLED,PIN_LOW);
	}

		
//	err = sendAtCmdWaitResp("AT+CIPSTART=\"TCP\",\"111.207.18.18\",56153,2\r",RT_TICK_PER_SECOND*10, "CONNECT OK",50);  //����TCP
//	if (err != GU620_OK)
//	return err;
//	else rt_pin_write(pLED,PIN_LOW);

	return GU620_OK;
}



//��ʼ������
rt_uint8_t GU620_Init(void)
{
	GU620_dev = rt_device_find("uart5");
	
	if(GU620_dev != RT_NULL)
	{	
		return GU620_ERR;  //û�н�������жϴ���
	}
	else
		LOG(DEBUG,("NOT Find GU620_uart \r\n"));
	
	return GU620_OK;   //û�н�������жϴ���
}

static void rt_thread_entry_GU620(void* parameter)
{
	rt_device_t dev;
	char tmpbuf[32];
	
	GU620_Init();     //��ʼ������
	if(rt_device_open(GU620_dev,RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX) != GU620_OK)
	{
		LOG(DEBUG,("Open GU620 failed \r\n"));
	}

	while(1)
	{
		if(GU620_ONOFF)
		{
			GU620_on();
			if(gprsDail() != GU620_OK)       //���аκ�
			GU620_off();
			else 
			{
				GU620_ONOFF = 0;
				gps_flag = 1;
				gps_onoff =1;
			}
			
			rt_thread_delay(RT_TICK_PER_SECOND);
		
		}
		
		if(TCP_reconnet_flag)
			{
					TCP_reconnet_flag = 0;
					if(TCP_ReConnet() != GU620_OK)
					GU620_off();
					else 
					{
						GU620_ONOFF = 0;
						gps_flag = 1;
						gps_onoff =1;
					}
					
			}
		rt_thread_delay(RT_TICK_PER_SECOND*3);
	
	}
}


int GU620_Thread(void)
{
	rt_pin_mode(pGU620,PIN_MODE_OUTPUT);    //��GU620��Դ
	rt_pin_write(pGU620,PIN_HIGH);
	rt_thread_t rt_GU620;
	
	rt_GU620 = rt_thread_create("GU620",&rt_thread_entry_GU620,RT_NULL,
									1024,9,10);
	if(rt_GU620 != RT_NULL)
		rt_thread_startup(rt_GU620);
	return GU620_OK;
}


#ifdef RT_USING_FINSH
#include <finsh.h>
#include <rtdevice.h>
//�����λ
void sysreset()
{
	__set_FAULTMASK(1);  //�ر������ж�
	NVIC_SystemReset();  //��λ

}
void DisCSQ()
{
	GP620_rss();
}


void TCP_close()
{
	int err;
	do
	{
		err = sendAtCmdWaitResp("AT+CIPCLOSE=1\r",RT_TICK_PER_SECOND/10, "OK",5);
		
	}//�Ͽ�TCP
	while( err != GU620_OK);
}


void DisIP(void)
{
	rt_kprintf("Current IP  :%s\r\n",rt_GU620_IP_t.ip);
	rt_kprintf("Current Port:%d\r\n",rt_GU620_IP_t.port);
}
FINSH_FUNCTION_EXPORT(sysreset, systemreset.)
FINSH_FUNCTION_EXPORT(DisIP, display ip port.)
FINSH_FUNCTION_EXPORT(TCP_close, close tcp.)
FINSH_FUNCTION_EXPORT(DisCSQ, show rssi.)
FINSH_FUNCTION_EXPORT(IP_Process, config IP.)
FINSH_FUNCTION_EXPORT(TCP_ReConnet, ReConnet IP.)
#endif





