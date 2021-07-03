
.PHONY: provision deploy

provision:
	poetry run ansible-playbook -i inventory.yaml --vault-password-file=.vaultpw provision.yaml

deploy:
	poetry run ansible-playbook -i inventory.yaml --vault-password-file=.vaultpw deploy.yaml
