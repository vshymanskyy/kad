#!/usr/bin/env sh

find . -path "*test*\.h" -print0 | xargs -0 cxxtestgen -o test.cpp
