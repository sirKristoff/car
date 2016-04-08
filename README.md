# car

Statistics of car's driving trips


## Dependencies

Only for unit test purposes [Google test framework] (https://github.com/google/googletest) and
[POSIX threads] (https://computing.llnl.gov/tutorials/pthreads/) are needed.

### Providing GTest libraries

GTest include headers must be accessable by C++ compiler without specifying additional `-I` option flag.
It may be achieved by copying them into system `/usr/include/` directory on linux.

Compiled GTest libraries with main function have to be accessable by C++ compiler through `-lgtest` option flag (like in commanand below).
```make
    g++ -lgtest -lpthread $^ -o $@
```

## Build system

System based on [GNU make] (https://www.gnu.org/software/make/)

### Targets:

#### Release targets
- `release`  -  build release ready application 

#### Production code targets
- `car`  -  build debug application
- `run`  -  run debug application with some example imput

#### Test targets
- `test`  -  build program aggregating tests for all units
- `test_<module>`  -  build program containing unit tests for `<module>` unit
- `run_test  -  run test program aggregating tests for all units
- `run_test_<module>`  - run test program for `<module>` unit

#### General
- `all`  -  build debug application and all possible tests
- `clean`  -  remove all building products (e.q. object files, binaries)
