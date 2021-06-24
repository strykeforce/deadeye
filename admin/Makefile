IMAGE_NAME := deadeye-admin

.PHONY: build push clean symlinks requirements install

build:
	docker build $(EXTRA_BUILD_ARGS) -t $(NS)/$(IMAGE_NAME):latest -t $(NS)/$(IMAGE_NAME):$(VERSION) -f Dockerfile .

push: build
	docker push $(NS)/$(IMAGE_NAME):latest
	docker push $(NS)/$(IMAGE_NAME):$(VERSION)

clean:
	pipenv --rm && rm -rf *.egg-info && rm -rf dist && rm -rf *.log*

symlinks:
	ln -sf $(CURDIR)/../admin-web/build $(CURDIR)/deadeye/assets

requirements:
	pipenv lock -r > requirements.txt

install: symlinks
	python3 setup.py sdist

