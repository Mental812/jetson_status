import json
import sys
import time
from utils.get_system_value_class import get_system_value

get_info_class = get_system_value()

def get_MBInfo():
    board,SN = get_info_class.get_MBInfo()
    data = {"Manu": "Aetina Corporation.",
    "Product": "{Board_name}".format(Board_name= board),
    "SN": "{SN_Number}".format(SN_Number = SN),
    "BIOSManu": "{BIOSM}".format(BIOSM = "Aetina Corporation."),
    "BIOSVer": "{BIOSV}".format(BIOSV = "Aetina_0010101")}

    return data

def get_GPUInfo():
    spec = get_info_class.get_GPUInfo()
    data = {"Name": "{Name}".format(Name = "Nvidia Jetson " + spec[0]),
    "Arch": "{Arch}".format(Arch = spec[1]),
    "DriverVer": "{DriverVer}".format(DriverVer = spec[2]),
    "ComputeCap": "{Com}".format(Com = spec[3]),  #CUDA Compute Capability
    "CoreNum": "{Core}".format(Core = spec[4]),
    "MemType": "{MemT}".format(MemT = spec[5]),
    "MemBusWidth": "{MemBW1}".format(MemBW1 = spec[6]),
    "MemSize": "{MemS}".format(MemS = spec[7]),
    "MemBandWidth": "{MemBW2}".format(MemBW2= spec[8]),
    "Clock": "{Clock}".format(Clock = spec[9]),
    "MemClock": "{MemC}".format(MemC = spec[10])}

    return data

def get_CPUDyInfo():
    CPU_Number,CPU_ON_Number,CPU_Frequency,CPU_Usage ,Temp ,FanRPM = get_info_class.get_CPUDyInfo()
    #print(CPU_Number,CPU_ON_Number,CPU_Frequency,CPU_Usage)       
    data = {"CPU": 
    {"{CPU1}".format(CPU1 = "0"): {
        "Freq": CPU_Frequency[0],
        "Usage": CPU_Usage[0],
        "Temp": Temp,
        "V": 9,
        },
        "{CPU2}".format(CPU2 = "1"): {
        "Freq": CPU_Frequency[1],
        "Usage": CPU_Usage[1],
        "Temp": Temp,
        "V": 8,
        },
        "{CPU3}".format(CPU3 = "2"): {
        "Freq": CPU_Frequency[2],
        "Usage": CPU_Usage[2],
        "Temp": Temp,
        "V": 8,
        },
        "{CPU4}".format(CPU4 = "3"): {
        "Freq": CPU_Frequency[3],
        "Usage": CPU_Usage[3],
        "Temp": Temp,
        "V": 8,
        },
        "{CPU5}".format(CPU5 = "4"): {
        "Freq": CPU_Frequency[4],
        "Usage": CPU_Usage[4],
        "Temp": Temp,
        "V": 8,
        },
        "{CPU6}".format(CPU6 = "5"): {
        "Freq": CPU_Frequency[5],
        "Usage": CPU_Usage[5],
        "Temp": Temp,
        "V": 8,
        },
        "FanRPM": FanRPM
        }
    }

    return data

def get_GPUDyInfo():
    Clock,Temp,MemU,Load ,FanTemp= get_info_class.get_GPUDyInfo()
    data = {"CoreClock": Clock,
    "Temp": Temp,
    "MemUsed": MemU,
    "Load": Load,
    "FanTemp": FanTemp}
    
    return data


def main():
    try:
        argv_str = sys.argv[1]
    except:
        argv_str = "test"

    if argv_str == "MBInfo":
        data = get_MBInfo()
    elif argv_str == "GPUInfo":
        data = get_GPUInfo()
    elif argv_str == "CPUDyInfo":
        data = get_CPUDyInfo()
    elif argv_str == "GPUDyInfo":
        data = get_GPUDyInfo()
    elif argv_str == "test":
        data  = 'Please Check argv'
    else:
        data = "parameter error"
    data_json = json.dumps(data)
    print(data_json)

if __name__ == "__main__":
    main()
