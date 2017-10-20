obj=lidar.o rawdata.o grid.o
lidar:$(obj)
	g++ -o lidar $(obj)
lidar.o:lidar.cpp rawdata.h
	g++ -c lidar.cpp lidar.h grid.h
rawdata.o:rawdata.cpp rawdata.h
	g++ -c rawdata.cpp
grid.o:grid.cpp grid.h
	g++ -c grid.cpp

.PHONY:clean
clean:
	-rm lidar grid.h.gch lidar.h.gch $(obj)
