CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2
LDLIBS ?= -lm

BIN := build/cagefight
SRC := src/cagefight.c
COMMAND_SETS := command_sets/headhunter.cfos \
	command_sets/joint_reaper.cfos \
	command_sets/shock_clinch.cfos \
	command_sets/cross_guard.cfos \
	command_sets/limb_breaker.cfos \
	command_sets/clinch_driver.cfos \
	command_sets/counter_guard.cfos

.PHONY: all clean demo tournament moves

all: $(BIN)

$(BIN): $(SRC) | build
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDLIBS)

build:
	mkdir -p build

demo: $(BIN)
	$(BIN) command_sets/headhunter.cfos command_sets/limb_breaker.cfos 42

tournament: $(BIN)
	$(BIN) --tournament 42 $(COMMAND_SETS)

moves: $(BIN)
	$(BIN) --list-moves

clean:
	rm -rf build
