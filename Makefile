CXX = gcc
CXXFLAGS = -Wall -Werror -Wextra -pedantic -std=c99 -g -fsanitize=address
LDFLAGS =  -fsanitize=address

SRC = main.c
OBJ = $(SRC:.cc=.o)
EXEC = minimake

all: $(EXEC)

$(EXEC):  $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)