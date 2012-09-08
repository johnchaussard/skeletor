
PINK_SRC = $(PINK)/src/lib
PINK_INC = $(PINK)/include
PINK_OBJ = $(PINK)/linux/obj

OBJ_FILES = $(wildcard $(PINK_OBJ)/*.o)


all: $(OBJ_FILES) libpink.a

libpink.so: $(OBJ_FILES)
	gcc -shared -Wl,-soname,$@ -o $@ $^ -lc

libpink.a: $(OBJ_FILES)
	ar rcs $@ $^

clean:
	rm -rf libpink.so libpink.a

