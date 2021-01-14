/*
 * Class:  ParkingLot 
 * --------------------
 * Properties: 
 *   - TODO
 */

#ifndef PARKINGLOT_H
#define PARKINGLOT_H
#include <iostream>
#include <vector>
#include <map>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>

#include "socket_utils.h"

#define CLIENT_STATE_WELCOME 1
#define CLIENT_STATE_MENU_PARKED 2
#define CLIENT_STATE_MENU_NOT_RESERVED 3
#define CLIENT_STATE_MENU_RESERVED 4
#define CLIENT_STATE_PAY 7
#define CLIENT_STATE_EXIT 8

#define VEH_STATE_INIT 0
#define VEH_STATE_RESERVED 1
#define VEH_STATE_PARKED 2
#define VEH_STATE_PARKED_NO_RESERVED 3

#define MSG_BUFSIZE 128

/*
 * Structure:  VehicleInfo 
 * --------------------
 * Properties: 
 *   - state: 
 *   - lot: range 1 ~ 3 integer related to lot[0] ~ lot[2] respectively
 *   - grid: bitmask for occupied grids(87654321). where 1 means occupied, 0 means available. 
 */

typedef struct VehicleInfo
{
  VehicleInfo() : state(VEH_STATE_INIT), lot(-1), grid(-1){};
  VehicleInfo(int _state) : state(_state), lot(-1), grid(-1){};
  int state;
  int lot;
  int grid;
} VehicleInfo;

class ParkingLot
{
private:
  int stop;
  int conn_fd, sock_fd;
  // int sysState;
  std::pair<int, VehicleInfo> currentVehicle;
  std::vector<int> loggedInVeh;
  std::vector<pthread_t*> clientThreads;
  std::map<int, VehicleInfo> vehList; // pairs of "ID" and vehicle

  // For I/O
  static void* handleConnection(void* p);
public:
  ParkingLot();
  ParkingLot(int sock_fd);
  // ~ParkingLot();
  // State Nodes
  int selectGrid(int lot, int grid);
  VehicleInfo getVehState(int);
  void setVehState(int, VehicleInfo);
  // void parkVehicle(int, int, int);
  // void reserveVehicle(int, int, int);
  void getGridState(char *);
  void getSpace(char *);
  // Actions
  void runSystem();
  int waitConnection();
  int stopSystem();
  void logStatus(const char *log_file);
  int balance;
};

class ClientSession{
private:
  std::pair<int, VehicleInfo> veh;
  ParkingLot* lot;
  int clientState;
  int connfd;
  int showWelcome();
  int showMenuParked();
  int showMenuReserved();
  int showMenuNotReserved();
  int showPay();

  int printMessage(const char *msg);
  int getMessage(char *msg);
public:
  // ClientSession();
  ClientSession(int connfd_, ParkingLot& lot_);
  ClientSession(const ClientSession& c);
  void run();
  ParkingLot* getLot() {return lot;}
};

#endif
