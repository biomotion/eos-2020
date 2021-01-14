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

void ParkingLot::runSystem()
{
  int ret;
  pthread_t *thread = new pthread_t();

  while (!stop)
  {
    if ((ret = waitConnection()) < 0)
    {
      perror("Error: wait_Connection\n");
    }
    ClientSession *session = new ClientSession(ret, *this);

    if (pthread_create(thread, NULL, (void *(*)(void *)) & (this->handleConnection), (void *)session))
    {
      perror("Error: pthread_create()\n");
    }
    clientThreads.push_back(thread);
  }
  // for(std::vector<pthread_t*>::iterator it=clientThreads.begin(); it!=clientThreads.end(); it++){
  //   pthread_join()
  // }
}

void *ParkingLot::handleConnection(void *p)
{
  ClientSession *s = (ClientSession *)p;
  printf("running session\n");
  s->run();
  printf("exit\n");
  pthread_exit(NULL);
}

void ParkingLot::getSpace(char *msg_buf)
{
  int sum = 0, n = 0;
  char gridState[3] = {0};
  this->getGridState(gridState);

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

int ParkingLot::selectGrid(int lot, int grid)
{
  printf("selecting %d, %d\n", lot, grid);
  char gridState[3] = {0};
  this->getGridState(gridState);
  if (lot > 3 || lot < 1 || grid > 8 || grid < 1)
  {
    return 1;
  }
  else if ((bit(grid - 1) & gridState[lot - 1]) == 0)
  {
    return 0;
  }
  else
  {
    return -1;
  }
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

void ParkingLot::getGridState(char state[3])
{
  // std::cout << "getting\n";
  printf("len(vehList)=%d\n", vehList.size());
  for (std::map<int, VehicleInfo>::iterator it = vehList.begin(); it != vehList.end(); it++)
  {
    // std::cout << "getting\n";
    printf("%d,%d,%d,%d\n", it->first, it->second.lot, it->second.grid, it->second.state);
    if (it->second.state != VEH_STATE_INIT)
    {
      // printf("%d, %d = 1\n", it->second.lot, bit(it->second.grid-1));
      state[it->second.lot - 1] |= bit(it->second.grid - 1);
      // printf("lot:%d\n", (int)gridState[it->second.lot -1]);
    }
  }
  printf("return\n");
  return;
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

ClientSession::ClientSession(int connfd_, ParkingLot &lot_)
{
  connfd = connfd_;
  lot = &lot_;
  clientState = CLIENT_STATE_WELCOME;
  printf("initialized\n");
}

int ClientSession::showWelcome()
{
  char buf[MSG_BUFSIZE];
  // Welcome message on lcd
  // printMessage("Input plate num: ");
  // Get Vehicle ID input
  // getMessage()
  if (getMessage(buf) == -1)
  {
    return CLIENT_STATE_EXIT;
  }

  this->veh.first = atoi(buf); //assign input data;
  // search stored vehicle & get vehicle state
  this->veh.second = this->getLot()->getVehState(this->veh.first);

  // TODO
  // loggedInVeh.push_back(this->veh.first);

  if (this->veh.second.state == VEH_STATE_INIT)
  {
    return CLIENT_STATE_MENU_NOT_RESERVED;
  }
  else if (this->veh.second.state == VEH_STATE_RESERVED)
  {
    return CLIENT_STATE_MENU_RESERVED;
  }
  else
  { // PARKED or PARKED_NOT_RESERVED
    return CLIENT_STATE_MENU_PARKED;
  }
}

int ClientSession::showMenuNotReserved()
{
  char msg_buf[MSG_BUFSIZE];
  char *tok;
  int lot, grid;

  printMessage("You haven't reserved grid.");
  while (1)
  {
    if (getMessage(msg_buf) == -1)
    {
      return CLIENT_STATE_EXIT;
    }

    // printf("%s", msg_buf);
    tok = strtok(msg_buf, " ");
    // printf("tok = %s\n", tok);
    // printf("msg = %s\n", msg_buf);
    if (strcmp(tok, "show") == 0)
    {
      this->getLot()->getSpace(msg_buf);
      printMessage(msg_buf);
      continue;
    }
    else if (strcmp(tok, "reserve") == 0)
    {
      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage("Invaild command.");
        continue;
      }
      lot = atoi(tok);

      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage("Invaild command.");
        continue;
      }
      grid = atoi(tok);
      if (this->getLot()->selectGrid(lot, grid) == 0)
      {
        this->veh.second.state = VEH_STATE_RESERVED;
        this->veh.second.lot = lot;
        this->veh.second.grid = grid;
        this->getLot()->setVehState(this->veh.first, this->veh.second);
        printMessage("Reserve successful.");
        return CLIENT_STATE_WELCOME;
      }else if (this->getLot()->selectGrid(lot, grid) == -1){
        printMessage("Error! Please select an ideal grid.");
      }else{
        printMessage("Invaild command.");
      }
      // return CLIENT_STATE_CHOOSE;
    }
    else if (strcmp(tok, "check-in") == 0)
    {
      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage("Invaild command.");
        continue;
      }
      lot = atoi(tok);

      tok = strtok(NULL, " ");
      if (tok == NULL)
      {
        printMessage("Invaild command.");
        continue;
      }
      grid = atoi(tok);
      if (this->getLot()->selectGrid(lot, grid) == 0)
      {
        this->veh.second.state = VEH_STATE_PARKED;
        this->veh.second.lot = lot;
        this->veh.second.grid = grid;
        this->getLot()->setVehState(this->veh.first, this->veh.second);
        printMessage("Check-in successful.");
        return CLIENT_STATE_WELCOME;
      }else if (this->getLot()->selectGrid(lot, grid) == -1){
        printMessage("Error! Please select an ideal grid.");
      }else{
        printMessage("Invaild command.");
      }
    }
    else if (strcmp(tok, "exit") == 0)
    {
      printMessage("Logout.");
      return CLIENT_STATE_WELCOME;
    }
    else
    {
      // std::cerr << "invalid input" << sysState << std::endl;
      printMessage("Invaild command.");
      continue;
    }
  }
}

int ClientSession::showMenuReserved()
{
  char msg_buf[MSG_BUFSIZE];

  printMessage("You have reserved grid.");

  while (1)
  {
    //get input
    if (getMessage(msg_buf) == -1)
    {
      return CLIENT_STATE_EXIT;
    }

    // printf("%s", msg_buf);

    if (strcmp(msg_buf, "show") == 0)
    {
      this->getLot()->getSpace(msg_buf);
      printMessage(msg_buf);
      continue;
    }
    else if (strcmp(msg_buf, "cancel") == 0)
      return CLIENT_STATE_PAY;
    else if (strcmp(msg_buf, "check-in") == 0)
    {
      this->veh.second.state = VEH_STATE_PARKED;
      // this->veh.second.lot = lot;
      // this->veh.second.grid = grid;
      this->getLot()->setVehState(this->veh.first, this->veh.second);
      printMessage("Check-in successful.");
      return CLIENT_STATE_WELCOME;
    }
    else if (strcmp(msg_buf, "exit") == 0)
    {
      return CLIENT_STATE_WELCOME;
      printMessage("Logout.");
    }
    else
    {
      // std::cerr << "invalid input" << sysState << std::endl;
      printMessage("Invaild command.");
      continue;
    }
  }
}

int ClientSession::showMenuParked()
{
  char msg_buf[MSG_BUFSIZE];

  sprintf(msg_buf, "Your grid is at lot P%d grid %d.", this->veh.second.lot, this->veh.second.grid);
  printMessage(msg_buf);
  while (1)
  {

    //get input
    if (getMessage(msg_buf) == -1)
    {
      return CLIENT_STATE_EXIT;
    }

    // printf("%s", msg_buf);
    if (strcmp(msg_buf, "show") == 0)
    {
      this->getLot()->getSpace(msg_buf);
      printMessage(msg_buf);
      return CLIENT_STATE_MENU_NOT_RESERVED;
    }
    else if (strcmp(msg_buf, "pick-up") == 0)
      return CLIENT_STATE_PAY;
    else if (strcmp(msg_buf, "exit") == 0)
    {
      printMessage("Logout.");
      return CLIENT_STATE_WELCOME;
    }
    else
    {
      // std::cerr << "invalid input" << sysState << std::endl;
      printMessage("Invaild command.");
      continue;
    }
  }
}

int ClientSession::showPay()
{
  // <!-- if action == cancel -->
  // Reserve fee: $20
  char msg_buf[MSG_BUFSIZE] = {0};
  if (this->veh.second.state == VEH_STATE_PARKED)
  {
    sprintf(msg_buf, "Parking fee: $40.");
    this->getLot()->balance += 30;
  }
  else if (this->veh.second.state == VEH_STATE_PARKED_NO_RESERVED)
  {
    sprintf(msg_buf, "Parking fee: $30.");
    this->getLot()->balance += 40;
  }
  else if (this->veh.second.state == VEH_STATE_RESERVED)
  {
    sprintf(msg_buf, "Reserve fee: $20.");
    this->getLot()->balance += 20;
  }

  this->veh.second.state = VEH_STATE_INIT;
  this->getLot()->setVehState(this->veh.first, this->veh.second);

  printMessage(msg_buf);

  return CLIENT_STATE_WELCOME;
}

int ClientSession::printMessage(const char *msg)
{
  printf("respond>%s\n", msg);
  return write(this->connfd, msg, strlen(msg));
}

int ClientSession::getMessage(char *msg)
{
  int n;
  if ((n = read(this->connfd, msg, MSG_BUFSIZE)) == 0)
  {
    // printf("Connection closed\n");
    close(this->connfd);
    return -1;
  }
  if (msg[n - 1] == '\n')
    msg[n - 1] = '\0';
  else
    msg[n] = '\0';

  printf("[Connection ID:%d]cmd> %s\n", this->connfd, msg);
  return n - 1;
}

void ClientSession::run()
{
  while (clientState != CLIENT_STATE_EXIT)
  {
    switch (clientState)
    {
    case CLIENT_STATE_WELCOME:
      clientState = showWelcome();
      break;
    case CLIENT_STATE_MENU_PARKED:
      clientState = showMenuParked();
      break;
    case CLIENT_STATE_MENU_NOT_RESERVED:
      clientState = showMenuNotReserved();
      break;
    case CLIENT_STATE_MENU_RESERVED:
      clientState = showMenuReserved();
      break;
    case CLIENT_STATE_PAY:
      clientState = showPay();
      break;
    default:
      break;
    }
  }
}
