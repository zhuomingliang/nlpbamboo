
CPPFLAGS = -g -fPIC -Wall $(INC) -I../../build/opt/bamboo/include
CC = g++ $(CPPFLAGS)
LIBS = -L../../build/opt/bamboo/lib -lbamboo

TARGET = regress_seg regress_ner

all : $(TARGET)

regress_seg : regress_seg.o
	$(CC) $^ -o $@ $(LIBS)

regress_ner : regress_ner.o
	$(CC) $^ -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
