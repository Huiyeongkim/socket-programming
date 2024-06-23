#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1096

void error_handling(char *message) {
        fputs(message, stderr);
        fputc('\n',stderr);
        exit(1);
}

int main(int argc, char *argv[]) {
        int sock;
        struct sockaddr_in serv_addr;
        char message[BUF_SIZE];
        int str_len, recv_len, recv_cnt;

        if (argc!=4) {
		printf("Usage:%s <IP> <port> <filename>\n",argv[0]);
		exit(1);
        }
       
        sock=socket(PF_INET, SOCK_STREAM, 0);
        if (sock==-1)
                error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family=AF_INET;
        serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
        serv_addr.sin_port=htons(atoi(argv[2]));

        if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
                error_handling("connect() error");
        else
                puts("Connected ...........");


	char filename[BUF_SIZE];
        strcpy(filename, argv[3]);
        
	FILE *fp=fopen(filename, "rb");
        if (fp==NULL) {
                printf("cannot open file\n");
                exit(1);
        }
	
	if (write(sock, filename,sizeof(filename))==-1) {
		printf("can not send filename\n");
		exit(1);
	}
	
	recv_len = 0;
        fseek(fp, 0, SEEK_END);
        str_len = ftell(fp);
        rewind(fp);

	while (recv_len<str_len) {
		recv_cnt=fread(&message[recv_len], sizeof (char), BUF_SIZE, fp);
		recv_len+=recv_cnt;
		write(sock, message, str_len); 
	}
	fclose(fp);
	close(sock); 
	return 0;
}
