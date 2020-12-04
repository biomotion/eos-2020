#ifndef PARKINGLOT_H
#define PARKINGLOT_H
#include<iostream>
#include<vector>
#include<map>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#define PL_STATE_WELCOME 0
#define PL_STATE_MENU_PARKED 1
#define PL_STATE_MENU_NOT_RESERVED 2
#define PL_STATE_MENU_RESERVED 3
#define PL_STATE_SHOW 4
#define PL_STATE_CHOOSE 5
#define PL_STATE_PAY 6

#define VEH_STATE_INIT 0
#define VEH_STATE_RESERVED 1
#define VEH_STATE_PARKED 2
#define VEH_STATE_PARKED_NO_RESERVED 3

typedef struct VehicleInfo{
  VehicleInfo():state(VEH_STATE_INIT), lot(-1), grid(0) {};
  VehicleInfo(int _state):state(_state), lot(-1), grid(0) {};
  int state;
  int lot;
  char grid;
} VehicleInfo;


class ParkingLot{
private:
  int lcd_fd;
  int sysState;
  lcd_write_info_t display;
  std::pair<int, VehicleInfo> currentVehicle;
  std::map<int, VehicleInfo> vehList;
  VehicleInfo getVehState(int);
  void setVehState(int, VehicleInfo);
  char* getGridState();
  // For I/O
  int printMessage(const char* msg, bool erase=true);
  void getMessage(char* msg);
  int segment_display_decimal(int data);
  int led_display_binary(int data);
public:
  ParkingLot();
  ParkingLot(int& fd);
  // State Nodes
  int showWelcome();
  int showMenuParked();
  int showMenuReserved();
  int showMenuNotReserved();
  int showSpace();
  int showChoose();
  int showPay();
  // Actions
  int runSystem();
  int getState();
};

#endif
