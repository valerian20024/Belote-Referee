CXX := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -g

ARCHIVE := s200825.tar.xz

ARCHIVE_FILES := src/belote.hh src/belote.cc

EXECUTABLE := belote

all: src/belote.cc src/belote.hh
	$(CXX) $(CXXFLAGS) src/belote.cc src/main.cc -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(ARCHIVE)

# Add a root directory with student ID in the archive
archive: $(ARCHIVE_FILES)
	@tar -cJf $(ARCHIVE) --transform='s,^,s200825/,' $^

.PHONY: all clean archive