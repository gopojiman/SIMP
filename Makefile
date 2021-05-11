
simp : *.h *.cpp
	clang++ -std=c++14 *.cpp -o simp

clean :
	- rm simp
	- rm -r simp.dSYM
