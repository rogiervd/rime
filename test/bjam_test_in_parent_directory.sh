#!/bin/bash

# Run this from the root directory of the "rime" repository.

# This script will generate a directory ../rime-test and not remove it!
# So watch out where you run it.

# Run the tests by using this repository as submodule of the "rime-test"
# repository.

# This is necessary because this repository cannot be tested by itself.


# Travis CI looks for this line.
set -ev

set -o nounset
set -o errexit

(
    # Check out the "master" branch of "rime-test" from GitHub, in the parent
    # directory.

    cd ../
    git clone git://github.com/rogiervd/rime-test.git
    cd rime-test
    git checkout master
    git submodule init
    git submodule update
    # Then replace the "rime" submodule with the one in ../rime.
    rm -r rime
    ln -s ../rime

    # Test it
    bjam test "$@"

)
