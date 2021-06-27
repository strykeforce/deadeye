NS ?= j3ff
VERSION ?= $(shell git rev-parse --short HEAD)
IMAGE_NAME := deadeye-admin
WHEEL := deadeye-$(shell poetry version -s)-py3-none-any.whl

.PHONY: build push run clean wheel

build: requirements.txt wheel
	docker build --build-arg wheel=$(WHEEL) -t $(NS)/$(IMAGE_NAME):latest -t $(NS)/$(IMAGE_NAME):$(VERSION) .

push: build
	docker push $(NS)/$(IMAGE_NAME):latest
	docker push $(NS)/$(IMAGE_NAME):$(VERSION)

run:
	# poetry run $(shell poetry env info -p)/bin/deadeye-server 2>&1 | grep -Ev '^(127|\()'
	docker run -it --rm --name $(IMAGE_NAME) -p 5000:5000 --env-file .env $(NS)/$(IMAGE_NAME)

clean:
	rm -rf dist && rm -rf *.log*

wheel:
	poetry build --format wheel

requirements.txt: poetry.lock
	poetry export -f requirements.txt --output requirements.txt


