
CPPFLAGS = -g -fPIC -Wall $(INC) -I/opt/bamboo/include
CC = g++ $(CPPFLAGS)
LIBS = -L/opt/bamboo/lib -lbamboo

TARGET = regress_seg regress_ner regress_keyword

all : $(TARGET)

regress_seg : regress_seg.o
	$(CC) $^ -o $@ $(LIBS)

regress_ner : regress_ner.o
	$(CC) $^ -o $@ $(LIBS)

regress_keyword : regress_keyword.o
	$(CC) $^ -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
