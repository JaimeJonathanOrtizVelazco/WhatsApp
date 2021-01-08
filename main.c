#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 5000
#define TAM_BUFFER 200
#define DIR_IP "127.0.0.1"

void main() {
    int sockfd, id_cliente;
    struct sockaddr_in direccion_servidor;
    char buffermessage[TAM_BUFFER];
    memset(&direccion_servidor, 0, sizeof(direccion_servidor));
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port
            = htons(PUERTO);
    if (inet_pton(AF_INET, DIR_IP, &direccion_servidor.sin_addr) <= 0) {
        perror("Ocurrio un error al momento de asignar la direccion IP");
        exit(1);
    }
    printf("Creando Socket ....\n");
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Ocurrio un problema en la creacion del socket");
        exit(1);
    }
    printf("Estableciendo conexion ...\n");
    if (connect(sockfd, (struct sockaddr *) &direccion_servidor, sizeof(direccion_servidor)) < 0) {
        perror("Ocurrio un problema al establecer la conexion");
        exit(1);
    }
    while(1) {
        memset(buffermessage, 0, sizeof(buffermessage));
        if (read(sockfd, &buffermessage, TAM_BUFFER) < 0) {
            perror("Ocurrio algun problema al recibir datos del cliente");
            exit(1);
        }
        if(strcmp(buffermessage,"quit()")==0){
            close(sockfd);
            system("clear");
            printf("Gracias por usar nuestro sistema\n");
            exit(1);
        }
        printf("%s", buffermessage);
        fgets(buffermessage, 200, stdin);
        if (write(sockfd, buffermessage, strlen(buffermessage)) < 0) {
            perror("Ocurrio un problema en el envio de un mensaje al cliente");
            exit(1);
        }
    }
}