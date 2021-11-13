.PHONY: javadoc
javadoc:
	cd ../client && \
		./gradlew javadoc

.PHONY: book
book:
	@echo "====> building book"
	mdbook build

.PHONY: deploy
deploy: book
	@echo "====> deploying to github"
	$(eval TMPDIR := $(shell mktemp -d))
	git worktree add $(TMPDIR) gh-pages
	rm -rf $(TMPDIR)/*
	cp -rp book/* $(TMPDIR)
	cd $(TMPDIR) && \
		git add -A && \
		git commit -m "deployed on $(shell date) by ${USER}" && \
		git push origin gh-pages
	rm -rf $(TMPDIR)

# vim: set noexpandtab:
