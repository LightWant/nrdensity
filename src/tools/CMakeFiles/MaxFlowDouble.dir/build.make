# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yexw/anchored

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yexw/anchored

# Include any dependencies generated for this target.
include src/tools/CMakeFiles/MaxFlowDouble.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/tools/CMakeFiles/MaxFlowDouble.dir/compiler_depend.make

# Include the progress variables for this target.
include src/tools/CMakeFiles/MaxFlowDouble.dir/progress.make

# Include the compile flags for this target's objects.
include src/tools/CMakeFiles/MaxFlowDouble.dir/flags.make

src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o: src/tools/CMakeFiles/MaxFlowDouble.dir/flags.make
src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o: src/tools/MaxFlowDouble.cpp
src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o: src/tools/CMakeFiles/MaxFlowDouble.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yexw/anchored/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o"
	cd /home/yexw/anchored/src/tools && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o -MF CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o.d -o CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o -c /home/yexw/anchored/src/tools/MaxFlowDouble.cpp

src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.i"
	cd /home/yexw/anchored/src/tools && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yexw/anchored/src/tools/MaxFlowDouble.cpp > CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.i

src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.s"
	cd /home/yexw/anchored/src/tools && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yexw/anchored/src/tools/MaxFlowDouble.cpp -o CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.s

# Object files for target MaxFlowDouble
MaxFlowDouble_OBJECTS = \
"CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o"

# External object files for target MaxFlowDouble
MaxFlowDouble_EXTERNAL_OBJECTS =

src/tools/libMaxFlowDouble.a: src/tools/CMakeFiles/MaxFlowDouble.dir/MaxFlowDouble.cpp.o
src/tools/libMaxFlowDouble.a: src/tools/CMakeFiles/MaxFlowDouble.dir/build.make
src/tools/libMaxFlowDouble.a: src/tools/CMakeFiles/MaxFlowDouble.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yexw/anchored/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libMaxFlowDouble.a"
	cd /home/yexw/anchored/src/tools && $(CMAKE_COMMAND) -P CMakeFiles/MaxFlowDouble.dir/cmake_clean_target.cmake
	cd /home/yexw/anchored/src/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MaxFlowDouble.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/tools/CMakeFiles/MaxFlowDouble.dir/build: src/tools/libMaxFlowDouble.a
.PHONY : src/tools/CMakeFiles/MaxFlowDouble.dir/build

src/tools/CMakeFiles/MaxFlowDouble.dir/clean:
	cd /home/yexw/anchored/src/tools && $(CMAKE_COMMAND) -P CMakeFiles/MaxFlowDouble.dir/cmake_clean.cmake
.PHONY : src/tools/CMakeFiles/MaxFlowDouble.dir/clean

src/tools/CMakeFiles/MaxFlowDouble.dir/depend:
	cd /home/yexw/anchored && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yexw/anchored /home/yexw/anchored/src/tools /home/yexw/anchored /home/yexw/anchored/src/tools /home/yexw/anchored/src/tools/CMakeFiles/MaxFlowDouble.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/tools/CMakeFiles/MaxFlowDouble.dir/depend
