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
include src/flowseeds/CMakeFiles/flowseeds.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/flowseeds/CMakeFiles/flowseeds.dir/compiler_depend.make

# Include the progress variables for this target.
include src/flowseeds/CMakeFiles/flowseeds.dir/progress.make

# Include the compile flags for this target's objects.
include src/flowseeds/CMakeFiles/flowseeds.dir/flags.make

src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o: src/flowseeds/CMakeFiles/flowseeds.dir/flags.make
src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o: src/flowseeds/flowseedsWithA.cpp
src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o: src/flowseeds/CMakeFiles/flowseeds.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yexw/anchored/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o"
	cd /home/yexw/anchored/src/flowseeds && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o -MF CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o.d -o CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o -c /home/yexw/anchored/src/flowseeds/flowseedsWithA.cpp

src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.i"
	cd /home/yexw/anchored/src/flowseeds && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yexw/anchored/src/flowseeds/flowseedsWithA.cpp > CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.i

src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.s"
	cd /home/yexw/anchored/src/flowseeds && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yexw/anchored/src/flowseeds/flowseedsWithA.cpp -o CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.s

# Object files for target flowseeds
flowseeds_OBJECTS = \
"CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o"

# External object files for target flowseeds
flowseeds_EXTERNAL_OBJECTS =

src/flowseeds/libflowseeds.a: src/flowseeds/CMakeFiles/flowseeds.dir/flowseedsWithA.cpp.o
src/flowseeds/libflowseeds.a: src/flowseeds/CMakeFiles/flowseeds.dir/build.make
src/flowseeds/libflowseeds.a: src/flowseeds/CMakeFiles/flowseeds.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yexw/anchored/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libflowseeds.a"
	cd /home/yexw/anchored/src/flowseeds && $(CMAKE_COMMAND) -P CMakeFiles/flowseeds.dir/cmake_clean_target.cmake
	cd /home/yexw/anchored/src/flowseeds && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/flowseeds.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/flowseeds/CMakeFiles/flowseeds.dir/build: src/flowseeds/libflowseeds.a
.PHONY : src/flowseeds/CMakeFiles/flowseeds.dir/build

src/flowseeds/CMakeFiles/flowseeds.dir/clean:
	cd /home/yexw/anchored/src/flowseeds && $(CMAKE_COMMAND) -P CMakeFiles/flowseeds.dir/cmake_clean.cmake
.PHONY : src/flowseeds/CMakeFiles/flowseeds.dir/clean

src/flowseeds/CMakeFiles/flowseeds.dir/depend:
	cd /home/yexw/anchored && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yexw/anchored /home/yexw/anchored/src/flowseeds /home/yexw/anchored /home/yexw/anchored/src/flowseeds /home/yexw/anchored/src/flowseeds/CMakeFiles/flowseeds.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/flowseeds/CMakeFiles/flowseeds.dir/depend
