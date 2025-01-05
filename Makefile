manylinux2014:
	docker run \
		--rm \
		-v `pwd`:/src \
		--workdir /src \
		registry.gitlab.com/bjmuld/manylinux-boost/manylinux2014_x86_64 \
		./build-manylinux.sh

manylinux2014-sh:
	docker run \
		--rm \
		-it \
		-v `pwd`:/src \
		--workdir /src \
		registry.gitlab.com/bjmuld/manylinux-boost/manylinux2014_x86_64 \
		bash

requirements-dev.txt: requirements-dev.in .tool-versions
	uv pip compile requirements-dev.in -o requirements-dev.txt

.venv/bin/python:
	uv venv

sync: requirements-dev.txt .venv/bin/python
	uv pip sync requirements-dev.txt
