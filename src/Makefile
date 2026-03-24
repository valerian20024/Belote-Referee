CXX      := clang++
CXXFLAGS := -Weverything -Wno-c++98-compat -g

.PHONY: all clean

all: belote.cc belote.hh
	$(CXX) $(CXXFLAGS) belote.cc main.cc -o belote

clean:
	rm -f belote