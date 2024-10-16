CXX = g++
CFLAGS = -Iinclude -I/usr/include/opencv4 -g
TARGET = build/server
SRC = src/server.cpp src/main.cpp
OBJ = src/server.o  src/main.o
INC = include/logger.h include/server.hpp

all: $(TARGET)
	 
$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(TARGET) $(shell pkg-config --cflags --libs opencv4)

src/server.o : src/server.cpp $(INC)
	$(CXX) $(CFLAGS) -c src/server.cpp -o src/server.o 

src/main.o: src/main.cpp $(INC)
	$(CXX) $(CFLAGS) -c src/main.cpp -o src/main.o

clean:
	rm -f $(OBJ) $(TARGET)



