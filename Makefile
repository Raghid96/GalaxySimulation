galsim: galsim.c read_file.h
	gcc -O3  -march=native -o galsim galsim.c -lm -pthread
clean:
	rm galsim
