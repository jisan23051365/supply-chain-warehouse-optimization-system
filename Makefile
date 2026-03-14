# Makefile - Supply Chain & Warehouse Management System

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -g
SRCDIR  = src
OBJDIR  = obj
TARGET  = warehouse_mgmt

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(SRCDIR) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)
