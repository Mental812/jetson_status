import os 
from jtop import jtop, JtopException
import json

class get_system_value() :
    def __init__(self):
        self.python = 0
        self.__module_name  = self.__get_module_name()
        self.__Board_name = ""
        self.moudule_list = ["TX"]
        self.__get_BSP_Value()
        
    def get_MBInfo(self):
        return self.__Board_name ,self.__get_SERIAL_NUMBER_Val()
    def get_GPUInfo(self):
        self.__get_BSP_Value()
        spec = self.__get_Module_spec()
        return spec
        #return model_name,arch,cuda
    def get_CPUDyInfo(self): 
        return self.__get_CPU_freqency()
    def get_GPUDyInfo(self):
        return self.__get_GPU_freqency()


    def __get_BSP_Value(self):
        with jtop() as jetson:
            try:
                BSP_Name = jetson.board['hardware']['CODENAME']
                #print(BSP_Name)
                BSP_list = BSP_Name.split("_")
                board_name = BSP_list[4]
            
            except:
                module_name = "Not Aetina Boarcd"
                board_name = "UnKnown"

            #self.__module_name = module_name
            self.__Board_name = board_name
    
    def __get_module_name(self):
        module_file = open("/proc/device-tree/model")
        module_text = module_file.read()
        module_list = module_text.split(" ")
        return module_list

    def __get_SERIAL_NUMBER_Val(self):
        with jtop() as jetson:
            try:
                serial_num = jetson.board['hardware']['SERIAL_NUMBER']
            except:
                serial_num = ''
            return serial_num

    def __get_Module_spec(self):
        try:
            if "TX2" in self.__module_name:
                spec = ["TX2","Pascel","10.2","6.2","256","DDR4","128-bit","8 GB","59.7 GB/s","1301 MHz","1600MHz"]
            elif "Nano" in self.__module_name:
                spec = ["Nano","Maxwell","10.2","5.3","128","DDR4","64-bit","4 GB","25.6 GB/s","921.6 MHz","1600MHz"]
            elif "Xavier" in self.__module_name and "NX" in self.__module_name:
                spec = ["Xavier-NX","Volta","10.2","7.2","384","LPDDR4x","128-bit","8 GB","51.2 GB/s","1109 MHz","1109MHz"]
            elif "Jetson-AGX" in self.__module_name:
                spec = ["Xavier","Volta","10.2","7.2","512","LPDDR4x","256-bit","32 GB","136.5 GB/s","1109 MHz","2133MHz"]
            else:
                spec = [" "," "," "," "," "," "," "," "," "," "," "]

            return spec
        except:
            spec = [" "," "," "," "," "," "," "," "," "," "," "]
            return spec
    def __get_CPU_freqency(self):
        cpu_usage = []
        cpu_frequecny = []
        cpu_on_Number = 0
        cpu_temp = 0
        fan_rpm = 0
        try:
            with jtop() as jetson:
                data_cpu = jetson.cpu
                data_temp = jetson.temperature
                data_fan = jetson.fan
                cpu_number = len(data_cpu)
                for i in range(cpu_number):
                    CPU_Name = "CPU" + str(i+1)
                    if str(data_cpu[CPU_Name]) != "{}":
                        use_val = data_cpu[CPU_Name]['val']
                        freq_val = int(data_cpu[CPU_Name]['frq'] /1000)
                        cpu_usage.append(use_val)
                        cpu_frequecny.append(freq_val)
                        cpu_on_Number += 1
                    else:
                        cpu_usage.append(0)
                        cpu_frequecny.append(0)
                cpu_temp = data_temp['CPU']
                fan_rpm  = data_fan['rpm']
        except:
            pass
        return cpu_number,cpu_on_Number,cpu_frequecny,cpu_usage,cpu_temp,fan_rpm

    def __get_GPU_freqency(self):
        gpu_frequency = 0
        gpu_usage = 0
        mem_usage = 0
        gpu_temp = 0
        thermal_temp = 0
        try:      
            with jtop() as jetson:
                data_gpu = jetson.gpu
                data_mem = jetson.ram
                data_temp = jetson.temperature

                gpu_frequency = int(data_gpu['frq'] /1000)
                gpu_usage = data_gpu['val']
                mem_usage = (round((data_mem['use']/data_mem['tot'])*100,1))
                gpu_temp = data_temp['GPU']
                thermal_temp = data_temp['thermal']

        except:
            pass
        return gpu_frequency ,gpu_temp,mem_usage,gpu_usage,thermal_temp

            

