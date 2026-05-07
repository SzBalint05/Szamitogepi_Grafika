all:
	gcc -Iinclude/ src/app.c src/camera.c src/main.c src/scene.c src/utils.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32 -lm -o ShadowHunter.exe -Wall -Wextra -Wpedantic
	./ShadowHunter.exe

linux:
	gcc -Iinclude/ src/app.c src/camera.c src/main.c src/scene.c src/utils.c -lSDL2 -lSDL2_image -lGL -lGLU -lm -o ShadowHunter -Wall -Wextra -Wpedantic
	chmod +x ShadowHunter && ./ShadowHunter