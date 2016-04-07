###--- OS specific configurations
ifeq ($(OS),Windows_NT)
	EXE := .exe
endif
##-----------------------------------------------------------------------------

TGT := car$(EXE)
UNIT_SRCS := route_type.cpp spot.cpp trip.cpp trip_histogram.cpp
ALL_SRCS := main.cpp $(UNIT_SRCS)
ALL_OBJS := $(patsubst %.cpp,%.o,$(ALL_SRCS))

TEST_SRCS := $(patsubst %.cpp,%_utest.cpp,$(UNIT_SRCS))
TEST_OBJS := $(patsubst %.cpp,%.o,$(TEST_SRCS))
TEST_TGTS := $(patsubst %.cpp,test_%$(EXE),$(UNIT_SRCS))

CXXFLAGS += -g -Wall -std=c++11

TRASHES := $(TGT)  $(ALL_OBJS)  test$(EXE) $(TEST_TGTS)  $(TEST_OBJS)


.DEFAULT :  all

##-----------------------------------------------------------------------------
## Dependencies

# headers
main.cpp           :  trace.h
route_type.cpp     :  route_type.h trace.h
spot.cpp           :  spot.h
trip_histogram.cpp :  trip_histogram.h
trip.cpp           :  trip.h trace.h

# linked objects
test_trip$(EXE) :  route_type.o spot.o
test_trip_histogram$(EXE) :  route_type.o trip.o

##-----------------------------------------------------------------------------
## Sources & Headers

%.cpp : %.h 
	@touch $@
	echo $@ $^

##-----------------------------------------------------------------------------
## Objects

%.o :  %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

##-----------------------------------------------------------------------------
## Test binaries

# all tests
test$(EXE) :  $(filter-out main.o,$(ALL_OBJS))  $(TEST_OBJS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -lgtest -lpthread $^ -o $@

# unit test
test_%$(EXE) :  %_utest.o %.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -lgtest -lpthread $^ -o $@

##-----------------------------------------------------------------------------
## Binaries

# release binary
release :  clean $(TGT)
release :  CXXFLAGS += -DNDEBUG
release :  LDFLAGS += -static-libgcc

# debug target
$(TGT) :  $(ALL_OBJS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@

##-----------------------------------------------------------------------------
## Run programs

# run test program
run_% :  %$(EXE)
	./$<

# run debug target with input
run :  $(TGT)
	./$< < examples/trip.txt



all : $(TGT) test$(EXE) $(TEST_TGTS)

##-----------------------------------------------------------------------------
## Cleanup

clean :
	$(RM) $(TRASHES)

.PHONY : all release run test clean
