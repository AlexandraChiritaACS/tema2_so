gcc -g -Wall -fPIC -c util/so_stdio.c &&
gcc -shared so_stdio.o -o libso_stdio.so &&
cp libso_stdio.so checker-lin/ &&
cd checker-lin/
make -f Makefile.checker
./run_all.sh

