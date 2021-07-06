IMAGE   := deadeye-admin
NS      ?= j3ff
VERSION ?= $(shell git rev-parse --short HEAD)
WHEEL   := deadeye-$(shell poetry version -s)-py3-none-any.whl

.PHONY: build
build: .built

.built: Dockerfile requirements.txt .built_wheel
	docker build --build-arg wheel=$(WHEEL) -t $(NS)/$(IMAGE):latest -t $(NS)/$(IMAGE):$(VERSION) .
	docker inspect -f '{{.Id}}' $(NS)/$(IMAGE):latest > $@

.PHONY: push
push: build
	docker push $(NS)/$(IMAGE):latest
	docker push $(NS)/$(IMAGE):$(VERSION)

.PHONY: run
run:
	# poetry run $(shell poetry env info -p)/bin/deadeye-server 2>&1 | grep -Ev '^(127|\()'
	docker run -it --rm --name $(IMAGE) -p 5000:5000 --env-file .env $(NS)/$(IMAGE)

.PHONY: clean
clean:
	rm -rf dist *.log* .built*

.PHONY: wheel
wheel: .built_wheel

.built_wheel: dist/$(WHEEL)
	poetry version -s > $@

dist/$(WHEEL):
	poetry build --format wheel

requirements.txt: poetry.lock
	poetry export --format requirements.txt --without-hashes --output requirements.txt
