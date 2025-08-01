定义上位机APP、和STM32之间数据收发的通信协议，下发数据命令格式设置如下：
CRD0AONCR： 开A设备
CRD0AOFCR： 关A设备

CRALM1ONCR：开警报1
CRALM1OFCR：关警报1
CRALM2ONCR：开警报2
CRALM2OFCR：关警报2	
	

VFS1A*****VF： 温度阈值设定（*****最高位为符号位，后四位为数据位，例如:VFS1A+25.5VF,表示25摄氏度）
VFS2A*****VF： 湿度阈值设定（*****前三位为无效位，后两位为数据位，例如:VFS2A00040VF,表示40%RH）



上传温度数据格式设置如下：
VFD1A*****VF：温度数据上报（*****最高位为符号位，后四位为数据位，例如:VFD1A+25.5VF,表示25摄氏度）	 	VALUE/BOOL
VFD2A*****VF：温度数据上报（*****前三位为无效位，后两位为数据位，例如:VFD2A00040VF,表示40%RH）



+IPD,0,10:CRD0AONCR

+IPD,0,10:CRD0AOFCR

+IPD,0,13:VFS1A+25.0VF
+IPD,0,13:VFS2A00055VF

+IPD,0,11:CRALM2ONCR

+IPD,0,11:CRALM2OFCR
+IPD,0,11:CRALM1ONCR

+IPD,0,11:CRALM1OFCR
