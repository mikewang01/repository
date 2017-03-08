#include "GU620.h"
#include "led.h"
#include "stmflash.h"
#include <stdio.h>
#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

rt_uint8_t GU620_ONOFF = 1;
rt_device_t GU620_dev;
int gps_flag= 0;     //gps开关标志
extern int gps_onoff;
extern int TCP_reconnet_flag;
GU620_IP_t rt_GU620_IP_t={"111.207.18.18",56151};  //初始IP及端口  61650
char IpPort[50];

//功能：GU620关机
void GU620_off(void)
{
	rt_pin_write(pGU620,PIN_LOW);
	LOG(DEBUG,("GU620 TURN OFF......\r\n"));
}
//功能：GU620开机
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
//功能：读取信号质量
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
		rt_device_write(GU620_dev, 0, "AT+CSTT?\r", rt_strlen("AT+CSTT?\r"));  //查找运营商
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

/*功能：发送AT指令 并比较返回结果是否正确*******/
/*返回值：RT_EOK 正确 RT_ERROR 错误****/
/*_ATcmdString：发送的AT指令****/
/*_delay：延时的心跳周期****/
/*_response_string：比对的返回结果****/
/*_count：重试次数****/
extern int sendAtCmdWaitResp(char *atCmdString, rt_uint16_t delay, char *responseString,char count)
{
	char* ch;
	char respBuf[RESP_BUF_LEN +1];
	
	while(count-- >0)
	{
		int len = 0,tol_len = 0;
//		GP620_rss();  //读取一次信号质量	
		
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
		ch = rt_strstr(respBuf, responseString);//responseString为respBuf子串，则返回地址
		
		
		//只要有地址返回，即返回值里有我们想要的字符，则退出
		if (ch != RT_NULL)
		{
			return GU620_OK;
		}
	}
	return GU620_ERR;
}
//重新链接TCP
extern int TCP_ReConnet(void)
{
		u8 *p1;
		u8 err;
		gps_flag = 0;
		rt_pin_write(pLED,PIN_HIGH);
		err = sendAtCmdWaitResp("AT+GPSUART=0\r",RT_TICK_PER_SECOND*5, "OK",10);     //串口1输出
		if(err != GU620_OK)
		{
				GU620_ONOFF = 1;
				gps_flag = 0;
				return err;
		}
//		err = sendAtCmdWaitResp("AT+GPSPWR=0\r",RT_TICK_PER_SECOND*5,"OK",10);    //GPS断电
//		if(err != GU620_OK)
//		{
//				GU620_ONOFF = 1;
//				gps_flag = 0;
//				return err;	
//		}
		err = sendAtCmdWaitResp("AT+CIPCLOSE=1\r",RT_TICK_PER_SECOND*5, "OK",30); //首先快速断开TCP链接
		if(err != GU620_OK)
		{
				GU620_ONOFF = 1;
				gps_flag = 0;
				return err;
		}
		STMFLASH_Read(FLASH_SAVE_ADDR,(rt_uint16_t*)IpPort,sizeof(IpPort));    //从flash中读出ip
		p1 = (u8*)rt_strstr(IpPort,"AT+CIPSTART");
		if(p1 != NULL)
		{
			err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",30);            //重新建立链接TCP	
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
			err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",30);            //重新建立链接TCP
			if(err != GU620_OK)
			{
					GU620_ONOFF = 1;
					gps_flag = 0;
					return err;
			}
			rt_pin_write(pLED,PIN_LOW);
		}
		gps_flag = 1;
		return GU620_OK;   //GU620_OK   为 0 
}
//配置IP
int IP_Process(char ip[20],unsigned short port)
{
	unsigned short num = 0;
	u8 err;
	gps_flag = 0;
	sendAtCmdWaitResp("AT+GPSUART=0\r",RT_TICK_PER_SECOND, "OK",10);     //
	rt_pin_write(pLED,PIN_HIGH);
	sendAtCmdWaitResp("AT+GPSPWR=0\r",RT_TICK_PER_SECOND,"OK",5);    //GPS断电
	
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
	
	
	err = sendAtCmdWaitResp("AT+CIPCLOSE=1\r",RT_TICK_PER_SECOND*5, "OK",30); //首先快速断开TCP链接
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
	
	err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",50);            //重新建立链接TCP
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
	
	err = sendAtCmdWaitResp("AT+GPSPWR=0\r",RT_TICK_PER_SECOND*2, "OK",5);     //关闭GPS电源
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT\r",RT_TICK_PER_SECOND>>1, "OK",10);
	if (err != GU620_OK)
	{
		 LOG(DEBUG,("FIND GU620_DEVICE FAILED\r\n"));
		return err;
	}
	else LOG(DEBUG,("FIND GU620_DEVICE\r\n"));
	
	err = sendAtCmdWaitResp("ATE0\r",RT_TICK_PER_SECOND, "OK",5);     //允许注册
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+ESIMS?\r",RT_TICK_PER_SECOND, "+ESIMS: 1",5);     //检查SIM卡
//	if (err != GU620_OK)
//		return err;	

//	err = sendAtCmdWaitResp("AT+COPS=0\r",RT_TICK_PER_SECOND*5, "OK",5);     //自动选择运营商
//	if (err != GU620_OK)
//		return err;	
	
	err = sendAtCmdWaitResp("AT+CREG=1\r",RT_TICK_PER_SECOND, "OK",5);     //允许注册
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CREG?\r",RT_TICK_PER_SECOND, "OK",5);     //查询是否注册成功
	if (err != GU620_OK)
		return err;	
	
	err = findNetwork();                                   //查找网络
	if(err != GU620_OK)
		return err;
	
	GP620_rss();                                           //获取信号质量
	
	err = sendAtCmdWaitResp("AT+CGATT=1\r",RT_TICK_PER_SECOND*10, "OK",30);  //附着GPRS网络
	if (err != GU620_OK)
		return err;
	
	err = sendAtCmdWaitResp("AT+CIICR\r",RT_TICK_PER_SECOND, "OK",10);  		//激活网络
	if (err != GU620_OK)
		return err;

	err = sendAtCmdWaitResp("AT+CIPMUX=0\r",RT_TICK_PER_SECOND, "OK",10);  //单链接
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CIPMODE=0\r",RT_TICK_PER_SECOND, "OK",10);  //配置 1：透明传输 0：非透明
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CIPCFG=0,50,0\r",RT_TICK_PER_SECOND, "OK",10);  //空闲等待50秒后发送
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+CIPPACK=0,\"LYW\"\r",RT_TICK_PER_SECOND, "OK",10);  //设置模块发送的心跳包数据格式为“ LYW”，
	if (err != GU620_OK)
		return err;	
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(rt_uint16_t*)IpPort,sizeof(IpPort));    //从flash中读出ip
	p1 = (u8*)rt_strstr(IpPort,"AT+CIPSTART");
	if(p1 != NULL)
	{
		err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",50);            //重新建立链接TCP
		if (err != GU620_OK)
			return err;
		else rt_pin_write(pLED,PIN_LOW);
	}
	else
	{
		rt_sprintf(IpPort,"AT+CIPSTART=\"TCP\",\"%s\",%d,2\r",rt_GU620_IP_t.ip,rt_GU620_IP_t.port);	
		err = sendAtCmdWaitResp(IpPort,RT_TICK_PER_SECOND*5, "CONNECT OK",50);            //重新建立链接TCP
		if (err != GU620_OK)
			return err;
		else rt_pin_write(pLED,PIN_LOW);
	}

		
//	err = sendAtCmdWaitResp("AT+CIPSTART=\"TCP\",\"111.207.18.18\",56153,2\r",RT_TICK_PER_SECOND*10, "CONNECT OK",50);  //建立TCP
//	if (err != GU620_OK)
//	return err;
//	else rt_pin_write(pLED,PIN_LOW);

	return GU620_OK;
}



//初始化串口
rt_uint8_t GU620_Init(void)
{
	GU620_dev = rt_device_find("uart5");
	
	if(GU620_dev != RT_NULL)
	{	
		return GU620_ERR;  //没有进行相关判断处理
	}
	else
		LOG(DEBUG,("NOT Find GU620_uart \r\n"));
	
	return GU620_OK;   //没有进行相关判断处理
}

static void rt_thread_entry_GU620(void* parameter)
{
	rt_device_t dev;
	char tmpbuf[32];
	
	GU620_Init();     //初始化串口
	if(rt_device_open(GU620_dev,RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX) != GU620_OK)
	{
		LOG(DEBUG,("Open GU620 failed \r\n"));
	}

	while(1)
	{
		if(GU620_ONOFF)
		{
			GU620_on();
			if(gprsDail() != GU620_OK)       //进行拔号
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
	rt_pin_mode(pGU620,PIN_MODE_OUTPUT);    //打开GU620电源
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
//软件复位
void sysreset()
{
	__set_FAULTMASK(1);  //关闭所有中断
	NVIC_SystemReset();  //复位

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
		
	}//断开TCP
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





