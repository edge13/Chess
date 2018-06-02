SRC="Source"
BUILD="Build"
LIB="Lib"

chess: main.o BMPLoader.o bot.o chess.o geometry.o mesh.o sound.o
	g++ -g -o chess $(BUILD)/main.o $(BUILD)/BMPLoader.o $(BUILD)/bot.o $(BUILD)/chess.o $(BUILD)/geometry.o $(BUILD)/mesh.o $(BUILD)/sound.o -framework OpenGL -framework GLUT -lm Lib/libfmod.dylib -rpath Lib/

main.o:
	gcc -c $(SRC)/main.cpp  -o $(BUILD)/main.o

BMPLoader.o:
	gcc -c $(SRC)/BMPLoader.cpp  -o $(BUILD)/BMPLoader.o

bot.o:
	gcc -c $(SRC)/bot.cpp  -o $(BUILD)/bot.o

chess.o:
	gcc -c $(SRC)/chess.cpp  -o $(BUILD)/chess.o

geometry.o:
	gcc -c $(SRC)/geometry.cpp  -o $(BUILD)/geometry.o

mesh.o:
	gcc -c $(SRC)/mesh.cpp -o $(BUILD)/mesh.o

sound.o:
	gcc -c $(SRC)/sound.cpp -o $(BUILD)/sound.o

clean:
	rm -f $(BUILD)/*
	rm chess