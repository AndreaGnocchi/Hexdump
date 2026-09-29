CC     := gcc
CFLAGS := -Wall -Wextra -O2

# AGLib location (submodule by default). Override: make AGLIB_DIR=/path/to/AGLib
AGLIB_DIR ?= AGLib

# hexdump uses POSIX open()/fcntl, so only Linux and macOS are supported
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  PLATFORM := mac
  CC       := clang
else
  PLATFORM := linux
endif

AGLIB_LIB := $(AGLIB_DIR)/build/$(PLATFORM)/libag.a

TARGET := hexdump
SRC    := main.c

.PHONY: all clean

all: $(TARGET)

$(AGLIB_LIB):
	$(MAKE) -C $(AGLIB_DIR) all-$(PLATFORM)

$(TARGET): $(SRC) $(AGLIB_LIB)
	$(CC) $(CFLAGS) -I$(AGLIB_DIR)/include $(SRC) -o $@ $(AGLIB_LIB)

clean:
	rm -f $(TARGET)
