#!/bin/bash

set -eux -o pipefail

yum install -y taglib-devel

for ver in {7..9}
do
    python3.$ver -u setup.py bdist_wheel
    auditwheel repair ./dist/tagpy-*-cp3$ver* # Note not the full name as some have the "m" suffix
done
