CFLAGS=-Wall -fPIC
LDFLAGS=-mwindows
LIBS=-lComdlg32

all: keystroke

keystroke: keystroke.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f keystroke
	rm -f keystroke.o

.PHONY: clean
