CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=gnu11
OBJS = graph.o minheap.o dijkstra.o csv_loader.o service.o main.o
TARGET = emergency_routing

ifeq ($(OS),Windows_NT)
    TARGET_BIN = $(TARGET).exe
    LDFLAGS = -lws2_32 -lm
    RM_CMD = cmd /c del /f /q $(OBJS) $(TARGET) $(TARGET).exe 2>nul || exit 0
else
    TARGET_BIN = $(TARGET)
    LDFLAGS = -lpthread -lm
    RM_CMD = rm -f $(OBJS) $(TARGET) $(TARGET).exe
endif

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_BIN) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM_CMD)

.PHONY: all clean
