#include"parking_lot.hpp"

#define bit(x) (1ul<<x)

ParkingLot::ParkingLot(){
  lcd_fd = open("/dev/lcd", O_RDWR);
  sysState = PL_STATE_WELCOME;
  currentVehicle = std::pair<int, VehicleInfo>(0, VehicleInfo(VEH_STATE_INIT));
}

ParkingLot::ParkingLot(int& fd){
  lcd_fd = fd;
  sysState = PL_STATE_WELCOME;
  currentVehicle = std::pair<int, VehicleInfo>(0, VehicleInfo(VEH_STATE_INIT));
}

int ParkingLot::runSystem(){
  while(1){
    switch(sysState){
      case PL_STATE_WELCOME: sysState = showWelcome(); break;
      case PL_STATE_MENU_NOT_RESERVED: sysState = showMenuNotReserved(); break;
      case PL_STATE_MENU_RESERVED: sysState = showMenuReserved(); break;
      case PL_STATE_MENU_PARKED: sysState = showMenuParked(); break;
      case PL_STATE_SHOW: sysState = showSpace(); break;
      case PL_STATE_CHOOSE: sysState = showChoose(); break;
      case PL_STATE_PAY: sysState = showPay(); break;
      default: perror("Invalid State\n");
    }
  }
}

int ParkingLot::showWelcome(){
  char buf[5];
  // Welcome message on lcd
  segment_display_decimal(0000);
  led_display_binary(0xff);

  printMessage("Input plate num: ");
  // Get Vehicle ID input
  // getMessage()
  getMessage(buf);

  currentVehicle.first = atoi(buf); //assign input data;
  segment_display_decimal(currentVehicle.first);
  // search stored vehicle & get vehicle state
  currentVehicle.second = getVehState(currentVehicle.first);
  vehList[currentVehicle.first] = currentVehicle.second;
  // return new system state
  if(currentVehicle.second.state == VEH_STATE_INIT){
    return PL_STATE_MENU_NOT_RESERVED;
  }else if(currentVehicle.second.state == VEH_STATE_RESERVED){
    return PL_STATE_MENU_RESERVED;
  }else{ // PARKED or PARKED_NOT_RESERVED
    return PL_STATE_MENU_PARKED;
  }
}

int ParkingLot::showMenuNotReserved(){
  char input_buf[5]={0};
  char msg_buf[100];

  // Show menu options on lcd
  sprintf(msg_buf, "%s\n%s\n%s\n%s\n%s\n",
    "You haven't reserved a grid.",
    "1. show",
    "2. reserve",
    "3. check-in",
    "4. exit");
  printMessage(msg_buf);

  //get input
  getMessage(input_buf);

  // std::cout << menu_input << std::endl;
  switch (input_buf[0])
  {
  case '1':
    return PL_STATE_SHOW;
  case '2':
    return PL_STATE_CHOOSE;
  case '3':
    currentVehicle.second.state = VEH_STATE_PARKED_NO_RESERVED;
    this->setVehState(currentVehicle.first, currentVehicle.second);
    return PL_STATE_CHOOSE;
  case '4':
    return PL_STATE_WELCOME;
  default:
    std::cerr << "invalid input\n";
    return PL_STATE_WELCOME;
  }
}

int ParkingLot::showMenuReserved(){
  char input_buf[5]={0};
  char msg_buf[100];

  // Show menu options on lcd
  sprintf(msg_buf, "%s\n%s\n%s\n%s\n%s\n",
    "You have reserved a grid.",
    "1. show",
    "2. cancel",
    "3. check-in",
    "4. exit");
  printMessage(msg_buf);


  //get input
  getMessage(input_buf);
  switch (input_buf[0])
  {
  case '1':
    return PL_STATE_SHOW;
  case '2':
    return PL_STATE_PAY;
  case '3':
    currentVehicle.second.state = VEH_STATE_PARKED;
    this->setVehState(currentVehicle.first, currentVehicle.second);
    return PL_STATE_WELCOME;
  case '4':
    return PL_STATE_WELCOME;
  default:
    std::cerr << "invalid input\n";
    return PL_STATE_WELCOME;
  }
}

int ParkingLot::showMenuParked(){
  char input_buf[5]={0};
  char msg_buf[100];

  // Show menu options on lcd
  sprintf(msg_buf, "%s%d%s%d\n%s\n%s\n",
    "Your car is at lot p", currentVehicle.second.lot, " grid ", currentVehicle.second.grid,
    "1. show",
    "2. pick-up");
  printMessage(msg_buf);

  //get input
  getMessage(input_buf);
  switch (input_buf[0])
  {
  case '1':
    return PL_STATE_SHOW;
  case '2':
    return PL_STATE_PAY;
  default:
    std::cerr << "invalid input\n";
    return PL_STATE_WELCOME;
  }
}

int ParkingLot::showSpace(){
  // Show all car spaces on lcd
  int sum=0;
  char msg_buf[100]={0};
  char* gridState = this->getGridState();
  std::cout << "showing \n";

  for(int i=0; i<3; i++){

    sum=0;
    for(int j=0; j<8; j++){
      sum += (int)((gridState[i] & bit(j)) != 0);
    }
    std::cout << "p" << i << ": ";
    std::cout << 8-sum << std::endl;
    sprintf(msg_buf, "%sp%d: %d\n", msg_buf, i, 8-sum);
  }
  sprintf(msg_buf, "%spress # to return\n", msg_buf);
  printMessage(msg_buf);
  // wait for any key
  getMessage(msg_buf);
  return PL_STATE_WELCOME;
}

int ParkingLot::showChoose(){
  // Show prompt for choose 
  char msg_buf[100]={0};
  char* gridState = getGridState();
  int lot, grid_num;

  // <!-- select grid -->
  printMessage("Select parking lot: ");
  getMessage(msg_buf);
  lot = atoi(msg_buf);
  if(lot>3 || lot<1){
    printMessage("Error!");
    return PL_STATE_WELCOME;
  }else{
    printf("data = %d\n", gridState[lot-1]);
    led_display_binary(gridState[lot-1]);
  }

  // <!-- select grid && show lot state on "LED" -->
  printMessage("\nSelect parking grid: ", false);
  getMessage(msg_buf);
  grid_num = atoi(msg_buf);

  if(lot>3 || lot<1 || grid_num>8 || grid_num<1){
    perror("invalid input");
  }  else if((gridState[lot-1] >> grid_num-1)%2 == 0){
    // gridState[lot] |= (0x01 << grid_num);
    currentVehicle.second.lot = lot;
    currentVehicle.second.grid = (0x01 << grid_num-1);
    if(currentVehicle.second.state == VEH_STATE_INIT){
      currentVehicle.second.state = VEH_STATE_RESERVED;
      this->setVehState(currentVehicle.first, currentVehicle.second);
      sprintf(msg_buf, "Reserved, Have a nice day\n");
    }else if(currentVehicle.second.state == VEH_STATE_PARKED_NO_RESERVED){
      this->setVehState(currentVehicle.first, currentVehicle.second);
      sprintf(msg_buf, "Parked, Have a nice day\n");
    }
    std::cout << "Have a nice day!\n";
    
    sprintf(msg_buf, "%spress # to return\n", msg_buf);
    printMessage(msg_buf);
    // wait for any key
    getMessage(msg_buf);
    return PL_STATE_WELCOME;
  }else{
    printMessage("Error! Selected grid not available\n");
    // std::cerr << "Error! Selected grid not available\n";
    return PL_STATE_WELCOME;
  }

  return PL_STATE_WELCOME;
}

int ParkingLot::showPay(){
  // <!-- if action == cancel -->
  // Reserve fee: $20
  char msg_buf[100]={0};
  if(currentVehicle.second.state == VEH_STATE_PARKED){
    sprintf(msg_buf, "Parking fee: $30\n");
  }else if(currentVehicle.second.state == VEH_STATE_PARKED_NO_RESERVED){
    sprintf(msg_buf, "Parking fee: $40\n");
  }else if(currentVehicle.second.state == VEH_STATE_RESERVED){
    sprintf(msg_buf, "Reserve fee: $20\n");
  }

  currentVehicle.second.state = VEH_STATE_INIT;
  this->setVehState(currentVehicle.first, currentVehicle.second);
  sprintf(msg_buf, "%spress # to return\n", msg_buf);
  printMessage(msg_buf);
  // wait for any key
  getMessage(msg_buf);
  return PL_STATE_WELCOME;
}

VehicleInfo ParkingLot::getVehState(int id){
  std::map<int, VehicleInfo>::iterator veh = vehList.find(id);
  if(veh != vehList.end()){
    return veh->second;  
  }else{
    std::pair<int, VehicleInfo> _veh(id, VehicleInfo(VEH_STATE_INIT));
    vehList.insert(_veh);
    return _veh.second;
  }
}

void ParkingLot::setVehState(int id, VehicleInfo state){
  std::map<int, VehicleInfo>::iterator veh = vehList.find(id);
  if(veh != vehList.end()){
    veh->second = state;  
  }else{
    vehList.insert(std::pair<int, VehicleInfo>(id, state));
  }
}

char* ParkingLot::getGridState(){
  char* gridState = new char[3];
  // std::cout << "getting\n";
  for(std::map<int, VehicleInfo>::iterator it=vehList.begin(); it!=vehList.end(); it++){
    // std::cout << "getting\n";
    printf("%d,%d,%d,%d\n", it->first, it->second.lot, it->second.grid, it->second.state);
    if(it->second.state != VEH_STATE_INIT){
      gridState[it->second.lot-1] |= it->second.grid;
    }
  }

  return gridState;
}

int ParkingLot::printMessage(const char* msg, bool erase){
  int ret = 0;
  if(erase){
    ret = ioctl(this->lcd_fd, LCD_IOCTL_CLEAR, NULL);
  }
  display.Count = sprintf((char*)display.Msg, "%s", msg);
  return ioctl(this->lcd_fd, LCD_IOCTL_WRITE, &display);
}


void ParkingLot::getMessage(char* msg){
  int ret, cnt = 0;
  unsigned short key;
  char c = 0;
  while(1){
    ret = ioctl(lcd_fd, KEY_IOCTL_WAIT_CHAR, &key);
    c = key & 0xff;

    if(c == '#') break;
    else{
      msg[cnt++] = c;
      display.Count = sprintf((char*)display.Msg, "%c", c);
      ret = ioctl(lcd_fd, LCD_IOCTL_WRITE, &display);
    }
  };
  // return msg;

}

int ParkingLot::segment_display_decimal(int data){
  int i;
  _7seg_info_t seg_data;
  unsigned long result_num = 0;
  for(i=1; i<=4096; i*=16){
    // printf("%d^%d, ", data%10, (i));
    result_num += (data%10)*(i);
    data /= 10;
  }
  // printf("%04lx\n", result_num);

  seg_data.Mode = _7SEG_MODE_HEX_VALUE;
  seg_data.Which = _7SEG_ALL;
  seg_data.Value = result_num;
  return ioctl(lcd_fd, _7SEG_IOCTL_SET, &seg_data);
}

int ParkingLot::led_display_binary(int data){
  int i, ret=0;
  printf("displaying:%d \n", data);
  for(i=0; i<8; i++){
    if(data%2) ret |= ioctl(lcd_fd, LED_IOCTL_BIT_CLEAR, &i);
    else ret |= ioctl(lcd_fd, LED_IOCTL_BIT_SET, &i);
    data /= 2;
  }
  return ret;
}
