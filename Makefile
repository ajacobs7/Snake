#Makefile

#Compiler
CC = g++

# Compiler Flags:
CFLAGS = -g -Wall

# Target Executable:
TARGET = snake
all: $(TARGET)

$(TARGET): ${TARGET}.cpp
	$(CC) $(CFLAGS) ${TARGET}.cpp -o $(TARGET) -I/usr/include/allegro5 -L/usr/lib -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf

clean:
	$(RM) $(TARGET)

