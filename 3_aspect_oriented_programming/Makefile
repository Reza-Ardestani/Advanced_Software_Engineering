# The targets in this file are used in .gitlab-ci.yml and the files created are found in the .gitignore
# Changing any names below can change the target names which will require that you update .gitlab_ci.yml and .gitignore
PROJECT = fibonacci
GTEST = $(PROJECT)-test

# Compilation command and flags
CXX=g++
ACXX=ag++
CXXVERSION= -std=c++11
CXXFLAGS= $(CXXVERSION) -g -fprofile-arcs -ftest-coverage
LINKFLAGS= -lgtest

# Directories
SRC_DIR = src
SRC_INCLUDE = include
INCLUDE = -I ${SRC_INCLUDE}
GTEST_DIR = test

# Tool variables
GCOV = gcov
LCOV = lcov
COVERAGE_RESULTS = results.coverage
COVERAGE_DIR = coverage

STATIC_ANALYSIS = cppcheck

STYLE_CHECK = cpplint.py

DOXY_DIR = docs/code

# Default goal, used by Atom for local compilation
.DEFAULT_GOAL := $(PROJECT)

# default rule for compiling .cc to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean up all files that should NOT be submitted
.PHONY: clean
clean:
	rm -rf *~ $(SRC)/*.o $(GTEST_DIR)/output/*.dat \
	*.gcov *.gcda *.gcno \
	$(PROJECT) $(COVERAGE_RESULTS) \
	$(GTEST) $(MEMCHECK_RESULTS) $(COVERAGE_DIR)  \
	$(PROJECT).exe $(GTEST).exe

# compilation using the files in include, src, and src/project, but not test
.PHONY: $(PROJECT)
$(PROJECT): $(SRC_DIR)/*.cpp
	$(CXX) $(CXXFLAGS) -o $(PROJECT) $(INCLUDE) $(SRC_DIR)/*.cpp

profiler: $(SRC_DIR)/*.cpp
	$(ACXX) $(CXXFLAGS) -o $(PROJECT)-profile -a $(SRC_DIR)/profiler.ah $(INCLUDE) $(SRC_DIR)/*.cpp

cacher: $(SRC_DIR)/*.cpp
	$(ACXX) $(CXXFLAGS) -o $(PROJECT)-cache -a $(SRC_DIR)/cacher.ah -a $(SRC_DIR)/profiler.ah $(INCLUDE) $(SRC_DIR)/*.cpp

.PHONY: test
$(GTEST): $(GTEST_DIR)/*.cpp $(SRC_DIR)/fibonacci.cpp
	$(CXX) $(CXXFLAGS) -o $(GTEST) $(INCLUDE) $(GTEST_DIR)/*.cpp $(SRC_DIR)/$(PROJECT).cpp -lgtest


# To perform all tests
.PHONY: allTests
allTests: memcheck coverage docs static style

.PHONY: memcheck
memcheck: $(PROJECT)
	valgrind --tool=memcheck --leak-check=full --error-exitcode=1 $(PROJECT) 20

.PHONY: coverage
coverage: $(GTEST)
	$(GTEST)
	# Determine code coverage
	$(LCOV) --capture --gcov-tool $(GCOV) --directory . --output-file $(COVERAGE_RESULTS) --rc lcov_branch_coverage=1
	# Only show code coverage for the source code files (not library files)
	$(LCOV) --extract $(COVERAGE_RESULTS) */$(SRC_DIR)/* -o $(COVERAGE_RESULTS)
	#Generate the HTML reports
	genhtml $(COVERAGE_RESULTS) --output-directory $(COVERAGE_DIR)
	#Remove all of the generated files from gcov
	rm -f *.gcda *.gcno

.PHONY: static
static: ${SRC_DIR} ${GTEST_DIR}
	${STATIC_ANALYSIS} --verbose --enable=all ${SRC_DIR} ${GTEST_DIR} ${SRC_INCLUDE} --suppress=missingInclude

.PHONY: style
style: ${SRC_DIR} ${GTEST_DIR} ${SRC_INCLUDE} ${PROJECT_SRC_DIR}
	${STYLE_CHECK} ${SRC_DIR}/* ${GTEST_DIR}/* ${SRC_INCLUDE}/* ${PROJECT_SRC_DIR}/*

.PHONY: docs
docs: ${SRC_INCLUDE}
	doxygen $(DOXY_DIR)/doxyfile
