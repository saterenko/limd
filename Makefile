CC=gcc

SRC=./src
BUILD=./build
#CFLAGS=-std=gnu99 -g -Wall
CFLAGS = -std=gnu99 -g -O3 -fno-strict-aliasing
INCS=-I$(SRC)
LDLIBS=-lev -pthread
SRC_FILES=$(wildcard $(SRC)/*.c)
OBJ_FILES=$(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRC_FILES))
EXE_FILES=$(BUILD)/limd

all: prebuild $(EXE_FILES)

prebuild:
	test -d $(BUILD) || mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

$(EXE_FILES): $(OBJ_FILES)
	$(CC) $(OBJ) -o $@ $^ $(LDLIBS)

$(BUILD)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

