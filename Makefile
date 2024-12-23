CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpthread

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGETS = $(BIN_DIR)/wserver $(BIN_DIR)/wclient $(BIN_DIR)/spin.cgi

all: $(TARGETS)

$(BIN_DIR)/wserver: $(OBJ_DIR)/wserver.o $(OBJ_DIR)/request.o $(OBJ_DIR)/io_helper.o $(OBJ_DIR)/bounded_queue.o $(OBJ_DIR)/worker.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/wclient: $(OBJ_DIR)/wclient.o $(OBJ_DIR)/io_helper.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/spin.cgi: $(SRC_DIR)/spin.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
