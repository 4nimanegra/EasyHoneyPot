EasyHoneypot: EasyHoneypot.c ssh_host_rsa_key
	gcc -o EasyHoneypot EasyHoneypot.c -lcrypto -lssh -pthread
ssh_host_rsa_key:
	ssh-keygen -t rsa -b 4096 -f ssh_host_rsa_key -N "" -q
clean:
	rm EasyHoneypot ssh_host_rsa_key ssh_host_rsa_key.pub
