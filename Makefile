# a mak file
CC = cc
AR = ar
OPTMZ3 = -O3
OPTMZ0 = -O
CFLAGS = -std=c99 -I $(include_dir) -I $(posix_include) -L $(lib_dir)
ARFLAGS = rcs

source = $(shell pwd)/src
include_dir = $(source)/headers
posix_include = $(source)/posix/pheaders
lib_dir = $(shell pwd)/libs

OBJ_LPUTL := $(source)/posix/wrappers.o $(source)/posix/errHandler.o $(source)/posix/sigHandlerUtl.o $(source)/posix/sioUtl.o
OBJ_TEST := $(source)/calTest.c $(source)/jsmn.c $(source)/logger.c $(source)/mode.c $(source)/primeNTest.c $(source)/utl1.c src/utl2.c $(source)/utl3.c
OBJ_PGEN := $(source)/pGenerator.c $(source)/primeNTest.c $(source)/utl1.c src/utl3.c $(source)/mode.c
OBJ_CTEST := $(source)/ctest.c $(source)/primeTable.c $(source)/Euler.c $(source)/Euclid.c $(source)/powerMod.c
OBJ_SHADEMO := $(source)/sha/sha1.c $(source)/sha/sha224-256.c $(source)/jsmn.c  $(source)/sha/utls.c $(source)/sha/sha384-512.c $(source)/sha/base64.c $(source)/sha/demo.c 
OBJ_SHAFUTL := $(source)/sha/sha1.c $(source)/sha/sha224-256.c  $(source)/sha/utls.c $(source)/sha/sha384-512.c $(source)/sha/filesha.c
OBJ_B64FUTL := $(source)/sha/base64.c $(source)/sha/fileb64.c 

all: lputl pgen caltest ctest

lputl: $(OBJ_LPUTL) $(posix_include)/*.h
	$(AR) $(ARFLAGS) $(lib_dir)/libputl.a $(OBJ_LPUTL)

pgen: $(OBJ_PGEN) $(include_dir)/*.h $(OBJ_LPUTL)
	$(CC) $(OPTMZ3) $(CFLAGS) -o pgen $(OBJ_PGEN) -lputl

caltest: $(OBJ_TEST) $(include_dir)/*.h
	$(CC) $(OPTMZ3) $(CFLAGS) -o caltest $(OBJ_TEST)

ctest: $(OBJ_CTEST) $(include_dir)/littlePrime.h
	$(CC) $(OPTMZ3) $(CFLAGS) -o ctest $(OBJ_CTEST)

shademo : $(OBJ_SHADEMO)
	$(CC) $(OPTMZ0) $(CFLAGS) -o shademo $(OBJ_SHADEMO)

shafutl : $(OBJ_SHAFUTL)
	$(CC) $(OPTMZ0) $(CFLAGS) -o shafutl $(OBJ_SHAFUTL)

b64futl : $(OBJ_B64FUTL)
	$(CC) $(OPTMZ0) $(CFLAGS) -o b64futl $(OBJ_B64FUTL)

clean:
	rm -f *.O
	rm -f *.s
	rm -f *.out
	rm pgen caltest ctest shademo shafutl

.PHONY: clean