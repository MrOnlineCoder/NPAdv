CPP=g++
CFLAGS=\
	-Iinclude\
	-Ithirdparty/lua-5.4.6/src\
	-Ithirdparty\
	-g\
	-std=c++11\

LDFLAGS=\
	-lsfml-system-d\
	-lsfml-window-d\
	-lsfml-graphics-d\
	-lsfml-audio-d\
	-lsfml-network-d\
	-llua\

OBJ_FILES=obj/Main.o obj/Application.o\
	obj/Logger.o\
	obj/AssetManager.o\
	obj/StateManager.o\
	obj/MenuState.o\
	obj/Animation.o\
	obj/AnimatedSprite.o\
	obj/IntroState.o\
	obj/Story.o\
	obj/PlayState.o\
	obj/DialogueUI.o

OUT=npadv

obj/Main.o: src/Main.cpp
	${CPP} $(CFLAGS) -o obj/Main.o -c src/Main.cpp

obj/Logger.o: src/Common/Logger.cpp
	${CPP} $(CFLAGS) -o obj/Logger.o -c src/Common/Logger.cpp

obj/Application.o: src/Application.cpp include/Application.hpp
	${CPP} $(CFLAGS) -o obj/Application.o -c src/Application.cpp

obj/AssetManager.o: src/Resources/AssetManager.cpp include/Resources/AssetManager.hpp
	${CPP} $(CFLAGS) -o obj/AssetManager.o -c src/Resources/AssetManager.cpp

obj/StateManager.o: src/State/StateManager.cpp include/State/StateManager.hpp
	${CPP} $(CFLAGS) -o obj/StateManager.o -c src/State/StateManager.cpp

obj/MenuState.o: src/State/MenuState.cpp include/State/MenuState.hpp
	${CPP} $(CFLAGS) -o obj/MenuState.o -c src/State/MenuState.cpp

obj/Animation.o: src/Render/Animation.cpp include/Render/Animation.hpp
	${CPP} $(CFLAGS) -o obj/Animation.o -c src/Render/Animation.cpp

obj/AnimatedSprite.o: src/Render/AnimatedSprite.cpp include/Render/AnimatedSprite.hpp
	${CPP} $(CFLAGS) -o obj/AnimatedSprite.o -c src/Render/AnimatedSprite.cpp

obj/MathUtils.o: src/Common/MathUtils.cpp include/Common/MathUtils.hpp
	${CPP} $(CFLAGS) -o obj/MathUtils.o -c src/Common/MathUtils.cpp

obj/IntroState.o: src/State/IntroState.cpp include/State/IntroState.hpp
	${CPP} $(CFLAGS) -o obj/IntroState.o -c src/State/IntroState.cpp

obj/PlayState.o: src/State/PlayState.cpp include/State/PlayState.hpp
	${CPP} $(CFLAGS) -o obj/PlayState.o -c src/State/PlayState.cpp

obj/Story.o: src/Game/Story.cpp include/Game/Story.hpp
	${CPP} $(CFLAGS) -o obj/Story.o -c src/Game/Story.cpp

obj/DialogueUI.o: src/Game/DialogueUI.cpp include/Game/DialogueUI.hpp
	${CPP} $(CFLAGS) -o obj/DialogueUI.o -c src/Game/DialogueUI.cpp

mkdir:
	mkdir -p obj/

build: mkdir $(OBJ_FILES) 
	$(CPP) -o $(OUT) $(OBJ_FILES) $(CFLAGS) $(LDFLAGS)

clean:
	rm obj/*.o

run: build
	./$(OUT)

cleanrun: clean run

debug: build
	lldb -o run -- $(OUT)