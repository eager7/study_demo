default:test

SOURCE:=json_object.c debug.c arraylist.c json_c_version.c json_object_iterator.c json_tokener.c json_util.c libjson.c linkhash.c printbuf.c 
test.o:test.c test.h

#OBJS := $(SOURCE:.c=.o)

OBJS+=test.o

test:$(OBJS)
	#echo $(OBJS)
	gcc $< -o $@ -ljson-c 
clean:
	rm test $(OBJS)
