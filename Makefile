CC = gcc
OUT_DIR = out
SRC_DIR = src
TARGET = $(OUT_DIR)/HexViewer.out


$(TARGET): $(SRC_DIR)/main.c
	$(CC) -o $@ $(SRC_DIR)/main.c

clean:
	rm $(OUT_DIR)/*

.PHONY: clean

