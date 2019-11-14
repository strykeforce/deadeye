UNAME := $(shell uname)

clean:
	pipenv --rm && rm -rf *.egg-info && rm -rf dist && rm -rf *.log*

symlinks:
	ln -sf ../admin-web/build ./deadeye/assets

install: symlinks
	python3 setup.py sdist

