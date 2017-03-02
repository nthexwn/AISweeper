CC := gcc
CFLAGS := -std=c11 -Wall
SRC_DIR := src/c/
BUILD_DIR := build/

#Explanation for mkdir command:                                                                                   \
Before we can place objects in the build directory we need to ensure that the directory exists.  Placing this     \
command at the top of the file ensures that the directory will be generated whenever this makefile is parsed.     \

$(shell mkdir -p $(BUILD_DIR))

#Explanations for header search patterns:                                                                         \
By utilizing the 'wildcard' function which is built in to make we can search for file names which match the       \
provided pattern.  In our case this pattern will search for files within the source directory which begin with    \
category descriptors (IE: client, common, server) and end in '.h'.  This allows us to separate the headers into   \
differnt categories for our build targets so that targets only need to be updated when their associated header    \
files are modified.                                                                                               \

HEADERS_CLIENT := $(wildcard $(SRC_DIR)client_*.h)
HEADERS_COMMON := $(wildcard $(SRC_DIR)common_*.h)
HEADERS_SERVER := $(wildcard $(SRC_DIR)server_*.h)

#Explanations for object definition patterns:                                                                     \
By utilizing the 'patsubst' function which is built in to make we can perform a pattern substitution operation    \
on the output of the wildcard function to generate an object file name which is based on the original source      \
file name.  We cannot perform this substitution directly on the headers variables since not every header file     \
will have an associated source file (and visa versa).  In our case, the substitution generates an object file     \
name in the build directory for every corresponding source file name in the source directory.                     \

OBJECTS_CLIENT := $(patsubst $(SRC_DIR)client_%.c, $(BUILD_DIR)client_%.o, $(wildcard $(SRC_DIR)client_*.c))
OBJECTS_COMMON := $(patsubst $(SRC_DIR)common_%.c, $(BUILD_DIR)common_%.o, $(wildcard $(SRC_DIR)common_*.c))
OBJECTS_SERVER := $(patsubst $(SRC_DIR)server_%.c, $(BUILD_DIR)server_%.o, $(wildcard $(SRC_DIR)server_*.c))

#Explanations for auto-generated object compilation target syntax:                                                \
"%.o: %.c"   - For every dependency "foo.o" listed in another build target generate a new target that depends on  \
               "foo.c".  Whenever foo.c is modified (or foo.o has not been generated yet) foo.o will need to be   \
               re-run either manually or by another target.                                                       \
"$(HEADERS)" - In addition to the source file "foo.c" make sure all of these auto-generated object targets depend \
               on every header file listed in the "HEADERS" variable at the top of this file.  Whenever one of    \
               these header files is modified foo.o will also need to be re-made by re-running this               \
               auto-generated target.                                                                             \
"$(CC)"      - This target will execute the C compiler listed in the "CC" variable at the top of this file.       \
"$(CFLAGS)"  - Pass the flags stored in the "CFLAGS" variable at the top of this file to the C compiler being     \
               executed.                                                                                          \
"-c $<"      - Tell the compiler to compile the source file (-c) which is listed as the first dependency of the   \
               target ($<) into its own object file without linking any other objects (linking the output as an   \
               executable is implied when no -c argument is used).  In this case the first dependency is "%.c"    \
               which corresponds to "foo.c" where "foo.o" is the name of the target.                              \
"-o $@"      - Tell the compiler to output the compiled object (-o) as a file whose name matches the name of the  \
               current target.  Since the current target is auto-generated (%.o) the output file will be named    \
               "foo.o" where "foo.o" is a listed dependency of another target.                                    \

$(BUILD_DIR)client_%.o: $(SRC_DIR)client_%.c $(HEADERS_CLIENT) $(HEADERS_COMMON)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)common_%.o: $(SRC_DIR)common_%.c $(HEADERS_COMMON)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)server_%.o: $(SRC_DIR)server_%.c $(HEADERS_SERVER) $(HEADERS_COMMON)
	$(CC) $(CFLAGS) -c $< -o $@

#Explanations for manually defined executable target syntax:                                                      \
"client"     - This is the manually defined name of the target.                                                   \
"$(OBJECTS)  - This is a list of all the objects which this target depends on as defined in the "OBJECTS"         \
               variable at the top of this file.  Whenever one of these objects is modified (IE: re-compiled)     \
               then this target will need to be re-run either manually or by another target.                      \
"$(CC)"      - This target will execute the C compiler listed in the "CC" variable at the top of this file.       \
"$(CFLAGS)"  - Pass the flags stored in the "CFLAGS" variable at the top of this file to the C compiler being     \
               executed.                                                                                          \
"$(OBJECTS)  - In this case these objects from the "OBJECTS" variable are listed as object files for the compiler \
               to link together into an executable.                                                               \
"-o $@"      - Tell the compiler to output the compiled executable (-o) as a file whose name matches the name of  \
               the current target.                                                                                \

$(BUILD_DIR)client: $(OBJECTS_CLIENT) $(OBJECTS_COMMON)
	$(CC) $(CFLAGS) $(OBJECTS_CLIENT) $(OBJECTS_COMMON) -o $@

$(BUILD_DIR)server: $(OBJECTS_SERVER) $(OBJECTS_COMMON)
	$(CC) $(CFLAGS) $(OBJECTS_SERVER) $(OBJECTS_COMMON) -o $@

#Explanations for aliases:                                                                                        \
These are alias targets.  They allow one or more existing build targets to be run by typing in a different name.  \

client: $(BUILD_DIR)client
server: $(BUILD_DIR)server
all: client server
default: client server

# Explanations for clean target:                                                                                  \
This will run a series of "remove" commands with the "force" flag added so that no user confirmation is needed.   \
The removed files will evidently be the object files (*.o) as well as the exectuable program files.  This is      \
primarily useful for cleaning out unnecessary files prior to working with version control systems or archiving    \
the project.  These files can be easily rebuilt from source code, so this is a harmless action.                   \

clean:
	-rm -f $(BUILD_DIR)*.o
	-rm -f $(BUILD_DIR)client
	-rm -f $(BUILD_DIR)server

