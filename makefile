# 指定編譯器
CC = gcc

# 指定編譯選項
CFLAGS = -Wall -g

# 指定所需的目標程式名稱
TARGET = main

# 指定需要編譯的源文件
SRCS = main.c list.c ui.c

# 指定編譯後的對象文件
OBJS = $(SRCS:.c=.o)

# 指定包含的頭文件
INCLUDES = -I.

# 生成最終執行檔目標
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lsqlite3

# 編譯.c文件到.o文件
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# 清理命令
clean:
	rm -f $(OBJS) $(TARGET)
