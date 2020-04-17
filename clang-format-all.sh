#!/bin/sh

find ./src -type f \( -name '*.hpp' -o -name '*.cpp' \) -exec clang-format -i --verbose {} \;
