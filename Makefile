CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2 -Iinclude

scanner: src/main.cpp src/serialReader.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp src/serialReader.cpp -o scanner

clean:
	rm -f scanner
 