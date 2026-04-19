#!/bin/bash

set -eux -o pipefail

for ver in {10..14}
do
    ./build-manylinux-each.sh $ver
done
