CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2 -Iinclude -MMD -MP


scanner: main.o serialReader.o measurementValidation.o
	$(CXX) main.o serialReader.o measurementValidation.o -o scanner

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o main.o

serialReader.o: src/serialReader.cpp
	$(CXX) $(CXXFLAGS) -c src/serialReader.cpp -o serialReader.o

measurementValidation.o: src/measurementValidation.cpp
	$(CXX) $(CXXFLAGS) -c src/measurementValidation.cpp -o measurementValidation.o


.PHONY: clean
clean:
	rm -f scanner main.o serialReader.o measurementValidation.o main.d serialReader.d measurementValidation.d

-include main.d serialReader.d measurementValidation.d
 