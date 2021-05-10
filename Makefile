
while-ss : *.h *.cpp
	clang++ -std=c++14 *.cpp -o while-ss

clean :
	rm while-ss
