
from jtop import jtop, JtopException
import json

def read_stats(jetson):
    """
    This is your callback function where you can read all files when are availables.
    """
    print(jetson.stats)


if __name__ == "__main__":
    print("Initialize jtop callback")
    # Open the jtop
    with jtop() as jetson:
            print('*** CPUs ***')
            print(jetson.cpu)
            # GPU
            print('*** GPU ***')
            print(jetson.gpu)
            # Engines
            print('*** engine ***')
            print(jetson.engine)
            # nvpmodel
            print('*** NV Power Model ***')
            print(jetson.nvpmodel)
            # jetson_clocks
            print('*** jetson_clocks ***')
            print(jetson.jetson_clocks)
            # Status disk
            print('*** disk ***')
            print(jetson.disk)
            # Status fans
            print('*** fan ***')
            print(jetson.fan)
            # uptime
            print('*** uptime ***')
            print(jetson.uptime)
            # local interfaces
            print('*** local interfaces ***')
            print(jetson.local_interfaces)
            # Temperature
            print('*** temperature ***')
            print(jetson.temperature)
            # Power
            print('*** power ***')
            print(jetson.power)
            # EMC
            print('*** emc ***')
            print(jetson.emc)
            # IRAM
            print('*** ram ***')
            print(jetson.ram)
            # IRAM
            print('*** iram ***')
            print(jetson.iram)
            # MTS
            print('*** mts ***')
            print(jetson.mts)
            # jetson.ok() will provide the proper update frequency
            #if jetson.ok():
                # CPU
            #print('*** CPUs ***')
            #print(jetson.cpu)
            test = len(jetson.cpu)
            CPU_Usage = []
            CPU_Frequency = []
            with jtop() as jetson:
                cpu_number = len(jetson.cpu)
                for i in range(cpu_number):
                    CPU_Name = "CPU" + str(i+1)
                    #print(CPU_Name)
                    if str(jetson.cpu[CPU_Name]) != "{}":
                        pass
                        #print(jetson.cpu[CPU_Name])
                    #use_val = jetson.cpu[CPU_Name]['val']
                    #freq_val = jetson.cpu[CPU_Name]['frq'] /1000
                    #CPU_Usage.append(use_val)
                    #CPU_Frequency.append(freq_val)
            #print(test)

            #cpu_dict = json.loads(jetson.cpu)

        # Output: {'name': 'Bob', 'languages': ['English', 'Fench']}
            #print( cpu_dict)

# Output: ['English', 'French']
            #print(cpu_dict['CPU1'])