# Very simple. Compile object file aplib.o, and link to app object file. This simple Makefile assumes WIN32 console .exe output.

all: aplib app_front

aplib: ./aplib.c ./aplib.h ./aplib_extra_typedefs.h ./lean_string.h 
    gcc -O0 -g -DDEBUG -c -o ./aplib.o ./aplib.c
    
app_front: ./main.c ./lean_string.h
    gcc -O0 -g -DDEBUG -o ./app.exe ./aplib.o ./main.c 
    
