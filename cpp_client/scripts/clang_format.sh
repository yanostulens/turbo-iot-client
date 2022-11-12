#!/bin/bash
echo "Checking file formatting"
	find . -iname *.h -o -iname *.c -o -iname *.cpp -o -iname *.hpp -not -path "./lib/*" \
	| xargs clang-format --dry-run --Werror
	if [ $? -ne 0 ]; then
		echo "The code is not correctly formatted. Please fix any issues reported by clang-format."
        exit 8
    else
        echo "The code is correctly formatted."
	fi
