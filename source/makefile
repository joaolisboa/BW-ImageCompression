 # Easily adaptable makefile for Advanced Programming
# Note: remove comments (#) to activate some features
#
# author Vitor Carreira
# date 2010-09-26


# Libraries to include (if any)
LIBS=-pthread -lm

# Compiler flags
CFLAGS=-Wall -W -Wmissing-prototypes

# Indentation flags
IFLAGS=-br -brs -npsl -ce -cli4


# Name of the executable
PROGRAM=pacodec

# Prefix for the gengetopt file (if gengetopt is used)
PROGRAM_OPT=prog_opt

# Object files required to build the executable
PROGRAM_OBJS=main.o debug.o memory.o signal_handler.o file_handler.o pgm_handler.o cod_handler.o dict_handler.o encode.o decode.o validations.o psnr.o memory_handler.o time_handler.o ${PROGRAM_OPT}.o

# Clean and all are not files
.PHONY: clean all docs indent debugon

all: ${PROGRAM}

# compilar com depuracao
debugon: CFLAGS += -D SHOW_DEBUG -g
debugon: ${PROGRAM}

${PROGRAM}: ${PROGRAM_OBJS}
	${CC} -o $@ ${PROGRAM_OBJS} ${LIBS}

# Dependencies
main.o: main.c debug.h memory.h signal_handler.h file_handler.h cod_handler.h pgm_handler.h dict_handler.h encode.h decode.h validations.h psnr.h memory_handler.h time_handler.h ${PROGRAM_OPT}.h
${PROGRAM_OPT}.o: ${PROGRAM_OPT}.c ${PROGRAM_OPT}.h

debug.o: debug.c debug.h
signal_handler.o: signal_handler.c signal_handler.h
pgm_handler.o: pgm_handler.c pgm_handler.h
cod_handler.o: cod_handler.c cod_handler.h
dict_handler.o: dict_handler.c dict_handler.h
file_handler.o: file_handler.c file_handler.h
encode.o: encode.c encode.h
decode.o: decode.c decode.h
validations.o: validations.c validations.h
psnr.o: psnr.c psnr.h
memory_handler.o: memory_handler.c memory_handler.h
time_handler.o: time_handler.c time_handler.h

#how to create an object file (.o) from C file (.c)
.c.o:
	${CC} ${CFLAGS} -c $<

# Generates command line arguments code from gengetopt configuration file
${PROGRAM_OPT}.h: ${PROGRAM_OPT}.ggo
	gengetopt < ${PROGRAM_OPT}.ggo --file-name=${PROGRAM_OPT}

clean:
	rm -f *.o core.* *~ ${PROGRAM} *.bak ${PROGRAM_OPT}.h ${PROGRAM_OPT}.c

docs: Doxyfile
	doxygen Doxyfile

Doxyfile:
	doxygen -g Doxyfile

indent:
	indent ${IFLAGS} *.c *.h