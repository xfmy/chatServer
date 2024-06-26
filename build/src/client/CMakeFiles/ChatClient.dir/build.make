# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xf/code/chat-server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xf/code/chat-server/build

# Include any dependencies generated for this target.
include src/client/CMakeFiles/ChatClient.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/client/CMakeFiles/ChatClient.dir/compiler_depend.make

# Include the progress variables for this target.
include src/client/CMakeFiles/ChatClient.dir/progress.make

# Include the compile flags for this target's objects.
include src/client/CMakeFiles/ChatClient.dir/flags.make

src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.o: src/client/CMakeFiles/ChatClient.dir/flags.make
src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.o: ../src/client/chat_session.cpp
src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.o: src/client/CMakeFiles/ChatClient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xf/code/chat-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.o"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.o -MF CMakeFiles/ChatClient.dir/chat_session.cpp.o.d -o CMakeFiles/ChatClient.dir/chat_session.cpp.o -c /home/xf/code/chat-server/src/client/chat_session.cpp

src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ChatClient.dir/chat_session.cpp.i"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xf/code/chat-server/src/client/chat_session.cpp > CMakeFiles/ChatClient.dir/chat_session.cpp.i

src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ChatClient.dir/chat_session.cpp.s"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xf/code/chat-server/src/client/chat_session.cpp -o CMakeFiles/ChatClient.dir/chat_session.cpp.s

src/client/CMakeFiles/ChatClient.dir/main.cpp.o: src/client/CMakeFiles/ChatClient.dir/flags.make
src/client/CMakeFiles/ChatClient.dir/main.cpp.o: ../src/client/main.cpp
src/client/CMakeFiles/ChatClient.dir/main.cpp.o: src/client/CMakeFiles/ChatClient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xf/code/chat-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/client/CMakeFiles/ChatClient.dir/main.cpp.o"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/client/CMakeFiles/ChatClient.dir/main.cpp.o -MF CMakeFiles/ChatClient.dir/main.cpp.o.d -o CMakeFiles/ChatClient.dir/main.cpp.o -c /home/xf/code/chat-server/src/client/main.cpp

src/client/CMakeFiles/ChatClient.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ChatClient.dir/main.cpp.i"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xf/code/chat-server/src/client/main.cpp > CMakeFiles/ChatClient.dir/main.cpp.i

src/client/CMakeFiles/ChatClient.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ChatClient.dir/main.cpp.s"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xf/code/chat-server/src/client/main.cpp -o CMakeFiles/ChatClient.dir/main.cpp.s

src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.o: src/client/CMakeFiles/ChatClient.dir/flags.make
src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.o: ../src/client/tcp_client.cpp
src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.o: src/client/CMakeFiles/ChatClient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/xf/code/chat-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.o"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.o -MF CMakeFiles/ChatClient.dir/tcp_client.cpp.o.d -o CMakeFiles/ChatClient.dir/tcp_client.cpp.o -c /home/xf/code/chat-server/src/client/tcp_client.cpp

src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ChatClient.dir/tcp_client.cpp.i"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/xf/code/chat-server/src/client/tcp_client.cpp > CMakeFiles/ChatClient.dir/tcp_client.cpp.i

src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ChatClient.dir/tcp_client.cpp.s"
	cd /home/xf/code/chat-server/build/src/client && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/xf/code/chat-server/src/client/tcp_client.cpp -o CMakeFiles/ChatClient.dir/tcp_client.cpp.s

# Object files for target ChatClient
ChatClient_OBJECTS = \
"CMakeFiles/ChatClient.dir/chat_session.cpp.o" \
"CMakeFiles/ChatClient.dir/main.cpp.o" \
"CMakeFiles/ChatClient.dir/tcp_client.cpp.o"

# External object files for target ChatClient
ChatClient_EXTERNAL_OBJECTS =

../bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/chat_session.cpp.o
../bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/main.cpp.o
../bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/tcp_client.cpp.o
../bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/build.make
../bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/xf/code/chat-server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable ../../../bin/ChatClient"
	cd /home/xf/code/chat-server/build/src/client && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ChatClient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/client/CMakeFiles/ChatClient.dir/build: ../bin/ChatClient
.PHONY : src/client/CMakeFiles/ChatClient.dir/build

src/client/CMakeFiles/ChatClient.dir/clean:
	cd /home/xf/code/chat-server/build/src/client && $(CMAKE_COMMAND) -P CMakeFiles/ChatClient.dir/cmake_clean.cmake
.PHONY : src/client/CMakeFiles/ChatClient.dir/clean

src/client/CMakeFiles/ChatClient.dir/depend:
	cd /home/xf/code/chat-server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xf/code/chat-server /home/xf/code/chat-server/src/client /home/xf/code/chat-server/build /home/xf/code/chat-server/build/src/client /home/xf/code/chat-server/build/src/client/CMakeFiles/ChatClient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/client/CMakeFiles/ChatClient.dir/depend

