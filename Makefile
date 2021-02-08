APP=kernel_filter_2d
CXX=g++
SAIL=$(shell pwd)/external/sail/install
SAILLIB=$(SAIL)/lib
LDFLAGS=-L$(SAILLIB) -L$(SAILLIB)/sail/codecs -Wl,-rpath=$(SAILLIB) -Wl,-rpath=$(SAILLIB)/sail/codecs \
	-lsail-common -lsail -lsail-c++ #-lsail-codec-gif -lsail-codec-jpeg -lsail-codec-png \
	-lsail-codec-tiff
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

