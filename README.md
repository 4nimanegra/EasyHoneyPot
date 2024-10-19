# EasyHoneyPot: The simple HoneyPot

An simple honeypot implemented in c. It is more a canary than a honeypot, and has been implemented only as a blue team tool.

It implements FTP, SSH, TELNET and SMTP services.

Can be compiled with:

```bash
gcc -o EasyHonneypot EasyHonneypot.c -lcrypto -lssh -pthread
```

or simply by using the Makefile:

```bash
make easyhoneypot
```

The makefile creates also an ssh\_host\_rsa\_key needed by the ssh server.

In order to be used with standar ports, a port redirection should be made in ports 21, 22, 23 and 25 to the honeypot ports 2100, 2200, 2300 and 2500. Can be addressed by using iptables:

```
sudo iptables -t nat -A PREROUTING -p tcp --destination-port 21 -j REDIRECT --to-port 2100
sudo iptables -t nat -A PREROUTING -p tcp --destination-port 22 -j REDIRECT --to-port 2200
sudo iptables -t nat -A PREROUTING -p tcp --destination-port 23 -j REDIRECT --to-port 2300
sudo iptables -t nat -A PREROUTING -p tcp --destination-port 25 -j REDIRECT --to-port 250
```
