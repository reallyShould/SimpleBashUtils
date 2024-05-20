TARGET = s21_cat
BUILD_DIR = .

CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: $(TARGET)

$(TARGET): s21_cat.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(TARGET)

s21_cat.o: s21_cat.c 
	$(CC) $(CFLAGS) -c s21_cat.c -o $(BUILD_DIR)/s21_cat.o

rebuild: clean all

clean:
	rm -f $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/*.o

run:
	./$(TARGET)

clang:
	clang-format --style=Google -i *.?

test: all
	python3 test.py

leaks:
	valgrind --leak-check=yes ./$(TARGET) tests/Text.txt
