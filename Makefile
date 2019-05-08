CXX = gcc
CXXFLAGS = -Wall -Werror -Wextra -pedantic -std=c99 -fsanitize=address -g
LDFLAGS =  -fsanitize=address

SRC = main.c parse.c
OBJ = $(SRC:.cc=.o)
EXEC = minimake

all: $(EXEC)

$(EXEC):  $(OBJ)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $(OBJ) $(LBLIBS)

clean:
	rm -rf $(OBJ) $(EXEC)