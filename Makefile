CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -Igraphics
LIBS = -lSDL2
TARGET = engine3d
SOURCES = src/geometry.cpp graphics/window_renderer.cpp src/main.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run