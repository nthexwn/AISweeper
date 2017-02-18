CC = gcc
CFLAGS = -std=c11 -Wall
HEADERS = constants.h mt19937.h server_game.h server_interface.h utilities.h
OBJECTS = constants.o mt19937.o server_game.o server_interface.o server_main.o utilities.o

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
               "foo.o" where "foo.c" (%.c) is a listed dependency of another target.                              \

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

#Explanations for manually defined executable target syntax:                                                      \
"exe"        - This is the manually defined name of the target.                                                   \
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

exe: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

#Explanations for aliases:                                                                                        \
These are "alias" targets that depend on the exe target.  This means that exe will be run when the aliases are    \
run.  They are primarily here for the sake of convention, since some people are used to typing "make all" or      \
"make default" regardless of the project which they are working on.  These also serve to remind me of how make    \
works since I will probably forget the difference between features, syntax, and convention within makefiles in    \
the future.                                                                                                       \

all: exe
default: exe

# Explanations for clean target:                                                                                  \
This will run two separate "remove" commands with the "force" flag added so that no user confirmation is needed.  \
The removed files will evidently be the object files (*.o) as well as the executable file itself.  This is        \
primarily useful for cleaning out unnecessary files prior to working with version control systems or archiving    \
the project.  These files can be easily rebuilt from source code, so this is a harmless action.                   \

clean:
	-rm -f $(OBJECTS)
	-rm -f exe

