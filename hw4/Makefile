CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
TARGET = hw4

all: $(TARGET)

$(TARGET): hw4.c
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	./$(TARGET)
	@echo "---- average.txt ----"
	@cat average.txt

clean:
	rm -f $(TARGET) average.txt

.PHONY: all run clean