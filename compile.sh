mkdir -p build
gcc src/main.c -o build/main.o -c
gcc src/ini_parser.c -o build/ini_parser.o -c
gcc -o build/test build/main.o build/ini_parser.o
cp src/test.ini build/
