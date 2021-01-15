#ifndef _LIBATPLATFORM_
#define _LIBATPLATFORM_

#define ATP_BUFFER_SIZE 	32

/****************************************
 *
 * Enums
 *
 ****************************************/
enum ERROR_CODE{
	OK,
	NON_ATP,
	INVALID,
	FAILED,
	UNKNOWN
};

enum MODULE_ID{	
	TK1,
	TX1,
	TX2,	
	XAVIER
};

enum CARRIERBOARD_ID{
	AN310,
	AN510,
	AN62X,
	AX710
};

enum SOFTWARE_AVL{
	CUDA,
	CUDNN,
	TENSORRT,
	OPENCV,	
	VISIONWORKS
};

enum SENSOR_AVL{
	LM75
};

/****************************************
 *
 * Sturctures
 *
 ****************************************/
typedef struct tagSoftwareList{
	int 			nAVLId;
	char			szName[ATP_BUFFER_SIZE];
}SoftwareList;

typedef struct tagSensorList{
	int				nAVLId;
	char			szName[ATP_BUFFER_SIZE];
	int				nValue;
}SensorList;

typedef struct tagPlatformInfo{
	int				nModuleType;
	char 			szName[ATP_BUFFER_SIZE];
	char 			szCarrierBoard[ATP_BUFFER_SIZE];
	char 			szArchitecture[ATP_BUFFER_SIZE];
	char 			szCUDACores[ATP_BUFFER_SIZE];
	char 			szMemoryType[ATP_BUFFER_SIZE];
	char 			szMemorySize[ATP_BUFFER_SIZE];
	char			szL4TVer[ATP_BUFFER_SIZE];
}PlatformInfo;

typedef struct tagCPUList{
	int				nID;
	int				nLoading;
}CPUList;

typedef struct tagInfoList{
	CPUList			stCPUList[6];
	int 			nGPULoading;
	int				nMemLoading;
	int 			nTemperature;
}InfoList;

/****************************************
 *
 * APIs of Library
 *
 ****************************************/
typedef void 		(*CALLBACK_LOG_FUNC)(char *szMsg);

#ifdef __cplusplus
extern "C"{
		void		ATP_SetLogFunc(CALLBACK_LOG_FUNC func);
		bool 		ATP_IsAetinaPlatform();
		int			ATP_GetPlatformInfo(PlatformInfo *pInfo);
		int			ATP_GetDynamicInfo(InfoList *pInfo);
		int			ATP_GetSWNumber();
		int			ATP_GetSWInfo(SoftwareList *pSWList);
		int 		ATP_GetSensorNumber();
		int			ATP_GetSensorInfo(SensorList *pSensorList);
		int			ATP_SetModelFile(char *pszFilePath);

//For iCAP
		int			ATP_GetMBInfo(char* buffer, size_t num=1024);
		int			ATP_GetGPUInfo(char* buffer, size_t num=1024);
		int			ATP_GetCPUDyInfo(char* buffer, size_t num=1024);
		int			ATP_GETGPUDyInfo(char* buffer, size_t num=1024);
}
#else
extern	void		ATP_SetLogFunc(CALLBACK_LOG_FUNC func);
extern	bool 		ATP_IsAetinaPlatform();
extern	int			ATP_GetPlatformInfo(PlatformInfo *pInfo);
extern	int			ATP_GetDynamicInfo(InfoList *pInfo);
extern	int			ATP_GetSWNumber();
extern	int			ATP_GetSWInfo(SoftwareList *pSWList);
extern	int 		ATP_GetSensorNumber();
extern	int			ATP_GetSensorInfo(SensorList *pSensorList);
extern	int			ATP_SetModelFile(char *pszFilePath);
#endif

#endif
