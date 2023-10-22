# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11 -Iinclude

# Object files
OBJS = main.o server.o load_balancer.o http_message.o

# Executable name
EXEC = my_load_balancer

# Default target
all: $(EXEC)

# Linking to produce the executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)

# Compiling the source files into object files
main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp

server.o: src/server.cpp
	$(CXX) $(CXXFLAGS) -c src/server.cpp

http_message.o: src/http_message.cpp
	$(CXX) $(CXXFLAGS) -c src/http_message.cpp

load_balancer.o: src/load_balancer.cpp
	$(CXX) $(CXXFLAGS) -c src/load_balancer.cpp

# Cleanup
clean:
	rm -f $(OBJS) $(EXEC)
