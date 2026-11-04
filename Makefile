CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2 -Iinclude -MMD -MP

scanner: main.o serialReader.o
	$(CXX) main.o serialReader.o -o scanner

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o main.o

serialReader.o: src/serialReader.cpp
	$(CXX) $(CXXFLAGS) -c src/serialReader.cpp -o serialReader.o

.PHONY: clean
clean:
	rm -f scanner main.o serialReader.o main.d serialReader.d

-include main.d serialReader.d
 