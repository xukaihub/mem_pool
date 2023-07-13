CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lpthread

TARGET = test
SOURCES = test.c mem_pool.c mem_pool_lock.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
