#include "parking_lot.hpp"

#define bit(x) (1ul << (x))

ParkingLot::ParkingLot()
{
  stop = 0;
  balance = 0;
  currentVehicle = std::pair<int, VehicleInfo>(0, VehicleInfo(VEH_STATE_INIT));
}

ParkingLot::ParkingLot(int sock_fd)
{
  stop = 0;
  balance = 0;
  this->sock_fd = sock_fd;
  currentVehicle = std::pair<int, VehicleInfo>(0, VehicleInfo(VEH_STATE_INIT));
}

void ParkingLot::runSystem()
{
  int ret;
  pthread_t thread;
  while (!stop)
  {
    if ((ret = waitConnection()) < 0)
    {
      perror("Error: wait_Connection\n");
    }
    if (pthread_create(&thread, NULL, (void *(*)(void *)) & (this->handleConnection), (void *)this))
    {
      perror("Error: pthread_create()\n");
    }
  }
}

int ParkingLot::waitConnection()
{
  struct sockaddr_in cln_addr;
  socklen_t sLen = sizeof(cln_addr);
  conn_fd = accept(sock_fd, (struct sockaddr *)&cln_addr, &sLen);
  printf("accept: %d\n", conn_fd);
  if (conn_fd == -1)
  {
    perror("Error: accept()");
    return -1;
  }
  return conn_fd;
}

void *ParkingLot::handleConnection(void *p)
{
  ParkingLot *t = (ParkingLot *)p;
  int sysState = PL_STATE_WELCOME;
  int conn = t->conn_fd;
  int finish = 0;
  int vehId = -1;
  while (!t->stop && !finish)
  {
    // printf("state: %d\n", sysState);
    switch (sysState)
    {
    case PL_STATE_WELCOME:
      sysState = t->showWelcome(conn, &vehId);
      break;
    case PL_STATE_MENU_NOT_RESERVED:
      sysState = t->showMenuNotReserved(conn, &vehId);
      break;
    case PL_STATE_MENU_RESERVED:
      sysState = t->showMenuReserved(conn, &vehId);
      break;
    case PL_STATE_MENU_PARKED:
      sysState = t->showMenuParked(conn, &vehId);
      break;
    case PL_STATE_PAY:
      sysState = t->showPay(conn, &vehId);
      break;
    case PL_STATE_WAIT_CONNECTION:
      finish = 1;
      break;
    default:
      perror("Invalid State\n");
      break;
    }
  }
  pthread_exit(NULL);
}

int ParkingLot::showWelcome(int cfd, int* vehId)
{
  char buf[MSG_BUFSIZE];
  // Welcome message on lcd
  // printMessage("Input plate num: ");
  // Get Vehicle ID input
  // getMessage()
  if (getMessage(cfd, buf) == -1)
  {
    return PL_STATE_WAIT_CONNECTION;
  }

  currentVehicle.first = atoi(buf); //assign input data;
  // search stored vehicle & get vehicle state
  currentVehicle.second = getVehState(currentVehicle.first);
  // vehList[currentVehicle.first] = currentVehicle.second;
  // return new system state
  loggedInVeh.push_back(currentVehicle.first);
  if (currentVehicle.second.state == VEH_STATE_INIT)
  {
    return PL_STATE_MENU_NOT_RESERVED;
  }
  else if (currentVehicle.second.state == VEH_STATE_RESERVED)
  {
    return PL_STATE_MENU_RESERVED;
  }
  else
  { // PARKED or PARKED_NOT_RESERVED
    return PL_STATE_MENU_PARKED;
  }
}

int ParkingLot::showMenuNotReserved(int cfd, int* vehId)
{
  char msg_buf[MSG_BUFSIZE];
  char *tok;
  int lot, grid;

  sprintf(msg_buf, "%s", "You haven't reserved grid.");
  printMessage(cfd, msg_buf);
  while (1)
  {
    //get input
    if (getMessage(cfd, msg_buf) == -1)
    {
      return PL_STATE_WAIT_CONNECTION;
    }

    // printf("%s", msg_buf);
    tok = strtok(msg_buf, " ");
    // printf("tok = %s\n", tok);
    // printf("msg = %s\n", msg_buf);
    if (strcmp(tok, "show") == 0)
    {
      getSpace(msg_buf);
      printMessage(cfd, msg_buf);
      continue;
    }
    else if (strcmp(tok, "reserve") == 0)
    {
      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage(cfd, "Invaild command.");
        continue;
      }
      lot = atoi(tok);

      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage(cfd, "Invaild command.");
        continue;
      }
      grid = atoi(tok);
      if (selectGrid(cfd, lot, grid) == 0)
      {
        currentVehicle.second.state = VEH_STATE_RESERVED;
        this->setVehState(currentVehicle.first, currentVehicle.second);
        printMessage(cfd, "Reserve successful.");
        return PL_STATE_WELCOME;
      }
      // return PL_STATE_CHOOSE;
    }
    else if (strcmp(tok, "check-in") == 0)
    {
      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage(cfd, "Invaild command.");
        continue;
      }
      lot = atoi(tok);

      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage(cfd, "Invaild command.");
        continue;
      }
      grid = atoi(tok);
      if (selectGrid(cfd, lot, grid) == 0)
      {
        currentVehicle.second.state = VEH_STATE_PARKED;
        this->setVehState(currentVehicle.first, currentVehicle.second);
        printMessage(cfd, "Check-in successful.");
        return PL_STATE_WELCOME;
      }
    }
    else if (strcmp(tok, "exit") == 0)
    {
      printMessage(cfd, "Logout.");
      return PL_STATE_WELCOME;
    }
    else
    {
      // std::cerr << "invalid input" << sysState << std::endl;
      printMessage(cfd, "Invaild command.");
      continue;
    }
  }
}

int ParkingLot::showMenuReserved(int cfd, int* vehId)
{
  char msg_buf[MSG_BUFSIZE];

  // Show menu options on lcd

  sprintf(msg_buf, "%s", "You have reserved grid.");
  printMessage(cfd, msg_buf);
  while (1)
  {
    //get input
    if (getMessage(cfd, msg_buf) == -1)
    {
      return PL_STATE_WAIT_CONNECTION;
    }

    // printf("%s", msg_buf);

    if (strcmp(msg_buf, "show") == 0)
    {
      getSpace(msg_buf);
      printMessage(cfd, msg_buf);
      continue;
    }
    else if (strcmp(msg_buf, "cancel") == 0)
      return PL_STATE_PAY;
    else if (strcmp(msg_buf, "check-in") == 0)
    {
      currentVehicle.second.state = VEH_STATE_PARKED;
      this->setVehState(currentVehicle.first, currentVehicle.second);
      printMessage(cfd, "Check-in successful.");
      return PL_STATE_WELCOME;
    }
    else if (strcmp(msg_buf, "exit") == 0)
    {
      return PL_STATE_WELCOME;
      printMessage(cfd, "Logout.");
    }
    else
    {
      // std::cerr << "invalid input" << sysState << std::endl;
      printMessage(cfd, "Invaild command.");
      continue;
    }
  }
}

int ParkingLot::showMenuParked(int cfd, int* vehId)
{
  char msg_buf[MSG_BUFSIZE];

  sprintf(msg_buf, "Your grid is at lot P%d grid %d.", currentVehicle.second.lot, currentVehicle.second.grid);
  printMessage(cfd, msg_buf);
  while (1)
  {

    //get input
    if (getMessage(cfd, msg_buf) == -1)
    {
      return PL_STATE_WAIT_CONNECTION;
    }

    // printf("%s", msg_buf);
    if (strcmp(msg_buf, "show") == 0)
    {
      getSpace(msg_buf);
      printMessage(cfd, msg_buf);
      return PL_STATE_MENU_NOT_RESERVED;
    }
    else if (strcmp(msg_buf, "pick-up") == 0)
      return PL_STATE_PAY;
    else if (strcmp(msg_buf, "exit") == 0)
    {
      printMessage(cfd, "Logout.");
      return PL_STATE_WELCOME;
    }
    else
    {
      // std::cerr << "invalid input" << sysState << std::endl;
      printMessage(cfd, "Invaild command.");
      continue;
    }
  }
}

void ParkingLot::getSpace(char *msg_buf)
{
  int sum = 0, n = 0;
  char *gridState = this->getGridState();

  for (int i = 0; i < 3; i++)
  {
    sum = 0;
    for (int j = 0; j < 8; j++)
    {
      sum += (int)((gridState[i] & bit(j)) != 0);
    }
    // printf("P%d: %d\n", i, 8-sum);
    n = sprintf(msg_buf, "%.*sP%d: %d\n", n, msg_buf, i + 1, 8 - sum);
    n = sprintf(msg_buf, "%.*sgrid ", n, msg_buf);
    for (int j = 0; j < 8; j++)
    {
      if ((gridState[i] & bit(j)) == 0)
      {
        n = sprintf(msg_buf, "%.*s|%d", n, msg_buf, j + 1);
      }
    }
    n = sprintf(msg_buf, "%.*s|\n", n, msg_buf);
  }
}

int ParkingLot::selectGrid(int cfd, int lot, int grid)
{
  // printf("selecting %d, %d\n", lot, grid);
  char *gridState = this->getGridState();
  if (lot > 3 || lot < 1 || grid > 8 || grid < 1)
  {
    // std::cerr << "invalid input" << sysState << std::endl;
    printMessage(cfd, "Invaild command.");
    return -1;
  }
  else if ((bit(grid - 1) & gridState[lot - 1]) == 0)
  {
    currentVehicle.second.lot = lot;
    currentVehicle.second.grid = grid;
    return 0;
  }
  else
  {
    printMessage(cfd, "Error! Please select an ideal grid.");
    return -1;
  }
}

int ParkingLot::showPay(int cfd, int* vehId)
{
  // <!-- if action == cancel -->
  // Reserve fee: $20
  char msg_buf[MSG_BUFSIZE] = {0};
  if (currentVehicle.second.state == VEH_STATE_PARKED)
  {
    sprintf(msg_buf, "Parking fee: $40.");
    balance += 30;
  }
  else if (currentVehicle.second.state == VEH_STATE_PARKED_NO_RESERVED)
  {
    sprintf(msg_buf, "Parking fee: $30.");
    balance += 40;
  }
  else if (currentVehicle.second.state == VEH_STATE_RESERVED)
  {
    sprintf(msg_buf, "Reserve fee: $20.");
    balance += 20;
  }

  currentVehicle.second.state = VEH_STATE_INIT;
  this->setVehState(currentVehicle.first, currentVehicle.second);

  printMessage(cfd, msg_buf);

  return PL_STATE_WELCOME;
}

VehicleInfo ParkingLot::getVehState(int id)
{
  std::map<int, VehicleInfo>::iterator veh = vehList.find(id);
  if (veh != vehList.end())
  {
    return veh->second;
  }
  else
  {
    std::pair<int, VehicleInfo> _veh(id, VehicleInfo(VEH_STATE_INIT));
    vehList.insert(_veh);
    return _veh.second;
  }
}

void ParkingLot::setVehState(int id, VehicleInfo state)
{
  std::map<int, VehicleInfo>::iterator veh = vehList.find(id);
  if (veh != vehList.end())
  {
    veh->second = state;
  }
  else
  {
    vehList.insert(std::pair<int, VehicleInfo>(id, state));
  }
}

char *ParkingLot::getGridState()
{
  char *gridState = new char[3];
  // std::cout << "getting\n";
  for (std::map<int, VehicleInfo>::iterator it = vehList.begin(); it != vehList.end(); it++)
  {
    // std::cout << "getting\n";
    // printf("%d,%d,%d,%d\n", it->first, it->second.lot, it->second.grid, it->second.state);
    if (it->second.state != VEH_STATE_INIT)
    {
      // printf("%d, %d = 1\n", it->second.lot, bit(it->second.grid-1));
      gridState[it->second.lot - 1] |= bit(it->second.grid - 1);
      // printf("lot:%d\n", (int)gridState[it->second.lot -1]);
    }
  }

  return gridState;
}

int ParkingLot::printMessage(int conn, const char *msg, bool erase)
{
  return write(conn, msg, strlen(msg));
}

int ParkingLot::getMessage(int conn, char *msg)
{
  int n;
  if ((n = read(conn, msg, MSG_BUFSIZE)) == 0)
  {
    // printf("Connection closed\n");
    close(conn);
    return -1;
  }
  if (msg[n - 1] == '\n')
    msg[n - 1] = '\0';
  else
    msg[n] = '\0';

  printf("[Connection ID:%d]cmd> %s\n", conn, msg);
  return n - 1;
}

void ParkingLot::logStatus(const char *log_file)
{
  int log_fd, n;
  char msg_buf[MSG_BUFSIZE];
  log_fd = open(log_file, O_CREAT | O_RDWR);
  printf("logging to file\n");
  for (std::map<int, VehicleInfo>::iterator it = vehList.begin(); it != vehList.end(); it++)
  {
    if (it->second.state != VEH_STATE_INIT)
    {
      n = sprintf(msg_buf, "%d,%d,%d\n", it->second.lot, it->second.grid, it->first);
      n = write(log_fd, msg_buf, n);
      printf("%s", msg_buf);
    }
  }
  n = sprintf(msg_buf, "Total income : $%d\n", balance);
  n = write(log_fd, msg_buf, n);
  printf("%s", msg_buf);
  close(log_fd);
}

int ParkingLot::stopSystem()
{
  stop = 1;
  return 0;
}