CC = g++
TARGET = forward
OBJS = MemoryPool.o TCPNetworkEngine.o SThread.o main.o sha1.o Base64.o
CFLAGS = -g -std=c++11 -pthread -static-libstdc++ -static-libgcc

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJS):%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-$(RM) $(TARGET)
	-$(RM) $(OBJS)
