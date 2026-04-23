all:
	gcc -Iinclude/ src/main.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lm -o ShadowHunter.exe -Wall -Wextra -Wpedantic
	./ShadowHunter.exe

linux:
	gcc -Iinclude/ src/main.c -lSDL2 -lSDL2_image -lGL -lm -o ShadowHunter -Wall -Wextra -Wpedantic
	chmod +x ShadowHunter && ./ShadowHunter