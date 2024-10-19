#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <openssl/pem.h>
#include <signal.h>
struct sockaddr_in myip;
char *base64decode (const void *b64, int b64lon){
 BIO *b64_bio, *mem_bio;int index = 0;
 char *clean = calloc(b64lon,sizeof(char));
 b64_bio = BIO_new(BIO_f_base64());
 mem_bio = BIO_new(BIO_s_mem());BIO_write(mem_bio, b64, b64lon);
 BIO_push(b64_bio, mem_bio);
 BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);
 while ( 0 < BIO_read(b64_bio, clean+index, 1) ){index=index+1;}
 BIO_free_all(b64_bio); return clean;}
static int auth_password(const char *user, const char *password){
 return 0;}
char *getClientIp(ssh_session session) {
 struct sockaddr_storage tmp; struct sockaddr_in *sock;
 unsigned int len = 100;
 char *ip = (char *)malloc(100*sizeof(char));ip[0]='\0';
 getpeername(ssh_get_fd(session), (struct sockaddr*)&tmp, &len);
 sock = (struct sockaddr_in *)&tmp;
 inet_ntop(AF_INET, &sock->sin_addr, ip, len);return ip;}
int sshHonney(){
 ssh_session session;ssh_bind sshbind;ssh_message message;
 ssh_channel chan=0; char buf[2048]; int auth=0, sftp=0, i,r;
 struct timeval mytime;
 while(1==1){ sshbind=ssh_bind_new(); session=ssh_new();
  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT_STR,
   "2200");
  ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY,
   "./ssh_host_rsa_key");
  gettimeofday(&mytime, NULL);
  if(ssh_bind_listen(sshbind)<0){return -1;
  }else{r=ssh_bind_accept(sshbind,session);
   if(r!=SSH_ERROR){if(!ssh_handle_key_exchange(session)) {
    auth=0;
    while(!auth){message=ssh_message_get(session);
    if(!message)break;
    if(ssh_message_type(message)==SSH_REQUEST_AUTH){
     if(ssh_message_subtype(message)==SSH_AUTH_METHOD_PASSWORD){
      printf("%d:%s:SSH:%s:%s\n",mytime.tv_sec,
       getClientIp(session),ssh_message_auth_user(message),
       ssh_message_auth_password(message));fflush(stdout);
      ssh_message_auth_set_methods(message,
       SSH_AUTH_METHOD_PASSWORD);}}
    ssh_message_reply_default(message);ssh_message_free(message);
    }}}}ssh_disconnect(session);ssh_bind_free(sshbind);
  ssh_finalize();}return 0;}
int telnetHonney(){
 int telnetSocket,clientLen=0, clientSocket;
 struct sockaddr_in ip;struct sockaddr_in ipclient;
 char data[100];memset(data,0,100*sizeof(char));char user[100];
 memset(user,0,100*sizeof(char));char pass[100];
 memset(pass,0,100*sizeof(char));struct timeval mytime;
 bzero((char *) &ip, sizeof(ip));
 ip.sin_family = AF_INET;ip.sin_addr.s_addr = htonl(INADDR_ANY);
 ip.sin_port = htons(2300);clientLen=sizeof(ipclient);
 telnetSocket = socket(AF_INET,SOCK_STREAM,0);
 if(bind(telnetSocket, &ip , sizeof(ip))<0){return -1;}
 listen(telnetSocket , 20);clientSocket=0;
 while(1 == 1){if(clientSocket != 0){close(clientSocket);}
  clientSocket = accept(telnetSocket,&ipclient,&clientLen);
  gettimeofday(&mytime, NULL);write(clientSocket,"user: ",6);
  memset(data,0,100*sizeof(char));
  if(read(clientSocket,&data,99) < 1){continue;};
  data[99]='\0';sscanf(data,"%s",user);
  write(clientSocket,"password: ",10);
  memset(data,0,100*sizeof(char));
  if(read(clientSocket,&data,99) < 1){continue;};data[99]='\0';
  sscanf(data,"%s",pass);close(clientSocket);
  printf("%d:%s:TELNET:%s:%s\n",mytime.tv_sec,
   inet_ntoa(ipclient.sin_addr),user,pass);
  fflush(stdout);clientSocket=0;}}
int smtpHonney(){
 int smtpSocket, clientLen=0, clientSocket;struct sockaddr_in ip;
 struct sockaddr_in ipclient;char data[100];
 memset(data,0,100*sizeof(char));char user[100];
 memset(user,0,100*sizeof(char));char pass[100];
 memset(pass,0,100*sizeof(char));char *b64user,*b64pass;
 struct timeval mytime;bzero((char *) &ip, sizeof(ip));
 ip.sin_family = AF_INET;ip.sin_addr.s_addr = htonl(INADDR_ANY);
 ip.sin_port = htons(2500);clientLen=sizeof(ipclient);
 smtpSocket = socket(AF_INET,SOCK_STREAM,0);
 if(bind(smtpSocket, &ip , sizeof(ip))<0){return -1;}
 listen(smtpSocket , 20);clientSocket=0;
 while(1 == 1){if(clientSocket != 0){close(clientSocket);}
  clientSocket = accept(smtpSocket,&ipclient,&clientLen);
  gettimeofday(&mytime, NULL);write(clientSocket,
   "220 smtp.ezequiel.ca ESMTP server\r\n",35);
  memset(data,0,100*sizeof(char));
  if(read(clientSocket,&data,99)<1){continue;}; data[99]='\0';
  if(strlen(data)>7){sscanf(&data[5],"%s",user);}else{
   continue;};write(clientSocket,"250-smtp.ezequiel.ca Hello ",
     27); write(clientSocket,user,strlen(user));
   write(clientSocket,"\r\n",2);write(clientSocket,
    "250 AUTH LOGIN\r\n",16); memset(data,0,100*sizeof(char));
   if(read(clientSocket,&data,99)<1){continue;};
   sprintf(user,"AUTH"); while(strcmp(user,"AUTH")==0){
    write(clientSocket,"334 VXNlcm5hbWU6\r\n",18);
    memset(data,0,100*sizeof(char));
    if(read(clientSocket,&data,99)<1){data[0]='\0';break;};
    data[99]='\0';sscanf(data,"%s",user);}
   if(strlen(data)<1){continue;};
   data[99]='\0';sscanf(data,"%s",user);
   write(clientSocket,"334 UGFzc3dvcmQ6\r\n",18);
   memset(data,0,100*sizeof(char));
   if(read(clientSocket,&data,99)<1){continue;};
   data[99]='\0';sscanf(data,"%s",pass);
   write(clientSocket,"535 Bad password.\r\n",19);
   close(clientSocket);
   b64user=base64decode(user,strlen(user));
   b64pass=base64decode(pass,strlen(pass));
   strtok(b64user,"@ezequiel.ca");
   printf("%d:%s:SMTP:%s:%s\n",mytime.tv_sec,
    inet_ntoa(ipclient.sin_addr),b64user,b64pass);
   free(b64user);free(b64pass);fflush(stdout);clientSocket=0;}}
int ftpHonney(){
 int ftpSocket, clientLen=0, clientSocket; struct sockaddr_in ip;
 struct sockaddr_in ipclient; char data[100]; char user[100];
 memset(data,0,100*sizeof(char));char pass[100];
 memset(user,0,100*sizeof(char));struct timeval mytime;
 memset(pass,0,100*sizeof(char));bzero((char *) &ip, sizeof(ip));
 ip.sin_family = AF_INET; ip.sin_addr.s_addr = htonl(INADDR_ANY);
 ip.sin_port = htons(2100); clientLen=sizeof(ipclient);
 ftpSocket = socket(AF_INET,SOCK_STREAM,0);
 if(bind(ftpSocket, &ip , sizeof(ip))<0){return -1;}
 listen(ftpSocket , 20);
 while(1 == 1){
  clientSocket = accept(ftpSocket,&ipclient,&clientLen);
  gettimeofday(&mytime, NULL);write(clientSocket,"220 \r\n",6);
  memset(data,0,100*sizeof(char));
  if(read(clientSocket,&data,99) < 6){continue;};
  data[99]='\0';user[0]='\0';sscanf(data,"USER %s",user);
  write(clientSocket,"331 \r\n",6);
  memset(data,0,100*sizeof(char));
  if(read(clientSocket,&data,99) < 6){continue;};
  data[99]='\0';pass[0]='\0';sscanf(data,"PASS %s",pass);
  write(clientSocket,"530 User cannot log in.\r\n",25);
  close(clientSocket); printf("%d:%s:FTP:%s:%s\n",
  mytime.tv_sec,inet_ntoa(ipclient.sin_addr),user,pass);
  fflush(stdout);clientSocket=0;}}
int main(int argc, char **argv){
 signal(SIGPIPE,SIG_IGN);
 pthread_t sshThread,ftpThread,telnetThread,smtpThread;
 pthread_create(&sshThread, NULL,&sshHonney, NULL);
 pthread_create(&ftpThread, NULL,&ftpHonney, NULL);
 pthread_create(&telnetThread, NULL,&telnetHonney, NULL);
 pthread_create(&smtpThread, NULL,&smtpHonney, NULL);
 while(1==1){sleep(60);}}
