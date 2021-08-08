# a mak file
CC = cc
AR = ar
OPTMZ3 = -O3
OPTMZ0 = -O
CFLAGS = -std=c99 -I $(include_dir) -I $(posix_include) -I $(include_openssl) -I $(adt_include)
ARFLAGS = rcs

source = $(shell pwd)/src
include_dir = $(source)/headers
adt_include = $(source)/headers/ADT
posix_include = $(source)/posix/pheaders
include_openssl = $(shell pwd)/opensslHeader
lib_dir = $(shell pwd)/libs

OBJ_LPUTL := $(source)/posix/wrappers.o $(source)/posix/errHandler.o $(source)/posix/sigHandlerUtl.o $(source)/posix/sioUtl.o
OBJ_TEST := $(source)/calTest.c $(source)/jsmn.c $(source)/logger.c $(source)/mode.c $(source)/primeNTest.c $(source)/utl1.c src/utl2.c $(source)/utl3.c
OBJ_PGEN := $(source)/pGenerator.c $(source)/primeNTest.c $(source)/utl1.c src/utl3.c $(source)/mode.c $(source)/sha/bitArray.c
OBJ_CTEST := $(source)/ctest.c $(source)/primeTable.c $(source)/Euler.c $(source)/Euclid.c $(source)/powerMod.c
OBJ_DEMO := $(source)/ADT/ademo.c $(source)/posix/errHandler.c $(source)/ADT/binTree.c $(source)/ADT/avlTree.c $(source)/ADT/heapSort.c
OBJ_SHAFUTL := $(source)/sha/sha1.c $(source)/sha/sha224-256.c  $(source)/sha/utls.c $(source)/sha/sha384-512.c $(source)/sha/filesha.c
OBJ_B64FUTL := $(source)/sha/base64.c $(source)/sha/fileb64.c 
OBJ_OPENSSLDEMO := $(source)/opensslDemo/ademo.c

all: lputl pgen caltest ctest

lputl: $(OBJ_LPUTL) $(posix_include)/*.h
	$(AR) $(ARFLAGS) $(lib_dir)/libputl.a $(OBJ_LPUTL)

pgen: $(OBJ_PGEN) $(include_dir)/*.h $(OBJ_LPUTL)
	$(CC) $(OPTMZ3) $(CFLAGS) -o pgen $(OBJ_PGEN) -lputl

caltest: $(OBJ_TEST) $(include_dir)/*.h
	$(CC) $(OPTMZ3) $(CFLAGS) -o caltest $(OBJ_TEST)

ctest: $(OBJ_CTEST) $(include_dir)/littlePrime.h
	$(CC) $(OPTMZ3) $(CFLAGS) -o ctest $(OBJ_CTEST)

demo : $(OBJ_DEMO)
	$(CC) $(OPTMZ0) $(CFLAGS) -o demo $(OBJ_DEMO)

shafutl : $(OBJ_SHAFUTL)
	$(CC) $(OPTMZ0) $(CFLAGS) -o shafutl $(OBJ_SHAFUTL)

b64futl : $(OBJ_B64FUTL)
	$(CC) $(OPTMZ0) $(CFLAGS) -o b64futl $(OBJ_B64FUTL)

ossl : $(OBJ_OPENSSLDEMO) $(include_dir)/*.h
	$(CC) $(OPTMZ0) $(CFLAGS) -g -o ossldemo $(OBJ_OPENSSLDEMO) $(lib_dir)/libcrypto.dylib $(lib_dir)/libssl.dylib

clean:
	rm -f *.O
	rm -f *.s
	rm -f *.out
	rm pgen caltest ctest demo shafutl

.PHONY: clean