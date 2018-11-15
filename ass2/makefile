all : bwtencode bwtsearch

.PHONY : all

bwtencode : bwtencode.cpp
	g++ bwtencode.cpp -o bwtencode

bwtsearch : bwtsearch.cpp
	g++ bwtsearch.cpp -o bwtsearch

clean : cleanbwtencode cleanbwtsearch

.PHONY : clean

cleanbwtencode : 
	rm bwtencode

cleanbwtsearch : 
	rm bwtsearch
