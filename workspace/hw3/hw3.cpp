#include <stdio.h>
#include "parking_lot.hpp"
#include "socket_utils.h"

#include <pthread.h> // pthread_create, pthread_join, etc.

#include <cstring>
#include <signal.h>

ParkingLot* lotPtr;

void *th(void *arg);

int stop = 0;
void intHandler(int);

int main(int argc, char **argv)
{
  int sock_fd;
  struct sigaction sa;

  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    exit(-1);
  }
  sock_fd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);

  ParkingLot pl(sock_fd);
  lotPtr = &pl;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = intHandler;
  sigaction(SIGINT, &sa, NULL);

  pl.runSystem();

  printf("Exit\n");
}


void intHandler(int signum){
  printf("called\n");
  lotPtr->logStatus("result.txt");
  lotPtr->stopSystem();
  stop = 1;
}
