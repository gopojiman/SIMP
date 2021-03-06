EXEBIN  = simp
EXEBIN2 = $(EXEBIN)-np

all : $(EXEBIN) $(EXEBIN2)

$(EXEBIN) : *.h *.cpp
	clang++ -std=c++17 *.cpp -o $(EXEBIN)

$(EXEBIN2) : *.h *.cpp
	clang++ -std=c++17 -DNOPARALLEL *.cpp -o $(EXEBIN2)

clean :
	- rm $(EXEBIN)
	- rm -r $(EXEBIN).dSYM
	- rm $(EXEBIN2)
	- rm -r $(EXEBIN2).dSYM

test :
	@ echo "\nPARALLEL:"
	@ cd tests_in && for i in *; do \
		../$(EXEBIN) --full-print $$i | diff ../tests_out/$$i - \
			&& echo $$'\e[1;32m'Pass: $$i$$'\e[0m' \
			|| echo $$'\e[1;31m'Fail: $$i$$'\e[0m'; \
	done
	@ echo "\nNON-PARALLEL:"
	@ cd tests_in && for i in *; do \
		../$(EXEBIN2) --full-print $$i | diff ../tests_out/$$i - \
			&& echo $$'\e[1;32m'Pass: $$i$$'\e[0m' \
			|| echo $$'\e[1;31m'Fail: $$i$$'\e[0m'; \
	done