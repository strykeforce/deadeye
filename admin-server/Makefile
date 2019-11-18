PWD := $(shell pwd)

clean:
	pipenv --rm && rm -rf *.egg-info && rm -rf dist && rm -rf *.log*

symlinks:
	ln -sf $(PWD)/../admin-web/build $(PWD)/deadeye/assets

requirements:
	pipenv lock -r > requirements.txt

install: symlinks
	python3 setup.py sdist

