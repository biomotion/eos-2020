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

#define PL_STATE_WAIT_CONNECTION 0
#define PL_STATE_WELCOME 1
#define PL_STATE_MENU_PARKED 2
#define PL_STATE_MENU_NOT_RESERVED 3
#define PL_STATE_MENU_RESERVED 4
#define PL_STATE_PAY 7

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
  int balance;
  std::pair<int, VehicleInfo> currentVehicle;
  std::vector<int> loggedInVeh;
  std::map<int, VehicleInfo> vehList; // pairs of "ID" and vehicle
  VehicleInfo getVehState(int);
  void setVehState(int, VehicleInfo);
  char *getGridState();
  // For I/O
  int printMessage(int cfd, const char *msg, bool erase = true);
  int getMessage(int cfd, char *msg);

  static void* handleConnection(void* p);
public:
  ParkingLot();
  ParkingLot(int sock_fd);
  // State Nodes
  int showWelcome(int cfd, int* vehId);
  int showMenuParked(int cfd, int* vehId);
  int showMenuReserved(int cfd, int* vehId);
  int showMenuNotReserved(int cfd, int* vehId);
  int showPay(int cfd, int* vehId);
  int selectGrid(int cfd, int lot, int grid);
  void getSpace(char *);
  // Actions
  void runSystem();
  int waitConnection();
  int stopSystem();
  void logStatus(const char *log_file);

};

#endif
