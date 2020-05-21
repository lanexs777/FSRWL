include ../benchmarks.mk

TESTNAME = newRWlock

all: $(TESTNAME)

$(TESTNAME): $(TESTNAME).cc $(TESTNAME).h
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS) -std=c++11

clean:
	rm -f $(TESTNAME) *.o
