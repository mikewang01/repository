#include "upload.h"
#include <stdio.h>
#include "tcp_upload.h"
rt_device_t GPS_dev;
#define UPLOAD_DEBUGE    1
#define UPLOAD_RELEASE   1

rt_nmead_t gps;  //经纬度结构体
rt_nmea_msg_t gpsx;																	//GPS信息

rt_device_t GU620_UART2;
extern rt_device_t GU620_dev;
extern int gps_flag;
int TCP_reconnet_flag = 0;
extern int GU620_ONOFF;
extern int sendAtCmdWaitResp(char *atCmdString, rt_uint16_t delay, char *responseString,char count);
extern int TCP_ReConnet(void);
int gps_onoff = 1;
int tcp_up_flag = 0;
char GPS_sendbuf[256];
float gps_lon_num,gps_lat_num,lbs_lon_num,lbs_lat_num;
rt_uint8_t count = 0;
rt_uint8_t num=0;
//静态线程的 线程堆栈
static rt_uint8_t upload_stack[1024*2];
//静态线程的 线程控制块
static struct rt_thread upload_thread;

rt_sem_t sem = RT_NULL;

#define GPRMC_UPLOAD 0




//从buf里面得到第cx个逗号所在的位置
//返回值:0~0XFE,代表逗号所在位置的偏移.
//       0XFF,代表不存在第cx个逗号							  
rt_uint8_t NMEA_Comma_Pos(rt_uint8_t *buf,rt_uint8_t cx)
{	 		    
	rt_uint8_t *p=buf;
	while(cx)
	{		 
//		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf=='*'||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n函数
//返回值:m^n次方.
rt_uint32_t NMEA_Pow(rt_uint8_t m,rt_uint8_t n)
{
	rt_uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}
//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
int NMEA_Str2num(rt_uint8_t *buf,rt_uint8_t*dx)
{
	rt_uint8_t *p=buf;
	rt_uint32_t ires=0,fres=0;
	rt_uint8_t ilen=0,flen=0,i;
	rt_uint8_t mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==','||(*p=='*') || (*p=='\0')|| (*p=='\r')|| (*p=='\n'))break;//遇到结束了
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;   //整数长度 
		else ilen++;           //小数长度
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//最多取5位小数
	*dx=flen;	 		//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 
//功能：GPS初始化
int GPS_Init(void)
{
	int err;
#if 1
	err = sendAtCmdWaitResp("AT+GPSPWR=1\r",RT_TICK_PER_SECOND,"OK",5);    //GPS上电
	if(err != GU620_OK)
		return err;
	// AT+GPSRST=2,1    2为冷启动模式   1为启用AGPS
	err = sendAtCmdWaitResp("AT+GPSRST=2,1\r",RT_TICK_PER_SECOND, "OK",5);     //设置 GPS 为冷启动模式， GPS 开始进入工作状态
	if (err != GU620_OK)
		return err;	
	
	err = sendAtCmdWaitResp("AT+GPSLOC=34\r",RT_TICK_PER_SECOND, "OK",10);     //模式GPRMC GPGGA
	if(err != GU620_OK)
		return err;
	
//	err = sendAtCmdWaitResp("AT+GPSCONT=5,1\r",RT_TICK_PER_SECOND, "OK",10);     //
//	if(err != GU620_OK)
//		return err;
	
	err = sendAtCmdWaitResp("AT+GPSUART=1,1\r",RT_TICK_PER_SECOND, "OK",10);     //串口2输出
	if(err != GU620_OK)
		return err;
	
	err = sendAtCmdWaitResp("AT+GPSIPR=115200\r",RT_TICK_PER_SECOND, "OK",10);     //GPS串口波特率 115200
	if(err != GU620_OK)
		return err;
	
	GU620_UART2 = rt_device_find("uart2");    //finded uart2
	
	if(GU620_UART2 != RT_NULL)
	{	
		LOG(DEBUG,("Finded GU620_uart2 \r\n"));
	}
	else
		LOG(DEBUG,("NOT Find GU620_uart2 \r\n"));
	
	rt_device_open(GU620_UART2,RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);   //open uart2
	
	
#endif
//	err = sendAtCmdWaitResp("AT+ENBR=0,3,\"supl.nokia.com\",7275\r",RT_TICK_PER_SECOND*2, "OK",10);     //
//	if(err != GU620_OK)
//		return err;
	return GU620_OK;
}
//重新GPS开启
int GPS_ON()
{
		int err;
		err = sendAtCmdWaitResp("AT+GPSPWR=1\r",RT_TICK_PER_SECOND,"OK",5);    //GPS上电
		if(err != GU620_OK)
		return err;

		err = sendAtCmdWaitResp("AT+GPSRST=1,1\r",RT_TICK_PER_SECOND, "OK",5);     //设置 GPS 为暧启动模式， GPS 开始进入工作状态
		if (err != GU620_OK)
		return err;	
	
//	err = sendAtCmdWaitResp("AT+ENBR=0,3,\"supl.nokia.com\",7275\r",RT_TICK_PER_SECOND*2, "OK",10);     //
//	if(err != GU620_OK)
//		return err;
#if 1
	err = sendAtCmdWaitResp("AT+GPSLOC=34\r",RT_TICK_PER_SECOND, "OK",10);     //模式GPRMC GPGGA
	if(err != GU620_OK)
		return err;
	
	err = sendAtCmdWaitResp("AT+GPSUART=1,0\r",RT_TICK_PER_SECOND, "OK",10);     //串口1输出
	if(err != GU620_OK)
		return err;
#endif
	return GU620_OK;
}
//GPS关闭
int GPS_OFF()
{
		int err;
		err = sendAtCmdWaitResp("AT+GPSPWR=0\r",RT_TICK_PER_SECOND*2,"OK",50);    //GPS上电
		if(err != GU620_OK)
		return 0;

}

//分析GPRMC信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPRMC_Analysis(rt_nmea_msg_t *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;	   
	float rs;  
	p1=(u8*)rt_strstr((const char *)buf,"GPRMC");//"$GPRMC",经常有&和GPRMC分开的情况,故只判断GPRMC.
	
//	posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
//	if(posx!=0XFF)
//	{
//		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
//		gpsx->utc.hour=temp/10000;
//		gpsx->utc.min=(temp/100)%100;
//		gpsx->utc.sec=temp%100;	 	 
//	}	
	posx=NMEA_Comma_Pos(p1,3);								//得到纬度
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//得到°
		rs=temp%NMEA_Pow(10,dx+2);				//得到'		 
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为° 
	}
	posx=NMEA_Comma_Pos(p1,4);								//南纬还是北纬 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//得到经度
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//得到°
		rs=temp%NMEA_Pow(10,dx+2);				//得到'		 
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为° 
	}
	posx=NMEA_Comma_Pos(p1,6);								//东经还是西经
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	
	
	
//	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
//	if(posx!=0XFF)
//	{
//		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
//		gpsx->utc.date=temp/10000;
//		gpsx->utc.month=(temp/100)%100;
//		gpsx->utc.year=2000+temp%100;	 	 
//	} 
}
//分析GPGGA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPGGA_Analysis(rt_nmea_msg_t *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)rt_strstr((const char *)buf,"$GPGGA");
	posx=NMEA_Comma_Pos(p1,6);								//得到GPS状态
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);								//得到用于定位的卫星数
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);								//得到海拔高度
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);  
}
//LBS解析
void LBS_Analysis(rt_nmea_msg_t *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)rt_strstr((const char *)buf,"LOCI");
	
	posx=NMEA_Comma_Pos(p1,1);
//	rt_kprintf("%d\r\n",posx);
	if(posx!=0XFF)gpsx->LBS_LAT=NMEA_Str2num(p1+posx+1,&dx);
	
	posx=NMEA_Comma_Pos(p1,2);
//	rt_kprintf("%d\r\n",posx);
	if(posx!=0XFF)gpsx->LBS_LON=NMEA_Str2num(p1+posx+1,&dx);
//	else rt_kprintf("NOT FIND \"LOCI\"\r\n");
	
//	if(posx!=0XFF)gpsx->LBS_LAT=NMEA_Str2num(p1+8,&dx);

//	if(posx!=0XFF)gpsx->LBS_LON=NMEA_Str2num(p1+19,&dx);
//	else rt_kprintf("NOT FIND \"LOCI\"\r\n");
	
	
}

//TCP发送是否成功
void TCP_SENDOK(void)
{
		char otherbuf[100];
		rt_uint32_t i = 0;
		while(1)
			{
					char* ch;
					rt_device_read(GU620_dev,0,otherbuf,sizeof(otherbuf)-1);
					ch = rt_strstr(otherbuf,"SEND OK");
					if(ch != RT_NULL)
					{
							LOG(UPLOAD_DEBUGE,("FIND \"SEND OK\"\r\n"));
							break;
					}	
					rt_thread_delay(RT_TICK_PER_SECOND/1000);
					i ++;
					if(i >= 5000)    //5s
					{
						count++;
						if(count >= 5)
						{
							count = 0;
//							GPS_OFF();
//							rt_thread_delay(RT_TICK_PER_SECOND*2);
							num = 0;
							TCP_reconnet_flag = 1;
							tcp_up_flag = 0;
							if(TCP_reconnet_flag)
								LOG(UPLOAD_DEBUGE,("ReConnet TCP\r\n"));
//							TCP_ReConnet();		
						}
								break;
					}
			}	
}
//TCP检测'>'
void TCP_CMD(void)
{
		char otherbuf[100];
		rt_uint32_t i = 0;
		while(1)
			{
					char* ch;
					rt_device_read(GU620_dev,0,otherbuf,sizeof(otherbuf)-1);
					ch = rt_strstr(otherbuf,">");
					if(ch != RT_NULL)
					{
							LOG(UPLOAD_DEBUGE,("FIND '>'\r\n"));
							break;
					}
						
					rt_thread_delay(RT_TICK_PER_SECOND/1000);
					i ++;
					if(i >= 5000)
					{
						count++;
//						if(count >= 5)
//						{
//							count = 0;
////							GPS_OFF();
//							rt_thread_delay(RT_TICK_PER_SECOND*2);
//							num = 0;
//							TCP_reconnet_flag = 1;
//							if(TCP_reconnet_flag)
//								LOG(UPLOAD_DEBUGE,("ReConnet TCP\r\n"));							
//						}
								break;
					}
			}
			
		
}
//GPS定位上报
void GU620_GPSUPLOAD()
{
			rt_uint8_t GPS_buf[512];      //上报数据缓冲
			char LBS_buf[200];
			char LAT_buf[50];
			char LON_buf[50];
			char GPS_cmd[32];   //上报命令
//			char GPS_sendbuf[256];
			char GPS_UPBUF[70];
			char GPS_UPBUF1[70];
			char GPS_lat[32];
			char LAT_BUF[50];
			char GPS_lon[32];
			char LON_BUF[50];
			char otherbuf[50];
			char PosslNUM[40];
			
			char end[]="******************************************************************\r\n";
			rt_err_t result;
			int len = 0,tol_len = 0;
			rt_uint8_t posx , dx,*p1;
			rt_uint32_t temp;
			static u8 i;
			static rt_uint32_t log_count=0;
			float tp;
			
	
//			rt_device_read(GU620_UART2,0,otherbuf,sizeof(otherbuf)-1);    //清空一次串口2
			
//			if(rt_sem_take(sem,1000) == RT_EOK)
//			{
//					rt_thread_delay(RT_TICK_PER_SECOND);	
//					rt_device_read(GU620_dev,0,GPS_buf,sizeof(GPS_buf)-1);
//					rt_sem_release(sem);
//			}

//			rt_enter_critical();    //进入临界区
//			rt_thread_delay(RT_TICK_PER_SECOND*1);
			rt_device_read(GU620_UART2,0,GPS_buf,sizeof(GPS_buf)-1);    //读取串口2数据
//			rt_exit_critical();     //退出临界区
			
//			rt_thread_delay(RT_TICK_PER_SECOND);
			LOG(UPLOAD_DEBUGE,("GPS_buf :\r\n%s\r\n-------------------------------------------------\r\n",(char*)GPS_buf));
			
			
#if   0//GPRMC_UPLOAD		//发送原始数据	
			rt_memset(GPS_cmd,0,sizeof(GPS_cmd));
			rt_sprintf(GPS_cmd,"AT+CIPSEND=%d\r",rt_strlen((char*)GPS_buf));   //GPS_UPBUF为要发送数据的大小
			rt_device_write(GU620_dev,0,GPS_cmd,rt_strlen(GPS_cmd));      //写命令
			rt_thread_delay(RT_TICK_PER_SECOND/2);
			
			rt_device_write(GU620_dev,0,GPS_buf,rt_strlen((char*)GPS_buf));        //发送原始数据
			rt_thread_delay(RT_TICK_PER_SECOND*1.5);    //当数据量大时，应加大延时时间
			rt_device_write(GU620_dev,0,(rt_uint8_t *)0X1A,1);      //发送结束			
			rt_thread_delay(RT_TICK_PER_SECOND/2);
			TCP_SENDOK();
#endif 		
			
			NMEA_GPRMC_Analysis(&gpsx,(rt_uint8_t *)GPS_buf);					//GPRMC解析
			NMEA_GPGGA_Analysis(&gpsx,(rt_uint8_t *)GPS_buf);					//GPGGA解析
//			LBS_Analysis(&gpsx,(rt_uint8_t *)LBS_buf);   							//基站定位解析
//数据处理
			{
					tp = gpsx.posslnum;
					if(tp > 0)
					{
							i = tp;
					}
					rt_sprintf(PosslNUM,"Valid satellite : %02d\r\n",i);
					LOG(UPLOAD_RELEASE,("\r\n%s\r\n",PosslNUM));  //显示卫星数量
					
					tp=gpsx.longitude;
					if(tp > 10)
					{
							gps_lon_num = tp/100000;	
					}		
					sprintf(LON_BUF,"GPS LON : %.5f\r\n",gps_lon_num);	
					LOG(UPLOAD_RELEASE,("%s\r\n",LON_BUF));  //显示经度
					
					tp=gpsx.latitude;
					if(tp > 10)
					{
							gps_lat_num = tp/=100000;		
					}				
					sprintf(LAT_BUF,"GPS LAT : %.5f\r\n",gps_lat_num);			
					LOG(UPLOAD_RELEASE,("%s\r\n",LAT_BUF));   //显示纬度
					
					tp = gpsx.LBS_LON/100000;
					if(tp > 10)
					{
							lbs_lon_num = tp;		
					}		
					sprintf(LON_buf,"LBS LON : %.5f\r\n",lbs_lon_num);						
					LOG(UPLOAD_RELEASE,("%s\r\n",LON_buf));
					
					tp = gpsx.LBS_LAT/100000;
					if(tp > 10)
					{
							lbs_lat_num = tp;		
					}		
					sprintf(LAT_buf,"LBS LAT : %.5f\r\n",lbs_lat_num);							
					LOG(UPLOAD_RELEASE,("%s\r\n",LAT_buf));
		  }
			
			result = rt_sem_take(sem,1000);  //take sem for wait 1s
			if(result == RT_EOK)
			{
					rt_kprintf("GPS_UP Take sem success !\r\n");
					sprintf(GPS_sendbuf,"%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n",PosslNUM,LON_BUF,LAT_BUF,LON_buf,LAT_buf,end);
					rt_sem_release(sem);//数据准备好后，释放一次信号量
			}
			
			
/********************************************GPS LBS 上报***************************************************/	
#if 0
//			sprintf(GPS_sendbuf,"%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n",PosslNUM,LON_BUF,LAT_BUF,LON_buf,LAT_buf,end);
			rt_thread_delay(250);
			rt_memset(GPS_cmd,0,sizeof(GPS_cmd));
			rt_sprintf(GPS_cmd,"AT+CIPSEND=%d\r\n",rt_strlen(GPS_sendbuf));   //GPS_sendbuf为要发送数据的大小
			rt_device_write(GU620_dev,0,GPS_cmd,rt_strlen(GPS_cmd));      //写命令
			TCP_CMD();
			rt_device_write(GU620_dev,0,GPS_sendbuf,rt_strlen(GPS_sendbuf));        //发送卫星、经纬度
			rt_thread_delay(RT_TICK_PER_SECOND/5);
//			rt_device_write(GU620_dev,0,(rt_uint8_t *)0X1A,1);      //发送结束
			rt_device_write(GU620_dev,0,(rt_uint8_t *)"\r\n",2);      //发送结束
			TCP_SENDOK();
#endif
			LOG(UPLOAD_RELEASE,("****************************************\r\n"));
			rt_thread_delay(100);
				
}

//基站定位上报
void GU620_LBSUP()
{
				char LBS_buf[200];
//				char LBS_buf[]="+LOCI: 0, 40.096965, 16.365900";
				char LAT_buf[50];
				char LON_buf[50];
				char GPS_cmd[32];   //上报命令
				char otherbuf[50];
				u8 *p1;
	/**********************读取基站信息********************/
#if 0
				rt_kprintf("**************************************\r\n");
				while(i)
					{
						if(i<=0) i=5;
						
						rt_device_write(GU620_dev,0,"AT+ENBR\r",rt_strlen("AT+ENBR\r"));
						rt_thread_delay(300);		
						rt_memset(LBS_buf,0,sizeof(LBS_buf));						
						rt_device_read(GU620_dev,0,LBS_buf,sizeof(LBS_buf)-1);
						rt_thread_delay(50);
						rt_kprintf("LBS_buf1:\r\n%s",LBS_buf);
						rt_kprintf("**************************************\r\n");
						i--;
						if(rt_strstr(LBS_buf,"ENBR") != RT_NULL) break;
					}
	      LOG(UPLOAD_DEBUGE,("LBS first:\r\n%s\r",LBS_buf));
//				rt_thread_delay(5);
				rt_kprintf("**************************************\r\n");
	
				rt_device_read(GU620_dev,0,otherbuf,sizeof(otherbuf)-1);
#endif
					
#if 1
//				while(j)
				{
						GPS_OFF(); //关闭GPS获取LBS
						rt_thread_delay(RT_TICK_PER_SECOND*5);
						rt_device_read(GU620_dev,0,otherbuf,sizeof(otherbuf)-1);
						rt_device_write(GU620_dev,0,"AT+ENBR=0\r",rt_strlen("AT+ENBR=0\r"));
						rt_thread_delay(RT_TICK_PER_SECOND*35);
						rt_memset(LBS_buf,0,sizeof(LBS_buf));
						rt_device_read(GU620_dev,0,LBS_buf,sizeof(LBS_buf)-1);	
						rt_thread_delay(RT_TICK_PER_SECOND/2);
						rt_kprintf("LBS_buf:\r\n%s\r\n--------------------------\r\n",LBS_buf);
					
						LBS_Analysis(&gpsx,(rt_uint8_t *)LBS_buf);   //基站定位解析
						p1 = (u8*)rt_strstr(LBS_buf,"LOCI");
						if(p1 != NULL)
						rt_thread_delay(RT_TICK_PER_SECOND*20);
						GPS_ON(); //重新开启GPS
			}
				
#endif
}
//入口函数
void rt_thread_entry_GU620_Upload(void* parameter)
{
		
	while(1)
	{
		if(gps_flag && gps_onoff)
		{
	
//			GPS_Init();
			if(GPS_Init() != GU620_OK)
			{				
					gps_flag = 0;
					GU620_ONOFF = 1;
			}
			else
				tcp_up_flag = 1;
		}
		
		
		if(gps_flag)
		{			
				GU620_GPSUPLOAD();
				if(TCP_reconnet_flag == 1)
				{
					gps_flag = 0;					
				}
			  if(num > 30)
				{					
//					GU620_LBSUP();
//					rt_thread_delay(RT_TICK_PER_SECOND*5);  // 1000ms*10
					num = 0;
				}

//				rt_thread_delay(RT_TICK_PER_SECOND*5);  // 1000ms*60
				gps_onoff = 0;
				num ++;
		}	
		rt_thread_delay(500*2);  //上报间隔 5000ms
	}
}

void GU620_Upload(void)
{
	
	rt_err_t result;
	/*创建一个信号量，初始值为0*/
	sem = rt_sem_create("sem",1,RT_IPC_FLAG_FIFO);
	
	result = rt_thread_init(&upload_thread,"GU620_upload",&rt_thread_entry_GU620_Upload,RT_NULL,
							(rt_uint8_t*)&upload_stack[0],sizeof(upload_stack),7,10);
	
//	rt_thread_t rt_upload;
//	rt_upload = rt_thread_create("upload",&rt_thread_entry_GU620_Upload,RT_NULL,
//									1024*2,7,10);
	if(result == RT_EOK)
	rt_thread_startup(&upload_thread);
}

