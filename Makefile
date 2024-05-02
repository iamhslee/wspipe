# 2024-1 Operating Systems (ITP30002) - HW #2
# File  : Makefile
# Author: Hyunseo Lee (22100600) <hslee@handong.ac.kr>

CC=gcc
SOURCE=wspipe.c
TARGET=wspipe

all:
	@echo "Build $(TARGET)..."
	$(CC) $(SOURCE) -o $(TARGET)
	@echo "Build $(TARGET) done."

debug:
	@echo "Build $(TARGET) with debug mode..."
	$(CC) $(SOURCE) -Wall -Wextra -DDEBUG -o $(TARGET)
	@echo "Build $(TARGET) with debug mode done."

clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
	@rm -rf $(TARGET).dSYM
	@rm -rf TEST
	@echo "Cleaned up."