CC      = g++ -O3 -std=c++17
CFLAGS  = -Wall -Wextra -I../include -I./unique-function/include
SRC_DIR = ./src
OBJ_DIR = ./obj
SOURCES = $(shell ls $(SRC_DIR)/*.cpp)
OBJS    = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.o))
FC_TARGET = function.out
UF_TARGET = unique_function.out
TARGET = $(FC_TARGET) $(UF_TARGET)
DEPENDS = $(OBJS:.o=.d)

all: $(TARGET)

$(FC_TARGET): $(OBJ_DIR)/function.o
	$(CC) -o $@ $< -lpthread

$(UF_TARGET): $(OBJ_DIR)/unique_function.o
	$(CC) -o $@ $< -lpthread

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if [ ! -d $(OBJ_DIR) ]; \
		then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
	fi
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPENDS)

-include $(DEPENDS)

.PHONY: all clean
