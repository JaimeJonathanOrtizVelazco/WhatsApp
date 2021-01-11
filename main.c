#include <pthread.h>
#include <curses.h>
#include <termios.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
/*curses lib sudo apt-get install libncurses5-dev
 * -lcurses*/
#define PUERTO 5000
#define TAM_BUFFER 80
#define DIR_IP "127.0.0.1"
int sockfd;
int done = 0;
int line = 1;
int input = 1;
int maxX, maxY;
WINDOW *top;
WINDOW *bottom;
sem_t mutex;

void *sendMessage();

void *listener();

int main() {
    sem_init(&mutex, 0, 1);
    struct sockaddr_in direccion_servidor;
    memset(&direccion_servidor, 0, sizeof(direccion_servidor));
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port = htons(PUERTO);
    if (inet_pton(AF_INET, DIR_IP, &direccion_servidor.sin_addr) <= 0) {
        exit(-1);
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        exit(-1);
    }
    if (connect(sockfd, (struct sockaddr *) &direccion_servidor, sizeof(direccion_servidor)) < 0) {
        exit(-1);
    }
    //Init Window
    initscr();
    getmaxyx(stdscr, maxY, maxX);
    top = newwin(maxY / 2, maxX, 0, 0);
    bottom = newwin(maxY / 2, maxX, maxY / 2, 0);
    scrollok(top, TRUE);
    scrollok(bottom, TRUE);
    wsetscrreg(top, 1, maxY / 2 - 2);
    wsetscrreg(bottom, 1, maxY / 2 - 2);
    //End init Window
    pthread_t threads[2];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, sendMessage, NULL);
    pthread_create(&threads[1], &attr, listener, NULL);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    endwin();
    sem_destroy(&mutex);
    return EXIT_SUCCESS;
}

void *sendMessage() {
    char buffer[TAM_BUFFER];
    while (1) {
        bzero(buffer, TAM_BUFFER);
        wrefresh(top);
        wrefresh(bottom);
        mvwgetstr(bottom, input, 2, buffer);
        if (send(sockfd, buffer, strlen(buffer), 0) < 0)
            break;
        sem_wait(&mutex);
        mvwprintw(top, line, 2, buffer);
        if (line != maxY / 2 - 2) {
            line++;
        } else {
            scroll(top);
        }
        if (input != maxY / 2 - 2) {
            input++;
        } else {
            scroll(bottom);
        }
        sem_post(&mutex);
    }
    close(sockfd);
    pthread_exit(NULL);
}

void *listener() {
    char buffer[TAM_BUFFER];
    while (1) {
        bzero(buffer, TAM_BUFFER);
        wrefresh(top);
        wrefresh(bottom);
        if (recv(sockfd, buffer, TAM_BUFFER, 0) < 0)
            break;
        if (strcmp(buffer, "quit()") == 0)
            break;
        sem_wait(&mutex);
        mvwprintw(top, line, 2, buffer);
        if (line != maxY / 2 - 2) {
            line++;
        } else {
            scroll(top);
        }
        sem_post(&mutex);
    }
    close(sockfd);
    pthread_exit(NULL);
}

