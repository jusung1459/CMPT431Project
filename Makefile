
ifdef USE_INT
MACRO = -DUSE_INT
endif

#compiler setup
CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14 -O3 -pthread $(MACRO) binaryFileController.cpp

COMMON= core/utils.h core/cxxopts.h core/get_time.h 
SERIAL= sort_serial sort_parallel generateFloats
ALL= $(SERIAL) $(PARALLEL)

all : $(ALL)

$(SERIAL): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f *.o *.obj $(ALL)
