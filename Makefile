.DEFAULT_GOAL=all

PAPI_DIR=/share/apps/papi/5.4.1

ifeq ($(TRANS),Y)
    TRANSFLAG = -DTRANS
endif

CC = icpc
CFLAGS = -O3 -std=c++11 -qopt-report=2 -o bin/matrix_mult.o
FLAGS = -D$(IMPL) -D$(M_SIZE) $(TRANSFLAG)
PAPIFLAGS = -L$(PAPI_DIR)/lib -I$(PAPI_DIR)/include -lpapi

clean:
	-@rm bin/*.o

all:
	$(CC) $(FLAGS) $(PAPIFLAGS) $(CFLAGS) matrix_mult.c
