CXX      := clang++
CXXFLAGS := -Weverything -Wno-c++98-compat -g

ARCHIVE := s200825.tar.xz

ARCHIVE_FILES := belote.hh belote.cc

all: belote.cc belote.hh
	$(CXX) $(CXXFLAGS) belote.cc main.cc -o belote

clean:
	rm -f belote $(ARCHIVE)

# Add a root directory with student ID in the archive
archive: $(ARCHIVE_FILES)
	@tar -cJf $(ARCHIVE) --transform='s,^,s200825/,' $^

.PHONY: all clean archive