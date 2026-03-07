CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2 -Iinclude

scanner: src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o scanner

clean:
	rm -f scanner
