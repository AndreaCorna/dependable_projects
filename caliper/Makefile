CXX=g++
CFLAGS=-c -O3
LDFLAGS=-lm

CALIPER_SRC=src/caliper.cpp src/utils.cpp
ALPHA_SRC=src/computeAlpha.cpp src/utils.cpp
CALIPER_OBJ=$(CALIPER_SRC:.cpp=.o)
ALPHA_OBJ=$(ALPHA_SRC:.cpp=.o)
CALIPER_EX=caliper
ALPHA_EX=computeAlpha

all: $(CALIPER_EX) $(ALPHA_EX)

$(CALIPER_EX): $(CALIPER_OBJ)  
	$(CXX) $(CALIPER_OBJ) $(LDFLAGS) -o $@

$(ALPHA_EX): $(ALPHA_OBJ)  
	$(CXX) $(ALPHA_OBJ) $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@
	
clean:
	rm -rf src/*.o $(CALIPER_EX) $(ALPHA_EX) 
