CC = gcc
CFLAGS = -I../rpi_ws281x
LDFLAGS = -L../rpi_ws281x -lws2811 -lm
TARGET = client
SRCS = Client.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)