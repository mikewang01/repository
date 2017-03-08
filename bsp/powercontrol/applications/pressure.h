#ifndef __PRESSURE_H__
#define __PRESSURE_H__

#define BMP180_OK      0
#define BMP180_ERR    ( -1 )
#define	LOG(debug,message) do {if(debug) rt_kprintf message;}while(0)
void PressureInit();
#endif

