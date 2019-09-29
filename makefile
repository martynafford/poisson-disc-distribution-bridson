NAME := poisson-disc-distribution
CXX ?= g++
CXXFLAGS := ${CXXFLAGS}
CXXFLAGS += -std=c++11 -O2
CXXFLAGS += -fno-rtti -fno-exceptions
CXXFLAGS += -Wall -Wextra -Wpedantic
CXXFLAGS += -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef -Wno-format-nonliteral
CXXFLAGS += -I./include

all:
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(NAME)
