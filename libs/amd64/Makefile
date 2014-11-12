LIBFTDI=libftd2xx.a
SHAPI=sh_api.a
EXE=test_shapi

all: $(EXE)

$(EXE):
	g++ -c CUSBSA.cpp -o CUSBSA.o
	g++ main.cpp CUSBSA.o $(SHAPI) $(LIBFTDI) -o $(EXE) -lpthread -ldl -lrt
clean:
	rm -f *.o
	rm -f *~
	rm -f $(EXE)
	rm -f *.plot