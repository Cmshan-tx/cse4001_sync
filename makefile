CXX = g++
CXXFLAGS = -Wall -Wextra -pthread

TARGET = cse4001_sync
SRCS = cse4001_sync.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

