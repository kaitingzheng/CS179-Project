Mac:
	clang++ main.cpp -o Dock -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2
Other:
	clang++ main.cpp -o Dock -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 