#!/bin/bash

set -eux -o pipefail

yum install -y utf8cpp-devel

for ver in {9..13}
do
    if [ ! -f wheelhouse/tagpy-*-cp3$ver* ]; then
        python3.10 ./src/builder.py --python-version 3.$ver --taglib-version=2.0.2 --build-wheel
    fi
done
