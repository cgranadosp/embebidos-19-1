archivos=huellaFG.bmp
PROYECTO=principal
DESTDIR=/usr/local/bin
CFLAGS=-Wall 
LDFLAGS=-lm -lpthread
CC=gcc
FILES := $(patsubst %.c, %.o, $(wildcard *.c))

all: $(PROYECTO)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

#EQUIVALENTE A LO DE ARRIBA
#muestreo.o: muestreo.c
#	$(CC) -c muestreo.c $(CFLAGS)

#procesamiento.o: procesamiento.c
#	$(CC) -c procesamiento.c $(CFLAGS)

#archivos.o: archivoc.c
#	$(CC) -c archivos.c $(CFLAGS)



$(PROYECTO): $(FILES)
	$(CC) $^ -o $@ $(LDFLAGS)

	
#EN FILES se obtienen todos los archivos con la terminación .c gracias a $(wildcard *-c), 
#luego se sustituye con patsubst,
#se buscan las cadenas con .c y cambia por .o dentro de lo que contiene la variable con wildcard
#con eso tenemos lo siguente:
#$(PROYECTO): muestreo.o procesamiento.o archivos.o
#	$(CC) $^ -o $@ $(LDFLAGS)

#EQUIVALENTE A LO DE ARRIBA
#$(PROYECTO): muestreo.o procesamiento.o archivos.o
#	$(CC) muestreo.o procesamiento.o archivos.o -o $(PROYECTO) $(LDFLAGS)

.PHONY: clean

install: all
	if [ ! -d $(DESTDIR) ] ; then \
		sudo mkdir $(DESTDIR) ; \
	fi; \
	sudo cp $(PROYECTO) $(DESTDIR)
	$(PROYECTO)


uninstall:
	sudo rm $(DESTDIR)/$(PROYECTO)

clean:
	rm -f *.o $(PROYECTO) $(archivos)
