# Makefile pro Linux/Powershell. Nebude fungovat s Windows CMD.

CC = gcc

CFLAGS = -Wall -Wextra -g
LDFLAGS = $(CFLAGS)

BUILD_DIR = build
BIN = lp.exe

all: clean $(BUILD_DIR) $(BUILD_DIR)/$(BIN) install

$(BUILD_DIR)/$(BIN): $(BUILD_DIR)/matrix.o $(BUILD_DIR)/stack.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/simplex.o $(BUILD_DIR)/queue.o $(BUILD_DIR)/main.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/matrix.o: src/matrix.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/stack.o: src/stack.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/parser.o: src/parser.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/simplex.o: src/simplex.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/queue.o: src/queue.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/main.o: src/main.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(BIN)

# Podle zadání semestrální práce (cíl je možné přidat jako závislost cíle `all`).
install: $(BUILD_DIR)/$(BIN)
	cp $(BUILD_DIR)/$(BIN) ./$(BIN)
