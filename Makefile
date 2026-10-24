manylinux2014:
	docker run \
		--rm \
		-v `pwd`:/src \
		--workdir /src \
		quay.io/pypa/manylinux_2_34_x86_64 \
		./build-manylinux.sh

manylinux2014-each:
	docker run \
		--rm \
		-v `pwd`:/src \
		--workdir /src \
		quay.io/pypa/manylinux_2_34_x86_64 \
		./build-manylinux-each.sh $(VER)

manylinux2014-sh:
	docker run \
		--rm \
		-it \
		-v `pwd`:/src \
		--workdir /src \
		quay.io/pypa/manylinux_2_34_x86_64 \
		bash

.venv/bin/python:
	uv venv

sync: pyproject.toml .venv/bin/python
	uv sync

testpypi: sync
	.venv/bin/twine upload -r testpypi wheelhouse/*

pypi: sync
	.venv/bin/twine upload wheelhouse/*

pre-commit: sync
	.venv/bin/pre-commit run -a
