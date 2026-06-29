CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic -O2
LDLIBS ?= -lm
TARGET ?= linux-x86

HOST_OS := $(shell uname -s)
HOST_MACHINE := $(shell uname -m)
HAS_LINUX_GNU_GCC := $(shell command -v x86_64-linux-gnu-gcc 2>/dev/null)
HAS_LINUX_MUSL_GCC := $(shell command -v x86_64-linux-musl-gcc 2>/dev/null)
HAS_ZIG := $(shell command -v zig 2>/dev/null)

BUILD_DIR := build
LINUX_X86_BIN := $(BUILD_DIR)/linux-x86_64/cagefight
NATIVE_BIN := $(BUILD_DIR)/native/cagefight
APPLE_SILICON_BIN := $(BUILD_DIR)/apple-silicon/cagefight
ZIG_GLOBAL_CACHE_DIR ?= $(BUILD_DIR)/zig-cache/global
ZIG_LOCAL_CACHE_DIR ?= $(BUILD_DIR)/zig-cache/local
RUN_BIN ?= $(NATIVE_BIN)
SWIFT_PROJECT ?= ../CageFightMetal/CageFightMetal.xcodeproj
SWIFT_SCHEME ?= CageFightMetal
SWIFT_CONFIGURATION ?= Debug
SWIFT_DERIVED_DATA ?= /tmp/cagefightmetal-derived
TEST_SWIFT ?= 1

ifeq ($(HOST_OS),Linux)
ifeq ($(HOST_MACHINE),x86_64)
LINUX_X86_CC ?= cc
else
ifneq ($(HAS_LINUX_GNU_GCC),)
LINUX_X86_CC ?= x86_64-linux-gnu-gcc
else ifneq ($(HAS_LINUX_MUSL_GCC),)
LINUX_X86_CC ?= x86_64-linux-musl-gcc
else ifneq ($(HAS_ZIG),)
LINUX_X86_CC ?= zig cc -target x86_64-linux-musl
else
LINUX_X86_CC ?= x86_64-linux-gnu-gcc
endif
endif
else
ifneq ($(HAS_LINUX_GNU_GCC),)
LINUX_X86_CC ?= x86_64-linux-gnu-gcc
else ifneq ($(HAS_LINUX_MUSL_GCC),)
LINUX_X86_CC ?= x86_64-linux-musl-gcc
else ifneq ($(HAS_ZIG),)
LINUX_X86_CC ?= zig cc -target x86_64-linux-musl
else
LINUX_X86_CC ?= x86_64-linux-gnu-gcc
endif
endif
LINUX_X86_CFLAGS ?= $(CFLAGS)
LINUX_X86_LDFLAGS ?=
LINUX_X86_LDLIBS ?= $(LDLIBS)
NATIVE_CC ?= $(CC)
NATIVE_CFLAGS ?= $(CFLAGS)
NATIVE_LDFLAGS ?=
NATIVE_LDLIBS ?= $(LDLIBS)
APPLE_SILICON_CC ?= cc
APPLE_SILICON_CFLAGS ?= $(CFLAGS) -arch arm64
APPLE_SILICON_LDFLAGS ?=
APPLE_SILICON_LDLIBS ?= $(LDLIBS)
SRC := src/cagefight.c
HEADERS := src/cagefight.h
COMMAND_SETS := command_sets/headhunter.cfos \
	command_sets/joint_reaper.cfos \
	command_sets/shock_clinch.cfos \
	command_sets/cross_guard.cfos \
	command_sets/limb_breaker.cfos \
	command_sets/clinch_driver.cfos \
	command_sets/counter_guard.cfos

.PHONY: all clean demo tournament moves version linux-x86 linux-x86_64 native host apple-silicon mac macos-arm64 swift-mac setup-linux-x86 test test-builds test-build-matrix

all: $(TARGET)

linux-x86: linux-x86_64

linux-x86_64: $(LINUX_X86_BIN)

$(LINUX_X86_BIN): $(SRC) $(HEADERS) | $(BUILD_DIR)/linux-x86_64
	@command -v $(firstword $(LINUX_X86_CC)) >/dev/null 2>&1 || { echo "error: $(firstword $(LINUX_X86_CC)) not found. Install a Linux x86_64 compiler or pass LINUX_X86_CC=..."; exit 127; }
	mkdir -p "$(ZIG_GLOBAL_CACHE_DIR)" "$(ZIG_LOCAL_CACHE_DIR)"
	ZIG_GLOBAL_CACHE_DIR="$(ZIG_GLOBAL_CACHE_DIR)" ZIG_LOCAL_CACHE_DIR="$(ZIG_LOCAL_CACHE_DIR)" $(LINUX_X86_CC) $(LINUX_X86_CFLAGS) $(LINUX_X86_LDFLAGS) -o $@ $(SRC) $(LINUX_X86_LDLIBS)

native host: $(NATIVE_BIN)

$(NATIVE_BIN): $(SRC) $(HEADERS) | $(BUILD_DIR)/native
	@command -v $(firstword $(NATIVE_CC)) >/dev/null 2>&1 || { echo "error: $(firstword $(NATIVE_CC)) not found. Install a host C compiler or pass NATIVE_CC=..."; exit 127; }
	$(NATIVE_CC) $(NATIVE_CFLAGS) $(NATIVE_LDFLAGS) -o $@ $(SRC) $(NATIVE_LDLIBS)

apple-silicon mac macos-arm64: $(APPLE_SILICON_BIN)

$(APPLE_SILICON_BIN): $(SRC) $(HEADERS) | $(BUILD_DIR)/apple-silicon
	@if [ "$(HOST_OS)" != "Darwin" ]; then echo "error: Apple Silicon CLI builds require macOS/Xcode command line tools. Use the Swift/Xcode build for app targets."; exit 2; fi
	@command -v $(firstword $(APPLE_SILICON_CC)) >/dev/null 2>&1 || { echo "error: $(firstword $(APPLE_SILICON_CC)) not found. Install Xcode command line tools or pass APPLE_SILICON_CC=..."; exit 127; }
	$(APPLE_SILICON_CC) $(APPLE_SILICON_CFLAGS) $(APPLE_SILICON_LDFLAGS) -o $@ $(SRC) $(APPLE_SILICON_LDLIBS)

swift-mac:
	@if [ ! -d "$(SWIFT_PROJECT)" ]; then echo "error: Swift project not found at $(SWIFT_PROJECT)"; exit 2; fi
	xcodebuild -project "$(SWIFT_PROJECT)" -scheme "$(SWIFT_SCHEME)" -configuration "$(SWIFT_CONFIGURATION)" -derivedDataPath "$(SWIFT_DERIVED_DATA)" build

setup-linux-x86:
	@if command -v $(firstword $(LINUX_X86_CC)) >/dev/null 2>&1; then \
		echo "Linux x86_64 compiler already available: $(LINUX_X86_CC)"; \
	elif command -v brew >/dev/null 2>&1; then \
		echo "Installing Zig for Linux x86_64 cross-builds"; \
		brew install zig; \
	else \
		echo "error: install zig, x86_64-linux-gnu-gcc, or x86_64-linux-musl-gcc"; \
		exit 127; \
	fi

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/linux-x86_64:
	mkdir -p $(BUILD_DIR)/linux-x86_64

$(BUILD_DIR)/native:
	mkdir -p $(BUILD_DIR)/native

$(BUILD_DIR)/apple-silicon:
	mkdir -p $(BUILD_DIR)/apple-silicon

demo: $(RUN_BIN)
	$(RUN_BIN) command_sets/headhunter.cfos command_sets/limb_breaker.cfos 42

tournament: $(RUN_BIN)
	$(RUN_BIN) --tournament 42 $(COMMAND_SETS)

moves: $(RUN_BIN)
	$(RUN_BIN) --list-moves

version: $(RUN_BIN)
	$(RUN_BIN) --version

test: test-builds

test-build-matrix: test-builds

test-builds:
	@set -u; \
	fail=0; \
	built=0; \
	skipped=0; \
	echo "CFA build availability matrix"; \
	echo "host: $(HOST_OS) $(HOST_MACHINE)"; \
	echo "default target: $(TARGET)"; \
	run_target() { \
		label="$$1"; \
		target="$$2"; \
		bin="$$3"; \
		smoke="$$4"; \
		available="$$5"; \
		reason="$$6"; \
		if [ "$$available" != "yes" ]; then \
			echo "--> $$label: skipped ($$reason)"; \
			skipped=$$((skipped + 1)); \
			return; \
		fi; \
		echo "==> $$label: make $$target"; \
		if $(MAKE) --no-print-directory "$$target"; then \
			built=$$((built + 1)); \
			if [ -n "$$bin" ] && [ -f "$$bin" ] && command -v file >/dev/null 2>&1; then \
				file "$$bin"; \
			fi; \
			if [ "$$smoke" = "yes" ]; then \
				echo "==> $$label: smoke --list-moves"; \
				if "$$bin" --list-moves >/dev/null; then \
					echo "ok: $$label smoke run passed"; \
				else \
					echo "FAIL: $$label smoke run failed"; \
					fail=$$((fail + 1)); \
				fi; \
			else \
				echo "--> $$label: smoke skipped (not executable on this host)"; \
			fi; \
		else \
			echo "FAIL: $$label build failed"; \
			fail=$$((fail + 1)); \
		fi; \
	}; \
	has_linux=no; \
	linux_reason="$(firstword $(LINUX_X86_CC)) unavailable"; \
	if command -v $(firstword $(LINUX_X86_CC)) >/dev/null 2>&1; then has_linux=yes; linux_reason=""; fi; \
	linux_smoke=no; \
	if [ "$(HOST_OS)" = "Linux" ] && [ "$(HOST_MACHINE)" = "x86_64" ]; then linux_smoke=yes; fi; \
	run_target "linux-x86_64" "linux-x86_64" "$(LINUX_X86_BIN)" "$$linux_smoke" "$$has_linux" "$$linux_reason"; \
	has_native=no; \
	native_reason="$(firstword $(NATIVE_CC)) unavailable"; \
	if command -v $(firstword $(NATIVE_CC)) >/dev/null 2>&1; then has_native=yes; native_reason=""; fi; \
	run_target "native" "native" "$(NATIVE_BIN)" "yes" "$$has_native" "$$native_reason"; \
	has_apple=no; \
	apple_reason="not running on macOS"; \
	if [ "$(HOST_OS)" = "Darwin" ]; then \
		apple_reason="$(firstword $(APPLE_SILICON_CC)) unavailable"; \
		if command -v $(firstword $(APPLE_SILICON_CC)) >/dev/null 2>&1; then has_apple=yes; apple_reason=""; fi; \
	fi; \
	apple_smoke=no; \
	if [ "$(HOST_OS)" = "Darwin" ] && [ "$(HOST_MACHINE)" = "arm64" ]; then apple_smoke=yes; fi; \
	run_target "apple-silicon" "apple-silicon" "$(APPLE_SILICON_BIN)" "$$apple_smoke" "$$has_apple" "$$apple_reason"; \
	has_swift=no; \
	swift_reason="TEST_SWIFT=0"; \
	if [ "$(TEST_SWIFT)" != "0" ]; then \
		swift_reason="xcodebuild unavailable or $(SWIFT_PROJECT) missing"; \
		if command -v xcodebuild >/dev/null 2>&1 && [ -d "$(SWIFT_PROJECT)" ]; then has_swift=yes; swift_reason=""; fi; \
	fi; \
	run_target "swift-mac" "swift-mac" "" "no" "$$has_swift" "$$swift_reason"; \
	if [ "$$has_native" = "yes" ]; then \
		echo "==> cfa version smoke"; \
		expected_version=$$(sed -n 's/^#define CFA_CORE_VERSION "\([^"]*\)"/\1/p' src/cagefight.h); \
		version_output=$$("$(NATIVE_BIN)" --version); \
		if [ "$$version_output" = "$$expected_version" ]; then \
			echo "ok: cfa core version $$version_output"; \
		else \
			echo "FAIL: cfa core version expected $$expected_version, got $$version_output"; \
			fail=$$((fail + 1)); \
		fi; \
		echo "==> cfa log smoke"; \
		mkdir -p "$(BUILD_DIR)/test-logs"; \
		if CFA_LOG_DIR="$(BUILD_DIR)/test-logs" "$(NATIVE_BIN)" --smoke-log command_sets/headhunter.cfos command_sets/limb_breaker.cfos 42 3 > "$(BUILD_DIR)/test-logs/smoke.out"; then \
			log_path=$$(tail -n 1 "$(BUILD_DIR)/test-logs/smoke.out"); \
			if [ -f "$$log_path" ] && grep -q "smoke_turn turn=3" "$$log_path" && grep -q "ACTION .*finish: smoke log complete" "$$log_path"; then \
				echo "ok: cfa log smoke wrote $$log_path"; \
			else \
				echo "FAIL: cfa log smoke did not produce expected frame/action entries"; \
				fail=$$((fail + 1)); \
			fi; \
		else \
			echo "FAIL: cfa log smoke command failed"; \
			fail=$$((fail + 1)); \
		fi; \
	else \
		echo "--> cfa log smoke: skipped (native build unavailable)"; \
		skipped=$$((skipped + 1)); \
	fi; \
	echo "summary: $$built built, $$skipped skipped, $$fail failed"; \
	if [ "$$built" -eq 0 ]; then echo "FAIL: no build targets were available"; exit 1; fi; \
	if [ "$$fail" -ne 0 ]; then exit 1; fi

clean:
	rm -rf $(BUILD_DIR)
