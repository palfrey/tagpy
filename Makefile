manylinux2014:
	docker run \
		--rm \
		-v `pwd`:/src \
		--workdir /src \
		quay.io/pypa/manylinux_2_34_x86_64 \
		./build-manylinux.sh

manylinux2014-sh:
	docker run \
		--rm \
		-it \
		-v `pwd`:/src \
		--workdir /src \
		quay.io/pypa/manylinux_2_34_x86_64 \
		bash

requirements-dev.txt: requirements-dev.in .tool-versions
	uv pip compile --python 3.9 requirements-dev.in -o requirements-dev.txt

.venv/bin/python:
	uv venv

sync: requirements-dev.txt .venv/bin/python
	uv pip sync requirements-dev.txt

testpypi: sync
	.venv/bin/twine upload -r testpypi wheelhouse/*

pypi: sync
	.venv/bin/twine upload wheelhouse/*

pre-commit: sync
	.venv/bin/pre-commit run -a
