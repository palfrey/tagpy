#!/bin/bash

set -eux -o pipefail
ver=$1

if [ ! -f wheelhouse/tagpy-*-cp3$ver* ]; then
    python3.10 -m pip install -r requirements-dev.txt
    yum install -y utf8cpp-devel
    python3.10 ./src/builder.py --python-version 3.$ver --taglib-version=2.0.2 --build-wheel
fi
