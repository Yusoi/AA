.DEFAULT_GOAL=all

PAPI_DIR=/share/apps/papi/5.4.1

ifeq ($(TRANS),Y)
    TRANSFLAG = -DTRANS
endif

CC = icpc
CFLAGS = -O3 -std=c++11 -qopt-report=2 -o bin/$(FILENAME).o
FLAGS = -D$(IMPL) -D$(M_SIZE) $(TRANSFLAG)
PAPIFLAGS = -L$(PAPI_DIR)/lib -I$(PAPI_DIR)/include -lpapi
VECFLAGS = 
OMPFLAGS = -qopenmp-simd -fopenmp

clean:
	-@rm bin/*.o

omp:
	$(CC) $(FLAGS) $(PAPIFLAGS) $(VECFLAGS) $(OMPFLAGS) $(CFLAGS) matrix_mult_omp.c

vec: 
	$(CC) $(FLAGS) $(PAPIFLAGS) $(VECFLAGS) $(CFLAGS) matrix_mult_vec.c

all:
	$(CC) $(FLAGS) $(PAPIFLAGS) $(CFLAGS) matrix_mult.c
