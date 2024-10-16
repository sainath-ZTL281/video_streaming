CXX = g++
CFLAGS = -Iinclude -I/usr/include/opencv4 -g
TARGET = build/client
SRC = src/client.cpp src/main.cpp
OBJ = src/client.o  src/main.o
INC = include/logger.h include/client.hpp

all: $(TARGET)
	 
$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(TARGET) $(shell pkg-config --cflags --libs opencv4)

src/client.o : src/client.cpp $(INC)
	$(CXX) $(CFLAGS) -c src/client.cpp -o src/client.o 

src/main.o: src/main.cpp $(INC)
	$(CXX) $(CFLAGS) -c src/main.cpp -o src/main.o

clean:
	rm -f $(OBJ) $(TARGET)


	