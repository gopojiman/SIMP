Compiling:
    - Requires a version of clang++ that supports C++17
    - "make" creates two executables: 
        - simp:    the interpreter that uses parallelism
        - simp-np: the interpreter that doesn't use parallelism
    - "make clean" removes the executables
    - "make test" runs some basic tests

Running:
    - Usage: ./simp [--full-print] [inputFilename]
    - providing an input file name reads the program in the given file,
      if no file name is given then the program is read from stdin instead
    - if the "--full-print" option is given, then all elements of arrays are
      printed in the program output. Otherwise, a maximum of SHORT_PRINT_LENGTH
      (defined in Store.cpp, currently 10) elements are printed