.PHONY: all clean clean-all clean-log

GXX=g++
BIN_FOLDER=bin
SRC_FOLDER=src
LOG_FOLDER=log

FLAGS=-lSDL -lSDL_ttf

APP_NAME=app

all: $(BIN_FOLDER)/$(APP_NAME)

$(BIN_FOLDER)/$(APP_NAME): $(BIN_FOLDER)/main.o $(BIN_FOLDER)/m_learning.o $(BIN_FOLDER)/snake.o
	$(GXX) -o $@ $^ $(FLAGS)

$(BIN_FOLDER)/m_learning.o: $(SRC_FOLDER)/m_learning.cpp $(SRC_FOLDER)/m_learning.h
	$(GXX) -o $@ -c $< $(FLAGS)

$(BIN_FOLDER)/snake.o: $(SRC_FOLDER)/snake.cpp $(SRC_FOLDER)/snake.h
	$(GXX) -o $@ -c $< $(FLAGS)

$(BIN_FOLDER)/main.o: main.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

clean:
	rm -f $(BIN_FOLDER)/*.o

clean-log:
	rm -f $(LOG_FOLDER)/*.log

clean-all: clean clean-log
	rm -f $(BIN_FOLDER)/$(APP_NAME)
