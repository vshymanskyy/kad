#!/usr/bin/env sh

#perf ./Debug/kad
#valgrind --tool=memcheck ./Debug/kad 2&> memcheck.log
valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./Debug/kad StringTS
#valgrind --tool=cachegrind --cachegrind-out-file=cachegrind.out ./Debug/kad #2> cachegrind.log