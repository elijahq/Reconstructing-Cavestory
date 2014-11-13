compile:
	clang++ -o gen/cavestory src/**.cc -Wall -Wextra `sdl-config --cflags --libs`
run:
	gen/cavestory
