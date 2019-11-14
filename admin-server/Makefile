UNAME := $(shell uname)

clean:
	pipenv --rm && rm -rf *.egg-info && rm -rf dist && rm -rf *.log*

requirements:
	if [[ $(UNAME) == "Linux" ]]; then \
		pipenv lock -r > /var/opt/deadeye/requirements.txt; \
	fi

symlinks:
	[[ -L ./deadeye/assets ]] || ln -s ../admin-web/build ./deadeye/assets

install: requirements symlinks
	pipenv run python setup.py sdist

