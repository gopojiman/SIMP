EXEBIN = simp

$(EXEBIN) : *.h *.cpp
	clang++ -std=c++17 *.cpp -o $(EXEBIN)

clean :
	- rm $(EXEBIN)
	- rm -r $(EXEBIN).dSYM

test :
	@ cd tests_in && for i in *; do \
		../$(EXEBIN) $$i | diff ../tests_out/$$i - \
			&& echo $$'\e[1;32m'Pass: $$i$$'\e[0m' \
			|| echo $$'\e[1;31m'Fail: $$i$$'\e[0m'; \
	done