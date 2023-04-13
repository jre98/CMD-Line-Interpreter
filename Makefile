
#####
##### DO NOT EDIT THIS FILE
#####

define say
	$(info [Assignment 2] $1)
endef


#
CC := g++
COMMONFLAGS := -Wall -pedantic -std=c++17 -g
COMPILEFLAGS := $(COMMONFLAGS) -c
LINKFLAGS := $(COMMONFLAGS) -pthread


#
BIN_NAME := main
BIN := ./$(BIN_NAME)
#
BIN_TESTS_NAME := main-tests
BIN_TESTS := ./$(BIN_TESTS_NAME)


#
default:	help


#
help:
	@echo "Assignment 2 - Makefile Menu"
	@echo
	@echo "make build   ==> Build the program"
	@echo "make run     ==> Run the program"
	@echo "make debug   ==> Debug the program with gdb"
	@echo
	@echo "make test    ==> Test your program"
	@echo
	@echo "make clean   ==> Remove object files, the main executables, and other temporary files"


#
build:	$(BIN)
	$(call say,Finished building)

run:	$(BIN)
	$(BIN)

debug:	$(BIN)
	gdb $(BIN) -ex run

gradescope: test
tests:	test
test:	$(BIN_TESTS)
	$(BIN_TESTS)

.PHONY: build run debug test gradescope


#
$(BIN):	main.o Parser.o Executor.o
	$(CC) $(LINKFLAGS) $^ -o $@


#
$(BIN_TESTS):	CPP_Tests.o Parser.o Executor.o
	$(CC) $(LINKFLAGS) $^ -o $@


#
main.o:	main.cpp Parser.hpp Executor.hpp
	$(CC) $(COMPILEFLAGS) $< -o $@


#
Executor.o:	Executor.cpp Executor.hpp
	$(CC) $(COMPILEFLAGS) $< -o $@


#
Parser.o:	Parser.cpp Parser.hpp
	$(CC) $(COMPILEFLAGS) $< -o $@


#
CPP_Tests.o:	CPP_Tests.cpp Parser.hpp Executor.hpp puhp-tests/PuhPTests.h
	$(CC) $(COMPILEFLAGS) $< -o $@


#
clean:
	-rm *.o
	-rm $(BIN)
	-rm $(BIN_TESTS)
.PHONY: clean





