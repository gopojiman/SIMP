
while : *.h *.cpp
	clang++ -std=c++14 *.cpp -o while

clean :
	- rm while
	- rm -r while.dSYM
