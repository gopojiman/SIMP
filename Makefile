simp : *.h *.cpp
	clang++ -std=c++17 *.cpp -o simp

clean :
	- rm simp
	- rm -r simp.dSYM

test :
	@ cd tests_in && for i in *; do \
		../simp $$i | diff ../tests_out/$$i - \
			&& echo $$'\e[1;32m'Pass: $$i$$'\e[0m' \
			|| echo $$'\e[1;31m'Fail: $$i$$'\e[0m'; \
	done