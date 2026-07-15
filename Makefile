SRC = 	src/data.cpp				\
		src/draw.cpp				\
		src/input.cpp               \
		src/puzzles.cpp

MAIN 	= src/main.cpp
OUT 	= bin/main
INCLUDE = include/
LINK	= lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

run:
	g++ $(MAIN) $(SRC) -o $(OUT) -I $(INCLUDE) -L $(LINK)