UNAME := $(shell uname)

clean:
	pipenv --rm && rm -rf *.egg-info && rm -rf dist && rm -rf *.log*

requirements:
	pipenv lock -r > requirements.txt

symlinks:
	[[ -L ./deadeye/assets ]] || ln -s ../admin-web/build ./deadeye/assets

install: symlinks
	pipenv run python setup.py sdist

