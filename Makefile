OUTPUT_BINARY_NAME=imgviewer


OFILES = \
		base/Matrix.o \
		base/LinearAlgebra.o \
		base/StarterViewer.o \
		base/ImgProc.o \
		base/Stencil.o \
		base/Point.o \
		base/ColorLUT.o \
		base/JuliaSet.o \
		base/ImgOperations.o 
		




ROOTDIR = .
LIB = $(ROOTDIR)/lib/libstarter.a 


GLLDFLAGS     = -lglut -lGL -lm -lGLU


CXX = g++ -Wall -g -O2 -fPIC $(DEFINES) -fopenmp -std=c++14

CUSTOM_INCLUDE = -I/home/sorliog/.build/include
CUSTOM_LIB=-L/home/sorliog/.build/lib


SWIGLD = $(CXX) -shared
SWIGEXEC = swig


INCLUDES =  -I ./include/ -I /usr/local/include/ -I/usr/include/ $(CUSTOM_INCLUDE)




.C.o:
	$(CXX) -c $(INCLUDES) $< -o $@

base: $(OFILES)
	ar rv $(LIB) $?
	$(CXX) base/imgviewer.C $(INCLUDES)  -L./lib -lstarter $(GLLDFLAGS) -L/usr/local/lib $(CUSTOM_LIB) -lOpenImageIO -lOpenImageIO_Util -o bin/$(OUTPUT_BINARY_NAME)

clean:
	rm -rf bin/imgviewer doc/html *.o base/*.o base/*~ include/*~ python/*~ $(LIB)  *~ swig/*.cxx swig/*~ swig/*.so swig/*.o swig/StarterViewer.py swig/*.pyc ./*.pyc python/*StarterViewer*

.PHONY: doc
doc:
	cd doc;doxygen Doxyfile


