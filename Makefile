
simp : *.h *.cpp
	clang++ -std=c++17 *.cpp -o simp

clean :
	- rm simp
	- rm -r simp.dSYM
