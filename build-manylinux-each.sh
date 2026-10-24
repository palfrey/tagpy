#!/bin/bash

set -eux -o pipefail
ver=$1

if [ ! -f wheelhouse/tagpy-*-cp3$ver* ]; then
    uv sync --python 3.10
    yum install -y utf8cpp-devel
    .venv/bin/python3.10 ./src/builder.py --python-version 3.$ver --taglib-version=2.0.2 --build-wheel
fi
