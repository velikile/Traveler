rm Traveler
g++ -ggdb main.cpp -o Traveler -I/usr/local/include/SDL2 -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2
