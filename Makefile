compile:
	clang++ -o gen/cavestory src/**.cc -std=c++11 -Wall -Wextra `sdl-config --cflags --libs`
run:
	gen/cavestory
