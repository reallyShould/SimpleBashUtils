TARGET = s21_grep
BUILD_DIR = .

CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: $(TARGET)

$(TARGET): s21_grep.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(TARGET)

s21_grep.o: s21_grep.c 
	$(CC) $(CFLAGS) -c s21_grep.c -o $(BUILD_DIR)/s21_grep.o

rebuild: clean all

clean:
	rm -f $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/*.o

run:
	./$(TARGET)

clang:
	clang-format --style=Google -i *.c
	clang-format --style=Google -i *.h

test: all
	python3 test.py

leaks:
	valgrind --leak-check=yes ./$(TARGET) -e "e" pattern.txt
