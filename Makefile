CXX := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -g

ARCHIVE := s200825.tar.xz

SRC_DIR := src

ARCHIVE_FILES := $(SRC_DIR)/belote.hh $(SRC_DIR)/belote.cc

EXECUTABLE := belote

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC_DIR)/belote.cc $(SRC_DIR)/belote.hh
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/belote.cc $(SRC_DIR)/main.cc -o $(EXECUTABLE)


debug: CXXFLAGS += -DDEBUG
debug: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(ARCHIVE)

# Add a root directory with student ID in the archive
archive: $(ARCHIVE_FILES)
	tar -cJf $(ARCHIVE) --transform='s,^src/,,;s,^,s200825/,' $^

.PHONY: all clean archive debug