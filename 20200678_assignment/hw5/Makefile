CC = gcc209
CFLAGS += -g -fsanitize=address

STUDENT_ID := $(shell cat STUDENT_ID)
SUBMIT_DIR := .submit
SUBMIT_FILES:= *.c *.h Makefile readme EthicsOath.pdf
SUBMIT := $(STUDENT_ID)_assign5.tar.gz

TARGET = ish
OBJFILES = ish.o dynarray.o token.o analyzer.o valid.o
# FIX THIS FILE

all: $(TARGET)

ish: $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^

ish.o: dynarray.h token.h analyzer.h valid.h

dynarray.o: dynarray.h

token.o: token.h

analyzer.o: dynarray.h token.h

valid.o: dynarray.h token.h

submit:
	mkdir -p $(SUBMIT_DIR)
	cp $(SUBMIT_FILES) $(SUBMIT_DIR)
	cd $(SUBMIT_DIR) && tar -czf ../$(SUBMIT) *
	rm -rf $(SUBMIT_DIR)

clean:
	rm -rf $(TARGET) *.o

.PHONY: all clean submit
