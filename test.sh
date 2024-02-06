#!/usr/bin/sh

#
# Tests for b3.h.
#

set -xe

gcc -Wall -Wextra -ansi -pedantic -g tests.c -o tests
./tests
