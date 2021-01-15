#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "include/libATPlatform.h"

#define MAX_PATH 256

void LOG(char *szMsg){
	char szLogMsg[1024] = {0};
	sprintf(szLogMsg, "%s\n", szMsg);
	printf("%s", szLogMsg);
}

int main(void){
	char szPath[MAX_PATH] = {0};
	char szChar = 0;
	bool bIsStop = false;
	bool bMenuIsShow = false;

	ATP_SetLogFunc(LOG);

	while (!bIsStop){
		if(!bMenuIsShow){
			LOG("[A] ATP_IsAetinaPlatform.");
			LOG("[B] ATP_GetPlatformInfo.");
			LOG("[C] ATP_GetSWInfo.");
			LOG("[D] ATP_GetSensorInfo.");
			LOG("[E] ATP_SetModelFile.");
			LOG("[F] ATP_GetDynamicInfo.");
			LOG("[G] ATP_GetMBInfo.");
			LOG("[H] ATP_GetGPUInfo.");
			LOG("[I] ATP_GetCPUDyInfo.");
			LOG("[J] ATP_GETGPUDyInfo.");						
			LOG("[Q] Quit.");
			bMenuIsShow = true;
		}

		switch(getchar()){
			case 'A':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_IsAetinaPlatform...");

					if(ATP_IsAetinaPlatform())
						LOG("It's aetina platform.");
					else
						LOG("It's NOT aetina platform.");
						
					LOG("ATP_IsAetinaPlatform...done");
					LOG("----------------------------------------------------------------");
					bMenuIsShow = false;
				}
				break;

			case 'B':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GetPlatformInfo...");
					char szMsg[1024] = {0};
					PlatformInfo pInfo = {0};
					if(ATP_GetPlatformInfo(&pInfo) == OK){
						sprintf(szMsg, "ModuleType : %d, \nName : %s, \nCarrierBoard : %s, \nArchitecture : %s, \nCUDA Cores : %s, \nMemoryType : %s, \nMemorySize : %s, \nL4T Version %s", pInfo.nModuleType, pInfo.szName, pInfo.szCarrierBoard, pInfo.szArchitecture, pInfo.szCUDACores, pInfo.szMemoryType, pInfo.szMemorySize, pInfo.szL4TVer);
						LOG(szMsg);
					}
					else{
						LOG("ATP_GetPlatformInfo failed.");
					}
					
					LOG("ATP_GetPlatformInfo...done");
					LOG("----------------------------------------------------------------");
					bMenuIsShow = false;
				}
				break;

			case 'C':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GetSWInfo...");

					char szMsg[MAX_PATH] = {0};
					int nSWNumber = ATP_GetSWNumber();
					SoftwareList *pswlist = (SoftwareList*)malloc(sizeof(SoftwareList) * nSWNumber);
					if(pswlist){
						memset(pswlist, 0, sizeof(SoftwareList) * nSWNumber);
						if(ATP_GetSWInfo(pswlist) == OK){
							for(int n = 0 ; n < nSWNumber ; n++){
								sprintf(szMsg, "[%d]%s", pswlist[n].nAVLId, pswlist[n].szName);
								LOG(szMsg);
							}
						}
						else{
							LOG("ATP_GetSWInfo failed.");
						}

						free(pswlist);
					}

					LOG("ATP_GetSWInfo...done");
					LOG("----------------------------------------------------------------");
					bMenuIsShow = false;
				}
				break;

			case 'D':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GetSensorInfo...");

					char szMsg[MAX_PATH] = {0};
					int nSensorNumber = ATP_GetSensorNumber();
					SensorList *pSensorList = (SensorList*)malloc(sizeof(SensorList) * nSensorNumber);
					if(pSensorList){
						memset(pSensorList, 0, sizeof(SensorList) * nSensorNumber);
						if(ATP_GetSensorInfo(pSensorList) == OK){
							for(int n = 0 ; n < nSensorNumber ; n++){
								sprintf(szMsg, "[%d]%s(%d)", pSensorList[n].nAVLId, pSensorList[n].szName, pSensorList[n].nValue);
								LOG(szMsg);
							}
						}
						else{
							LOG("ATP_GetSensorInfo failed.");
						}
					}

					LOG("ATP_GetSensorInfo...done");
					LOG("----------------------------------------------------------------");
					bMenuIsShow = false;
				}
				break;

			case 'E':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_SetModelFile...");
					
					ATP_SetModelFile("./sample.module");

					LOG("ATP_SetModelFile...done");
					LOG("----------------------------------------------------------------");
					bMenuIsShow = false;
				}
				break;

			case 'F':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GetDynamicInfo...");
					char szMsg[1024] = {0};
					InfoList pInfo = {0};
					if(ATP_GetDynamicInfo(&pInfo) == OK){
						sprintf(szMsg, "CPU[%d] : %d, \nCPU[%d] : %d, \nCPU[%d] : %d, \nCPU[%d] : %d, \nCPU[%d] : %d, \nCPU[%d] : %d, \nGPU Loading : %d, \nMemory Loading : %d, \nTemperature : %d", 
								pInfo.stCPUList[0].nID,
								pInfo.stCPUList[0].nLoading,
								pInfo.stCPUList[1].nID,
								pInfo.stCPUList[1].nLoading,
								pInfo.stCPUList[2].nID,
								pInfo.stCPUList[2].nLoading,
								pInfo.stCPUList[3].nID,
								pInfo.stCPUList[3].nLoading,
								pInfo.stCPUList[4].nID,
								pInfo.stCPUList[4].nLoading,
								pInfo.stCPUList[5].nID,
								pInfo.stCPUList[5].nLoading,
								pInfo.nGPULoading, 
								pInfo.nMemLoading, 
								pInfo.nTemperature);
						LOG(szMsg);
					}
					else{
						LOG("ATP_GetDynamicInfo failed.");
					}
					LOG("ATP_GetDynamicInfo...done");
					LOG("----------------------------------------------------------------");
				}
				break;
			case 'G':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GetMBInfo...");
					char buffer[1024] = {0};				
					if(ATP_GetMBInfo(buffer, (size_t)1024) == OK){
						LOG(buffer);
					}
					else{
						LOG("ATP_GetMBInfo failed.");
					}
					LOG("ATP_GetMBInfo...done");
					LOG("----------------------------------------------------------------");
				}
				break;
			case 'H':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GetGPUInfo...");
					char buffer[1024] = {0};				
					if(ATP_GetGPUInfo(buffer, (size_t)1024) == OK){
						LOG(buffer);
					}
					else{
						LOG("ATP_GetGPUInfo failed.");
					}
					LOG("ATP_GetGPUInfo...done");
					LOG("----------------------------------------------------------------");
				}
				break;
			case 'I':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GetCPUDyInfo...");
					char buffer[1024] = {0};				
					if(ATP_GetCPUDyInfo(buffer, (size_t)1024) == OK){
						LOG(buffer);
					}
					else{
						LOG("ATP_GetCPUDyInfo failed.");
					}
					LOG("ATP_GetCPUDyInfo...done");
					LOG("----------------------------------------------------------------");
				}
				break;
			case 'J':
				{
					LOG("----------------------------------------------------------------");
					LOG("ATP_GETGPUDyInfo...");
					char buffer[1024] = {0};				
					if(ATP_GETGPUDyInfo(buffer, (size_t)1024) == OK){
						LOG(buffer);
					}
					else{
						LOG("ATP_GETGPUDyInfo failed.");
					}
					LOG("ATP_GETGPUDyInfo...done");
					LOG("----------------------------------------------------------------");
				}
				break;								
			case 'Q':
				bIsStop = true;
				break;
		}		
	}

	LOG("Sample finish.");

	return 0;
}
