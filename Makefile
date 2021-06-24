export NS := j3ff
export VERSION := $(shell git rev-parse --short HEAD) 

.PHONY: build push

build:
	$(MAKE) -C admin build
	$(MAKE) -C docker build
	$(MAKE) -C web build

push:
	$(MAKE) -C admin push
	$(MAKE) -C docker push
	$(MAKE) -C web push
