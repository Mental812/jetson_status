CC = g++
CFLAGS := -fPIC -Wall -Wextra -O3 -g -fpermissive
CFLAGS += -D_DEBUG_ 
LDFLAGS := -lpthread

OBJS = libATPlatform.o
LIB = ./libATPlatform.so
SAMPLE_EXC = UsingATPlatform
SAMPLE_SRC = main.cpp
INCLUDE_PATH = include/

all : $(LIB) $(SAMPLE_EXC)

%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -c $< -o $@ 

$(LIB): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $(LIB) $(OBJS) $(LDFLAGS)

$(SAMPLE_EXC):
	$(CC) $(SAMPLE_SRC) $(LIB) -o $(SAMPLE_EXC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) $(LIB) $(SAMPLE_EXC)
