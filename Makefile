GCC = g++
GCC_VERSION = $(shell echo `gcc -dumpversion | cut -f1-2 -d.` \>= 5.0 | bc )
CFLAGS += -O2 -std=c++14
SSEFLAGS = -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -march=native
EFLAGS = -lpthread

FILES = version_print \
version_check \
fcmsketch.out fcmplus.out elasticp4.out mrac.out hll.out cusketch.out cmsketch.out pyramid.out univmon.out

all: $(FILES)

version_print: 
	@echo "KERNEL_GCC_VERSION: " $(shell gcc -dumpversion)

version_check:
ifeq ($(GCC_VERSION),0) 
	$(error GCC version is not higher than 5.0)	
endif

fcmsketch.out: fcmsketch.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o fcmsketch.out fcmsketch.cpp $(EFLAGS)

fcmplus.out: fcmplus.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o fcmplus.out fcmplus.cpp $(EFLAGS)

elasticp4.out: elasticp4.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o elasticp4.out elasticp4.cpp

mrac.out: mrac.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o mrac.out mrac.cpp

hll.out: hll.cpp
	$(GCC) $(CFLAGS) -o hll.out hll.cpp

cusketch.out: cusketch.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o cusketch.out cusketch.cpp

cmsketch.out: cmsketch.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o cmsketch.out cmsketch.cpp

pyramid.out: pyramid.cpp
	$(GCC) $(CFLAGS) -lm -w -o pyramid.out pyramid.cpp

univmon.out: univmon.cpp
	$(GCC) $(CFLAGS) -o univmon.out univmon.cpp

clean:
	rm $(all) -f *~ *.o *.out


mrac_zl.out: mrac_zl.cpp
	$(GCC) $(CFLAGS) $(SSEFLAGS) -o mrac_zl.out mrac_zl.cpp
