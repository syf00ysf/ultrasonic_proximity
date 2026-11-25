CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2 -Iinclude -MMD -MP


scanner: main.o serialReader.o measurementValidation.o timestampValidation.o
	$(CXX) main.o serialReader.o measurementValidation.o timestampValidation.o -o scanner

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o main.o

serialReader.o: src/serialReader.cpp
	$(CXX) $(CXXFLAGS) -c src/serialReader.cpp -o serialReader.o

measurementValidation.o: src/measurementValidation.cpp
	$(CXX) $(CXXFLAGS) -c src/measurementValidation.cpp -o measurementValidation.o

timestampValidation.o: src/timestampValidation.cpp
	$(CXX) $(CXXFLAGS) -c src/timestampValidation.cpp -o timestampValidation.o


.PHONY: clean
clean:
	rm -f scanner main.o serialReader.o measurementValidation.o timestampValidation.o main.d serialReader.d measurementValidation.d timestampValidation.d

-include main.d serialReader.d measurementValidation.d timestampValidation.d
 