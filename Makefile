CXX = g++
CXXFLAGS = -std=c++20 -Wall -O2 -Iinclude -MMD -MP
SANITIZERS ?= undefined
TEST_CXXFLAGS = -std=c++20 -g -Wall -Wextra -fsanitize=$(SANITIZERS) -Iinclude

CORE_OBJS = serialReader.o measurementValidation.o timestampValidation.o
DEPS = main.d $(CORE_OBJS:.o=.d)

scanner: main.o $(CORE_OBJS)
	$(CXX) main.o $(CORE_OBJS) -o scanner

main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o main.o

serialReader.o: src/serialReader.cpp
	$(CXX) $(CXXFLAGS) -c src/serialReader.cpp -o serialReader.o

measurementValidation.o: src/measurementValidation.cpp
	$(CXX) $(CXXFLAGS) -c src/measurementValidation.cpp -o measurementValidation.o

timestampValidation.o: src/timestampValidation.cpp
	$(CXX) $(CXXFLAGS) -c src/timestampValidation.cpp -o timestampValidation.o


.PHONY: test
test: $(CORE_OBJS)
	@tmp_dir=$$(mktemp -d); \
	trap 'rm -rf "$$tmp_dir"' EXIT; \
	set -e; \
	for file in tests/test_*.cpp; do \
		echo "Running $$file..."; \
		$(CXX) $(TEST_CXXFLAGS) $$file $(CORE_OBJS) -o "$$tmp_dir/runner" && "$$tmp_dir/runner"; \
		done; \
	echo "All tests passed!"

.PHONY: clean
clean:
	rm -f scanner main.o $(CORE_OBJS) $(DEPS)

-include $(DEPS)
 