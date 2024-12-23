@echo off
echo compilin calcebonk
cd src
cd resource
del resource.o
windres -i resource.rc -o resource.o
cd ..
cd ..
gcc -o calcebonk src/main.c src/resource/resource.o -mwindows
calcebonk