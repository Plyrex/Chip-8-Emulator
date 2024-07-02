all:
	g++ -Isrc/include -Lsrc/lib -o chip8 main.cpp -lmingw32 -lSDL2main -lSDL2