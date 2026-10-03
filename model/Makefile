CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -MMD -MP
SANFLAGS = -fsanitize=address,undefined -g -O0

TARGET = model


SRC = src/main.cpp
OBJ = $(SRC:src/%.cpp=build/%.o)
DEP = $(OBJ:.o=.d)

release: $(TARGET)

debug: CXXFLAGS += $(SANFLAGS)
debug: $(TARGET)_debug

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

$(TARGET)_debug: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(SANFLAGS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -Iinclude -Itests -c $< -o $@

-include $(DEP)

clean:
	rm -rf build model model_debug

.PHONY:release debug clean

