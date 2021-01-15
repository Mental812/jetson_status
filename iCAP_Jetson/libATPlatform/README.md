# What is libATPlatform

```
libATPlatform is a C++ library for geting informations like software, sensor, and remote control like update a file on Aetina(AT) platform.
```

# Runtime Environment

```
NVIDIA Jetson TX2(Ubuntu 16.04 arm64) + Aetina ACE-N310
```

# How to use

### Make sure you have installed all of the following prerequisites on your runtime environment:

```bash
None
```

### Running the sample:

```bash
cd libATPlatform
make
sudo ./bin/UsingATPlatform
```

### Chose the function that you want:

```bash
[A] ATP_IsAetinaPlatform.
[B] ATP_GetPlatformInfo.
[C] ATP_GetSWInfo.
[D] ATP_GetSensorInfo.
[E] ATP_SetModelFile.
[F] ATP_GetDynamicInfo.
[Q] Quit.
```

### Output:
```
A
----------------------------------------------------------------
ATP_IsAetinaPlatform...
It's aetina platform.
ATP_IsAetinaPlatform...done
----------------------------------------------------------------
B
----------------------------------------------------------------
ATP_GetPlatformInfo...
ModuleType : 2, 
Name : NVIDIA Jetson TX2, 
CarrierBoard : AN310, 
Architecture : NVIDIA Pascal™, 
CUDA Cores : 256, 
MemoryType : LPDDR4, 
MemorySize : 8 GB, 
L4T Version 32.3.1
ATP_GetPlatformInfo...done
----------------------------------------------------------------
C
----------------------------------------------------------------
ATP_GetSWInfo...
[0]CUDA Toolkit for L4T
[1]cuDNN
[4]VisionWorks
ATP_GetSWInfo...done
----------------------------------------------------------------
D
----------------------------------------------------------------
ATP_GetSensorInfo...
[0]LM75(Temperature)(34)
ATP_GetSensorInfo...done
----------------------------------------------------------------
```

# Limitation
The library only support AT platform.

# License
[IPA]

# How to build

### Make sure you have installed all of the following prerequisites on your development machine:

```bash
None
```

### Building the sample:
```C
cd libATPlatform
make
```

### Devlopment Environment
```
NVIDIA Jetson serial like TX2, Xavier, Nano, etc.
```