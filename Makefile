export NS        ?= j3ff
export VERSION   ?= $(shell git rev-parse --short HEAD) 


.PHONY: build
build:
	$(MAKE) -C admin build
	$(MAKE) -C cpp-base build
	$(MAKE) -C web build

.PHONY: push
push:
	$(MAKE) -C admin push
	$(MAKE) -C cpp-base push
	$(MAKE) -C web push

.PHONY: clean
clean:
	$(MAKE) -C admin clean
	$(MAKE) -C cpp-base clean
	$(MAKE) -C web clean
