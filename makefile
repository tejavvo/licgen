# Licgen - License Generator Tool
# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I$(SRC_DIR)/headers
LDFLAGS =

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Installation
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
DATADIR = $(PREFIX)/share/licgen

# Path for templates
CFLAGS += -DTEMPLATE_DIR=\"$(DATADIR)/templates\"

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/colors.c \
       $(SRC_DIR)/prompt.c \
       $(SRC_DIR)/help.c \
       $(SRC_DIR)/licenses.c \
       $(SRC_DIR)/json.c

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Binary name
TARGET = licgen

.PHONY: all build clean install uninstall debug

all: $(TARGET)

build: $(OBJS)
	$(CC) $(OBJS) -o licgen $(LDFLAGS)
	@echo "Build complete: ./licgen"

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) licgen
	@echo "Cleaned build directory and local binary"

install: $(TARGET)
	@mkdir -p $(DESTDIR)$(BINDIR)
	@mkdir -p $(DESTDIR)$(DATADIR)/templates
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/licgen
	cp templates/*.template $(DESTDIR)$(DATADIR)/templates/
	@echo "Installed licgen to $(DESTDIR)$(BINDIR)/licgen"
	@echo "Installed templates to $(DESTDIR)$(DATADIR)/templates"

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/licgen
	rm -rf $(DESTDIR)$(DATADIR)
	@echo "Uninstalled licgen"

debug: CFLAGS += -g -DDEBUG
debug: clean $(TARGET)
