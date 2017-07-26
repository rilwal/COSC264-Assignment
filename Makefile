
reciever_source_dir=reciever/src
sender_source_dir=sender/src
channel_source_dir=channel/src
common_source_dir=common/src

reciever_build_dir=reciever/build
sender_build_dir=sender/build
channel_build_dir=channel/build

reciever_include_dir=reciever/include
sender_include_dir=sender/include
channel_include_dir=channel/include
common_include_dir=common/include

reciever_compiler_libs=
sender_compiler_libs=

common_source_files = $(wildcard $(common_source_dir)/*.cpp)
reciever_source_files = $(common_source_files) $(wildcard $(reciever_source_dir)/*.cpp)
sender_source_files = $(common_source_files) $(wildcard $(sender_source_dir)/*.cpp)
channel_source_files = $(common_source_files) $(wildcard $(channel_source_dir)/*.cpp)

reciever_obj_files = $(patsubst src/%.cpp,obj/%.o,$(reciever_source_files))
sender_obj_files = $(patsubst src/%.cpp,obj/%.o,$(sender_source_files))
channel_obj_files = $(patsubst src/%.cpp,obj/%.o,$(channel_source_files))

ifdef SCHOOL
	cxx_compiler = g++
	common_compiler_flags = -g -std=c++14 -Wall -Werror -I$(common_include_dir)
else
	cxx_compiler = clang++
	common_compiler_flags = -g --std=c++14 -Wall -Werror -I$(common_include_dir)
endif

reciever_compiler_flags = $(common_compiler_flags) -I$(reciever_include_dir)
sender_compiler_flags = $(common_compiler_flags) -I$(sender_include_dir)
channel_compiler_flags = $(common_compiler_flags) -I$(channel_include_dir)

bin/reciever: $(reciever_obj_files)
	mkdir -p $(reciever_build_dir)
	$(cxx_compiler) $(reciever_compiler_flags) $(reciever_compiler_libs) -o $@ $^

bin/channel: $(channel_obj_files)
	mkdir -p $(channel_build_dir)
	$(cxx_compiler) $(channel_compiler_flags) $(channel_compiler_libs) -o $@ $^


channel/obj/%.o: channel/src/%.cpp
	mkdir -p channel/obj
	$(cxx_compiler) $(channel_compiler_flags) -c -o $@ $<


reciever/obj/%.o: reciever/src/%.cpp
	mkdir -p reciever/obj
	$(cxx_compiler) $(reciever_compiler_flags) -c -o $@ $<


bin/sender: $(sender_obj_files)
	mkdir -p $(sender_build_dir)
	$(cxx_compiler) $(sender_compiler_flags) $(sender_compiler_libs) -o $@ $^


sender/obj/%.o: sender/src/%.cpp
	mkdir -p sender/obj
	$(cxx_compiler) $(sender_compiler_flags) -c -o $@ $<


run: bin/sender bin/reciever
	bin/sender &
	bin/reciever
	kill $(pidof sender)

all: bin/sender bin/channel bin/reciever
	echo Done!
