#######################################################################

LIBTIFFPATH=$(HOME)/$(DICE)usr

LIBXML2_FLAGS=`xml2-config --libs --cflags`
LIBGTS_FLAGS=`gts-config --libs --cflags`

#######################################################################

pvmbuild: pvmbuild.cpp rgbhsv.o PVModelFile.o
	g++ $(CFLAGS) -I$(LIBTIFFPATH)/include/ -L$(LIBTIFFPATH)/lib/ \
	-ltiff $(LIBXML2_FLAGS) $(LIBGTS_FLAGS) \
	pvmbuild.cpp rgbhsv.o PVModelFile.o -o pvmbuild

pvmtest: PVModelFile.cpp
	g++ -DTEST_PVMODELFILE PVModelFile.cpp -o pvmtest

debug: 
	make pvmbuild "CFLAGS=-g"
	
#######################################################################

clean:
	rm -f pvmbuild *.o *.*~
	
#######################################################################
