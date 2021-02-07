APP=kernel_filter_2d
CXX=g++
SAIL=../sail/install
SAILLIB=$(SAIL)/lib
LDFLAGS=-L$(SAILLIB) -lsail-c++ -lsail -lsail-common -lsail-codecs-objects -lsail-codecs
CXXFLAGS=-isystem$(SAIL)/include/sail -std=c++20 -O0 -g
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

$(APP): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean print all

all: $(APP)

clean:
	-rm *.o $(APP)

print:
	$(info OBJ=$(OBJ))
	$(info SRC=$(SRC))

