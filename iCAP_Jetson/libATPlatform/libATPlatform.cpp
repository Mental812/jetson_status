#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <iostream>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "include/libATPlatform.h"

using namespace std;

#define LIB_VERSION					"2.0.0.0"

#define FLOAT_TO_INT(x)				((x)>=0?(int)((x)+0.5):(int)((x)-0.5))
#define MAX_CMD						1024
#define MAX_PATH					256
#define MAX_CARRIERBOARD_AVL_NUMBER	4
#define MAX_SOFTWARE_AVL_NUMBER		5
#define MAX_SENSOR_AVL_NUMBER		1
#define TX2_MEMORY_TOTALSIZE		7844
#define AETINA_N310_LM75_BUS		0
#define AETINA_N310_LM75_SLAVE		0x48
#define EEPROM_I2C_BUS				7
#define EEPROM_I2C_SLAVE			0x57
#define EEPROM_CONTENT				"Aetina Corporation"


/*------------------------------------------------------------------------------------
								Module default name
------------------------------------------------------------------------------------*/
#define DEFAULT_TX1_NAME			"NVIDIA Jetson TX1"
#define DEFAULT_TX2_NAME			"NVIDIA Jetson TX2"
#define DEFAULT_XAVIER_NAME			"NVIDIA Jetson AGX Xavier"
#define DEFAULT_NANO_NAME			"NVIDIA Jetson Nano"

/*------------------------------------------------------------------------------------
								TX2 Spec
------------------------------------------------------------------------------------*/
#define TX2_GPU_ARCHITECTURE		"NVIDIA Pascal™"
#define TX2_GPU_CUDA_CORES			"256"
#define TX2_MEMORY_TYPE				"LPDDR4"
#define TX2_MEMORY_SIZE				"8 GB"

/*------------------------------------------------------------------------------------
								Carrier board default name
------------------------------------------------------------------------------------*/
#define AETINA_CBNAME_AN310			"AN310"
#define AETINA_CBNAME_ACEN622		"ACE-N622"
#define AETINA_CBNAME_ACEN510		"ACE-N510"
#define AETINA_CBNAME_AX710			"AX710"

/*------------------------------------------------------------------------------------
								Software default name
------------------------------------------------------------------------------------*/
#define DEFAULT_NAME_CUDA			"CUDA Toolkit for L4T"
#define DEFAULT_NAME_CUDNN			"cuDNN"
#define DEFAULT_NAME_TENSORRT		"TensorRT"
#define DEFAULT_NAME_OPENCV			"OpenCV"
#define DEFAULT_NAME_VISIONWORKS	"VisionWorks"

/*------------------------------------------------------------------------------------
								Sensor default name
------------------------------------------------------------------------------------*/
#define DEFAULT_NAME_LM75			"LM75(Temperature)"

/*------------------------------------------------------------------------------------
								Parser tag
------------------------------------------------------------------------------------*/
#define CPU_LOADING_TAG				"CPU ["
#define GPU_LOADING_TAG 			"GR3D_FREQ "
#define GPU_TEMPERATURE_TAG			"GPU@"
#define GPU_MEMORY_USED_TAG 		"RAM "
#define GPU_FAN_TEMPERATURE_TAG 	"thermal@"
#define AETINA_CARRIERBOARD_510		"510"
#define AETINA_CARRIERBOARD_310		"310"
#define AETINA_CARRIERBOARD_62X		"62X"
#define AETINA_CARRIERBOARD_710		"710"

enum JETSON_MODULE_ID{
	TK1_ID		= 64,
	TX1_ID		= 33,
	TX2_ID		= 24,
	XAVIER_ID	= 25,
};

typedef struct tagATP{
	CALLBACK_LOG_FUNC 	log_func;
}ATP_OBJ;

static ATP_OBJ g_ATPObj = {0};

int SystemCMD(char *szCMD){
	char szMsg[MAX_CMD] = {0};
	int status = 0;
	if(szCMD == NULL)
	{
		g_ATPObj.log_func("szCMD is null.");
		return -EINVAL;
	}
#ifdef _DEBUG_
	else{
		g_ATPObj.log_func(szCMD);
	}
#endif

	status = system(szCMD);
	if(status < 0)
	{
		sprintf(szMsg, "szCMD : %s error : %s", szCMD, strerror(errno));
		g_ATPObj.log_func(szMsg);
		return -EINVAL;
	}

	if(WIFEXITED(status))
	{
#ifdef _DEBUG_
		sprintf(szMsg, "normal termination, exit status = %d", WEXITSTATUS(status));
		g_ATPObj.log_func(szMsg);
#endif
	}
	else if(WIFSIGNALED(status))
	{
		sprintf(szMsg, "abnormal termination,signal number =%d\n", WTERMSIG(status));
		g_ATPObj.log_func(szMsg);
	}
	else if(WIFSTOPPED(status))
	{
		sprintf(szMsg, "process stopped, signal number =%d\n", WSTOPSIG(status));
		g_ATPObj.log_func(szMsg);
	}

	return OK;
}

/*------------------------------------------------------------------------------------
									inline function
------------------------------------------------------------------------------------*/

void getL4TVersion(char *pszVersion){
	char szResult[MAX_PATH] = {0}, szMajor[MAX_PATH] = {0}, szMinor[MAX_PATH] = {0};
	FILE *pFile = popen("head -n 1 /etc/nv_tegra_release", "r");
	if(pFile == NULL){
		return;
	}

	if(fgets(szResult, MAX_PATH, pFile)){
		strncpy(szMajor, strstr(szResult, "R") + 1, strstr(szResult, " (") - strstr(szResult, "R") - 1);
		strncpy(szMinor, strstr(szResult, "REVISION: ") + strlen("REVISION: "), strstr(szResult, ", GCID:") - (strstr(szResult, "REVISION: ") + strlen("REVISION: ")));
	}
	pclose(pFile);

	if(strlen(szMajor) > 0 && strlen(szMinor) > 0)
		sprintf(pszVersion, "%s.%s", szMajor, szMinor);
}

int getModuleType(){
	int nModuleType = UNKNOWN;
	char szCMD[MAX_PATH] = {0};
	FILE *pFile = popen("cat /sys/module/tegra_fuse/parameters/tegra_chip_id", "r");
	if(pFile == NULL){
		g_ATPObj.log_func("cat /sys/module/tegra_fuse/parameters/tegra_chip_id failed.");
		return -FAILED;
	}

	if(fgets(szCMD, MAX_PATH, pFile)){
		switch(atoi(szCMD)){
			case TK1_ID:
				nModuleType = TK1;
				break;
			case TX1_ID:
				nModuleType = TX1;
				break;
			case TX2_ID:
				nModuleType = TX2;
				break;
			case XAVIER_ID:
				nModuleType = XAVIER;
				break;
		}
	}
	pclose(pFile);
	
	return nModuleType;
}

bool IsCUDAInstalled(){
	char szCMD[MAX_PATH] = {0};
	bool bInstalled = false;
	FILE *fp = popen("whereis cuda", "r");
	if(fp == NULL){
		g_ATPObj.log_func("whereis cuda failed.");
		return false;
	}

	if(fgets(szCMD, MAX_PATH, fp)){
		if(strstr(szCMD, "/"))
			bInstalled = true;
	}
	pclose(fp);

	return bInstalled;
}

bool IsCUDNNInstalled(){
	char szCMD[MAX_PATH] = {0};
	bool bInstalled = false;
	FILE *fp = popen("whereis cudnn", "r");
	if(fp == NULL){
		g_ATPObj.log_func("whereis cudnn failed.");
		return false;
	}

	if(fgets(szCMD, MAX_PATH, fp)){
		if(strstr(szCMD, "/"))
			bInstalled = true;
	}
	pclose(fp);

	return bInstalled;
}

bool IsOPENCVInstalled(){
	char szCMD[MAX_PATH] = {0};
	bool bInstalled = false;
	FILE *fp = popen("whereis opencv", "r");
	if(fp == NULL){
		g_ATPObj.log_func("whereis opencv failed.");
		return false;
	}

	if(fgets(szCMD, MAX_PATH, fp)){
		if(strstr(szCMD, "/"))
			bInstalled = true;
	}
	pclose(fp);

	return bInstalled;
}

bool IsVISIONWORKSInstalled(){
	char szCMD[MAX_PATH] = {0};
	bool bInstalled = false;
	FILE *fp = popen("whereis visionworks", "r");
	if(fp == NULL){
		g_ATPObj.log_func("whereis visionworks failed.");
		return false;
	}

	if(fgets(szCMD, MAX_PATH, fp)){
		if(strstr(szCMD, "/"))
			bInstalled = true;
	}
	pclose(fp);

	return bInstalled;
}

bool IsTENSORRTInstalled(){
	char szCMD[MAX_PATH] = {0};
	bool bInstalled = false;
	FILE *fp = popen("dpkg -l |grep tensorrt", "r");
	if(fp == NULL){
		g_ATPObj.log_func("dpkg -l |grep tensorrt failed.");
		return false;
	}

	if(fgets(szCMD, MAX_PATH, fp)){
		if(strstr(szCMD, "/"))
			bInstalled = true;
	}
	pclose(fp);

	return bInstalled;
}

bool IsSoftwareInsatlled(int nAVLId){
	switch(nAVLId){
		case CUDA:
			return IsCUDAInstalled();
		case CUDNN:
			return IsCUDNNInstalled();
		case TENSORRT:
			return IsTENSORRTInstalled();
		case OPENCV:
			return IsTENSORRTInstalled();
		case VISIONWORKS:
			return IsVISIONWORKSInstalled();
		default:
			return false;
	}
}

char* getSoftwareDefaultName(int nAVLId){
	switch(nAVLId){
		case CUDA:
			return DEFAULT_NAME_CUDA;
		case CUDNN:
			return DEFAULT_NAME_CUDNN;
		case TENSORRT:
			return DEFAULT_NAME_TENSORRT;
		case OPENCV:
			return DEFAULT_NAME_OPENCV;
		case VISIONWORKS:
			return DEFAULT_NAME_VISIONWORKS;
		default:
			return NULL;
	}
}

int i2c_read_data(int i2c_bus_id, char i2c_address, char* buffer){

	int file = 0;
	char i2c_bus[40] = {0};
	sprintf(i2c_bus, "/dev/i2c-%d", i2c_bus_id);
	if ((file = open(i2c_bus, O_RDWR)) < 0){
		return -INVALID;
	}

	if (ioctl(file, I2C_SLAVE_FORCE, i2c_address) < 0){
		close(file);
		return -INVALID;
	}

	char reg[1] = {0x00};
	write(file, reg, 1);
	if (read(file, buffer, sizeof(buffer)) != sizeof(buffer)){
		printf("Error : Input/Output error \n");
	}

	close(file);
	return OK;
}

int getLM750Value(){
	char buffer[2] = {0};
	int nResult = i2c_read_data(AETINA_N310_LM75_BUS, AETINA_N310_LM75_SLAVE, buffer);
	if (nResult != 0){
			return FAILED;
	}
	
	int conbined_data = (buffer[0] * 256 + (buffer[1] & 0xF0)) / 16;
	if (conbined_data > 2047){
			conbined_data -= 4096;
	}
	return conbined_data * 0.0625;
}

bool IsLM75OnBoard(){
	char buffer[2] = {0};
	int nResult = i2c_read_data(AETINA_N310_LM75_BUS, AETINA_N310_LM75_SLAVE, buffer);
	if (nResult != 0){
		return false;
	}
	return true;
}

bool IsSensorOnBoard(int nAVLId){	
	switch(nAVLId){
		case LM75:
			return IsLM75OnBoard();
		default:
			return false;
	}
}

char* getSensorDefaultName(int nAVLId){
	switch(nAVLId){
		case LM75:
			return DEFAULT_NAME_LM75;
		default:
			return NULL;
	}
}

void getCPUsInfo(char *pszData, CPUList *pCPUList){
	char szCPUsInfo[MAX_PATH] = {0};
	char szCPULoading[MAX_PATH] = {0};
	int nId = 0;
	char* temp = strstr(pszData, CPU_LOADING_TAG);
	if(temp){
		strncpy(szCPUsInfo, temp + strlen(CPU_LOADING_TAG), strstr(temp, "]") - (temp + strlen(CPU_LOADING_TAG)));
		if(strlen(szCPUsInfo)){
			//CPU0 parser
			char *pszCPUContent = strtok(szCPUsInfo, ",");
			if(pszCPUContent){
				pCPUList[nId].nID = nId;
				if(strstr(pszCPUContent, "off"))
					memset(szCPULoading, 0, MAX_PATH);
				else
					strncpy(szCPULoading, pszCPUContent, strstr(pszCPUContent, "%@") - pszCPUContent);
				pCPUList[nId].nLoading = atoi(szCPULoading);
			}
			
			nId++;

			//CPU1 parser
			pszCPUContent = strtok(NULL, ",");
			if(pszCPUContent){
				pCPUList[nId].nID = nId;
				if(strstr(pszCPUContent, "off"))
					memset(szCPULoading, 0, MAX_PATH);
				else
					strncpy(szCPULoading, pszCPUContent, strstr(pszCPUContent, "%@") - pszCPUContent);
				pCPUList[nId].nLoading = atoi(szCPULoading);
			}

			nId++;

			//CPU2 parser
			pszCPUContent = strtok(NULL, ",");
			if(pszCPUContent){
				pCPUList[nId].nID = nId;
				if(strstr(pszCPUContent, "off"))
					memset(szCPULoading, 0, MAX_PATH);
				else
					strncpy(szCPULoading, pszCPUContent, strstr(pszCPUContent, "%@") - pszCPUContent);
				pCPUList[nId].nLoading = atoi(szCPULoading);
			}

			nId++;

			//CPU3 parser
			pszCPUContent = strtok(NULL, ",");
			if(pszCPUContent){
				pCPUList[nId].nID = nId;
				if(strstr(pszCPUContent, "off"))
					memset(szCPULoading, 0, MAX_PATH);
				else
					strncpy(szCPULoading, pszCPUContent, strstr(pszCPUContent, "%@") - pszCPUContent);
				pCPUList[nId].nLoading = atoi(szCPULoading);
			}

			nId++;

			//CPU4 parser
			pszCPUContent = strtok(NULL, ",");
			if(pszCPUContent){
				pCPUList[nId].nID = nId;
				if(strstr(pszCPUContent, "off"))
					memset(szCPULoading, 0, MAX_PATH);
				else
					strncpy(szCPULoading, pszCPUContent, strstr(pszCPUContent, "%@") - pszCPUContent);
				pCPUList[nId].nLoading = atoi(szCPULoading);
			}

			nId++;

			//CPU5 parser
			pszCPUContent = strtok(NULL, ",");
			if(pszCPUContent){
				pCPUList[nId].nID = nId;
				if(strstr(pszCPUContent, "off"))
					memset(szCPULoading, 0, MAX_PATH);
				else
					strncpy(szCPULoading, pszCPUContent, strstr(pszCPUContent, "%@") - pszCPUContent);
				pCPUList[nId].nLoading = atoi(szCPULoading);
			}
		}
	}
}

int getGPULoadingValue(char *pszData){
	char szGPU_Loading[MAX_PATH] = {0};
	char *pszGPULoading = strstr(pszData, GPU_LOADING_TAG);
	if(pszGPULoading)
		strncpy(szGPU_Loading, pszGPULoading + strlen(GPU_LOADING_TAG), strchr(pszGPULoading, '%') - (pszGPULoading + strlen(GPU_LOADING_TAG)));

	return atoi(szGPU_Loading);
}

int getTemperature(char *pszData){
	char szTemperature[MAX_PATH] = {0};
	char *pszTemperature = strstr(pszData, GPU_FAN_TEMPERATURE_TAG);
	if(pszTemperature)
		strncpy(szTemperature, pszTemperature + strlen(GPU_FAN_TEMPERATURE_TAG), strchr(pszTemperature, '.') - (pszTemperature + strlen(GPU_FAN_TEMPERATURE_TAG)));

	return atoi(szTemperature);
}

int getMemLoadingValue(char *pszData){
	float fValue = 0;
	char szMemory_Loading[MAX_PATH] = {0}, szMsg[MAX_PATH] = {0};
	char *pszMemLoading = strstr(pszData, GPU_MEMORY_USED_TAG);
	if(pszMemLoading){
		strncpy(szMemory_Loading, pszMemLoading + strlen(GPU_MEMORY_USED_TAG), strchr(pszMemLoading, '/') - (pszMemLoading + strlen(GPU_MEMORY_USED_TAG)));
		fValue = (float)(atoi(szMemory_Loading)) / TX2_MEMORY_TOTALSIZE * 100.0;
	}

	sprintf(szMsg, "%lf", fValue);

	return FLOAT_TO_INT(fValue);
}



int	readEEPROM(char *pszContent){
	char szData[10] = {0}, szCMD[MAX_CMD] = {0};
	int fd = 0;

	if(pszContent == NULL)
		return -INVALID;

	sprintf(szCMD, "/dev/i2c-%d", EEPROM_I2C_BUS);
	fd = open(szCMD,O_RDWR);
	if(fd<0)
	{
		return -FAILED;
	}

	if (ioctl(fd , I2C_SLAVE, EEPROM_I2C_SLAVE) < 0) {
    	/* ERROR HANDLING; you can check errno to see what went wrong */
		close(fd);
    	return -FAILED;
	}

	if(pszContent){
		for(int n = 0 ; n < strlen(EEPROM_CONTENT) ; n++){
			szData[0] = n;  //eeprom address in Page 0
			if(write(fd, szData, 1) != 1)
			{
				/* ERROR HANDLING: i2c transaction failed */
				printf("Failed to write to the i2c bus.\n");
			}
			//now if the eeprom points byte which You want to read, do random read sequence:
			if(read(fd, pszContent + n, 1) != 1)  //now we know what address will be read:
			{
				/* ERROR HANDLING: i2c transaction failed */
				printf("Failed to read from the i2c bus.\n");
			}
		}
	}
	
	close(fd);

	return OK;
}

int getCarrierBoardId(){
	int nId = UNKNOWN;
	char szCMD[MAX_PATH] = {0};
	FILE *fp = popen("dmesg |grep dts", "r");
	if(fp == NULL){
		g_ATPObj.log_func("dmesg |grep dts failed.");
		return false;
	}

	if(fgets(szCMD, MAX_PATH, fp)){
		if(strstr(szCMD, AETINA_CARRIERBOARD_310))
			nId = AN310;
		else if (strstr(szCMD, AETINA_CARRIERBOARD_510))
			nId = AN510;
		else if (strstr(szCMD, AETINA_CARRIERBOARD_62X))
			nId = AN62X;
		else if(strstr(szCMD, AETINA_CARRIERBOARD_710))
			nId = AX710;		
	}
	pclose(fp);

	return nId;
}

char* getCarrierBoardName(int nId){
	switch(nId){
		case AN310:
			return AETINA_CBNAME_AN310;
		case AN510:
			return AETINA_CBNAME_ACEN510;
		case AN62X:
			return AETINA_CBNAME_ACEN622;
		case AX710:
			return AETINA_CBNAME_AX710;
		default:
			return "";
	}
}

/*------------------------------------------------------------------------------------
								 	export function
------------------------------------------------------------------------------------*/
void ATP_SetLogFunc(CALLBACK_LOG_FUNC func){
	if(func != NULL)
		g_ATPObj.log_func = func;
}

bool ATP_IsAetinaPlatform(){
	char szContent[MAX_PATH] = {0};
	if(readEEPROM(szContent) != OK)
		return false;
	
	return strcmp(szContent, EEPROM_CONTENT) == 0 ? true : false;
}

int	ATP_GetPlatformInfo(PlatformInfo *pInfo){

	if(ATP_IsAetinaPlatform() == false)
		return -NON_ATP;

	if(pInfo == NULL)
		return -INVALID;

	pInfo->nModuleType = getModuleType();
	sprintf(pInfo->szName, "%s", DEFAULT_TX2_NAME);
	sprintf(pInfo->szCarrierBoard, "%s", getCarrierBoardName(getCarrierBoardId()));
	sprintf(pInfo->szArchitecture, "%s", TX2_GPU_ARCHITECTURE);	
	sprintf(pInfo->szCUDACores, "%s", TX2_GPU_CUDA_CORES);
	sprintf(pInfo->szMemoryType, "%s", TX2_MEMORY_TYPE);
	sprintf(pInfo->szMemorySize, "%s", TX2_MEMORY_SIZE);
	getL4TVersion(pInfo->szL4TVer);

	return OK;
}

int	ATP_GetDynamicInfo(InfoList *pInfoList){

	if(ATP_IsAetinaPlatform() == false)
		return -NON_ATP;

	if(pInfoList == NULL)
		return -INVALID;
	
	FILE *pFile = popen("tegrastats", "r");
	if(pFile){
		char szResult[4096] = {0};
		if(fgets(szResult, 4096, pFile)){
			getCPUsInfo(szResult, pInfoList->stCPUList);
			pInfoList->nGPULoading = getGPULoadingValue(szResult);
			pInfoList->nMemLoading = getMemLoadingValue(szResult);
			pInfoList->nTemperature = getTemperature(szResult);
		}
		pclose(pFile);		
	}

	return OK;
}

int ATP_GetSWNumber(){
	
	if(ATP_IsAetinaPlatform() == false)
		return -NON_ATP;
	
	int nSWNumber = 0;
	
	for(int nId = 0 ; nId < MAX_SOFTWARE_AVL_NUMBER ; nId++){
		if(IsSoftwareInsatlled(nId))
			nSWNumber++;
	}
	return nSWNumber;
}

int	ATP_GetSWInfo(SoftwareList *pSWList){
	int nCount = 0;
	if(ATP_IsAetinaPlatform() == false)
		return -NON_ATP;

	if(pSWList == NULL)
		return -INVALID;

	for(int nId = 0 ; nId < MAX_SOFTWARE_AVL_NUMBER ; nId++){		
		if(IsSoftwareInsatlled(nId)){
			pSWList[nCount].nAVLId = nId;
			sprintf(pSWList[nCount].szName, "%s", getSoftwareDefaultName(nId));
			nCount++;
		}
	}

	return OK;
}

int ATP_GetSensorNumber(){

	if(ATP_IsAetinaPlatform() == false)
		return -NON_ATP;

	int nSensorNumber = 0;
	for(int nId = 0 ; nId < MAX_SENSOR_AVL_NUMBER ; nId++){
		if(IsSensorOnBoard(nId))
			nSensorNumber++;
	}

	return nSensorNumber;
}

int	ATP_GetSensorInfo(SensorList *pSensorList){

	if(ATP_IsAetinaPlatform() == false)
		return -NON_ATP;

	if(pSensorList == NULL)
		return -INVALID;

	for(int nId = 0 ; nId < MAX_SENSOR_AVL_NUMBER ; nId++){		
		if(IsSensorOnBoard(nId)){
			pSensorList[nId].nAVLId = nId;
			sprintf(pSensorList[nId].szName, "%s", getSensorDefaultName(nId));

			switch(nId){
				case LM75:
					pSensorList[nId].nValue = getLM750Value();
					break;
			}

		}
	}

	return OK;
}

int	ATP_SetModelFile(char *pszFilePath){

	if(ATP_IsAetinaPlatform() == false)
		return -NON_ATP;

	g_ATPObj.log_func(pszFilePath);
	return OK;
}

//For iCAP
int	ATP_GetMBInfo(char* buffer, size_t num=1024){
	FILE *pFile = popen("Jetson_status MBInfo", "r");
	if(pFile){
		char szResult[4096] = {0};
		if(fgets(szResult, 4096, pFile)){
			if(strstr(szResult,"MB")){
				sprintf(buffer,szResult);
			}
			else{
				string temp = "{\"MB\": {\"Manu\": \"Aetina Corporation.\",\"Product\": \"ACE-N310\",\"SN\": \"01210170531111\",\"BIOSManu\": \"Aetina Corporation.\",\"BIOSVer\": \"ALASKA - 1072009\"}}";		sprintf(buffer, "%s", temp.c_str());
				sprintf(buffer,"%s",temp.c_str());
			}
		}
	pclose(pFile);
	}
	return OK;
}
	
int	ATP_GetGPUInfo(char* buffer, size_t num=1024){
	FILE *pFile = popen("Jetson_status GPUInfo", "r");
	if(pFile){
		char szResult[4096] = {0};
		if(fgets(szResult, 4096, pFile)){
			if(strstr(szResult,"GPU")){
				sprintf(buffer,szResult);
			}
			else{
				string temp = "{\"GPU\": {\"Name\": \"NVIDIA Tegra X2\",\"Arch\": \"NVIDIA PascalTM\",\"DriverVer\": \"9.0\",\"ComputeCap\": \"6.2\",\"CoreNum\": \"256\",\"MemType\": \"LDDR4\",\"MemBusWidth\": \"128-bit\",\"MemSize\": \"7846 MB\",\"MemBandWidth\": \"59.7 GB/s\",\"Clock\": \"1301 MHz\",\"MemClock\": \"1600MHz\"}}";
				sprintf(buffer,"%s",temp.c_str());
			}
		}
	pclose(pFile);
	}
	return OK;
}

int	ATP_GetCPUDyInfo(char* buffer, size_t num=1024){
	FILE *pFile = popen("Jetson_status CPUDyInfo", "r");
	if(pFile){
		char szResult[4096] = {0};
		if(fgets(szResult, 4096, pFile)){
			if(strstr(szResult,"CPU")){
				sprintf(buffer,szResult);
			}
			else{
				string temp = "{\"CPU\": {\"0\": {\"Freq\": 1206,\"Usage\": 52,\"Temp\": 21,\"V\": 9,},\"1\": {\"Freq\": 1057,\"Usage\": 83,\"Temp\": 41,\"V\": 8,},\"Freq\": 1663,\"Usage\": 16,\"FanRPM\": 2486}}";			
				sprintf(buffer,"%s",temp.c_str());
			}
		}
	pclose(pFile);
	}
	return OK;
}

int	ATP_GETGPUDyInfo(char* buffer, size_t num=1024){
	FILE *pFile = popen("Jetson_status GPUDyInfo", "r");
	if(pFile){
		char szResult[4096] = {0};
		if(fgets(szResult, 4096, pFile)){
			if(strstr(szResult,"GPU")){
				sprintf(buffer,szResult);
			}
			else{
				string temp ="{\"GPU\": {\"CoreClock\": 907,\"Temp\": 5,\"MemUsed\": 93,\"Load\": 1,\"FanTemp\": 12}";
				sprintf(buffer,"%s",temp.c_str());
			}
		}
	pclose(pFile);
	}
	return OK;
}

