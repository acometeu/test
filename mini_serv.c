#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>


//forbidden
#include <stdio.h>

typedef struct s_clients{
	int	id;
	char msg[100000];
} t_clients;


void	ft_printf(char *str, int fd){
	if (!str)
		return;

	int i = 0;
	while (str[i]){
		write(fd, &str[i], 1);
		i++;
	}
}

void	send_message(char *buffer,  fd_set fdset_write, int exception, int fd_max){
	int i = 0;

	while(i < fd_max + 1){
		if (FD_ISSET(i, &fdset_write) && i != exception){
			ft_printf(buffer, i);
		}
		i++;
	}
}



int main(int argc, char ** argv) {
	int sockfd, connfd, client_limit;
	struct sockaddr_in servaddr, cli; 


	if (argc < 2){
		ft_printf("Wrong number of arguments\n", 2);
		exit(1);
	}
	client_limit = 10;


	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		ft_printf("Fatal error\n", 2);
		exit(1); 
	} 
printf("sockfd = %d\n", sockfd);
	bzero(&servaddr, sizeof(servaddr)); 


	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		ft_printf("Fatal error\n", 2);
		exit(1); 
	} 
	if (listen(sockfd, client_limit) != 0) {
		ft_printf("Fatal error\n", 2);
		exit(1); 
	}

	
	//loop
	socklen_t len = sizeof(cli);
	fd_set fdset, fdset_read, fdset_write;
	static t_clients clients[1024];
	int	fd_max = sockfd;
	int id_max = -1;
	char	buffer[100050], buffer_rcv[100000];

//	bzero(clients, sizeof(clients));
	bzero(buffer, sizeof(buffer));
	bzero(buffer_rcv, sizeof(buffer_rcv));

	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);

	while (1){
		fdset_read = fdset;
		fdset_write = fdset;

		if (select(fd_max + 1, &fdset_read, &fdset_write, NULL, NULL) == -1)
			continue;

		if (FD_ISSET(sockfd, &fdset_read)){
			int newclient = accept(sockfd, (struct sockaddr *)&cli, &len);

			if (newclient != -1){
				if (newclient > fd_max)
					fd_max = newclient;
				clients[newclient].id = id_max + 1;
				id_max++;
				FD_SET(newclient, &fdset);
				sprintf(buffer, "server: client %d just arrived\n", clients[newclient].id);
				send_message(buffer, fdset_write, newclient, fd_max);
				bzero(buffer, sizeof(buffer));
			}
		}
		
		int i = sockfd + 1;
		while (i < fd_max + 1){
			if (FD_ISSET(i, &fdset_read)){
				int size = read(i, buffer_rcv, sizeof(buffer_rcv));
				if (size <= 0){
					FD_CLR(i, &fdset);
					sprintf(buffer, "server: client %d just left\n", clients[i].id);
					send_message(buffer, fdset_write, i, fd_max);
					bzero(buffer, sizeof(buffer));
					bzero(clients[i].msg, sizeof(clients[i].msg));
					close(i);
				}
				else{
					int j = 0;
					int k = strlen(clients[i].msg);
					while(j < size){
						clients[i].msg[k] = buffer_rcv[j];
						if (clients[i].msg[k] == '\n'){
							clients[i].msg[k] = '\0';
							sprintf(buffer, "client %d: %s\n", clients[i].id, clients[i].msg);
							send_message(buffer, fdset_write, i, fd_max);
							bzero(clients[i].msg, sizeof(clients[i].msg));
							bzero(buffer, sizeof(buffer));
							k = -1;
						}
						j++;
						k++;
					}
				}
			}
			i++;
		}
		
	}


	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n");
}














