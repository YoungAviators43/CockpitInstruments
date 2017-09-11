#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <sys/epoll.h>
using namespace std;

#define TEST

#include "ProcessIndicators.hxx"
#include "CockpitNode.hxx"
#include "CockpitPanel.hxx"

void initControlProperties(void);
void processControls(char *pMessage);

void initFgInterfaceIn(int portIn);
void initFgInterfaceOut(int portOut);
void receiveFgPacket(char *buffer, size_t bufferSize);
void sendFgPacket(char *buffer);

static char fgBuffer[4096];

void processFgInput(void)
{
  memset(fgBuffer, 0, sizeof(fgBuffer));
  receiveFgPacket(fgBuffer, sizeof(fgBuffer));
  processIndicators(fgBuffer);
}

void processFgOutput(void)
{
  processControls(fgBuffer);
  sendFgPacket(fgBuffer);
}

static int epfd;

const uint32_t ioFgInterfaceIn = 98;
const uint32_t ioFgInterfaceOut = 99;

#include <string.h>
#include <errno.h>
#include <stdlib.h>

void errExit(const char *msg)
{
  cout << "Fatal error in " << msg << ": " << strerror(errno) << endl;
  _Exit(2);
}

void initIoController(void)
{
  epfd = epoll_create1(0);
  if (epfd == -1)
  {
    errExit("epoll_create");
  }
}

void ioControlAddNodeRead(CockpitNodeComm_t *pCommChannel, int fd)
{
  int result;
  struct epoll_event ev;

  ev.data.ptr = (void *)pCommChannel;
  ev.events = EPOLLIN;
  result = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
  if (result == -1)
  {
    errExit("epoll_add");
  }
}

void ioControlAddNodeWrite(CockpitNodeComm_t *pCommChannel, int fd)
{
  int result;
  struct epoll_event ev;

  ev.data.ptr = (void *)pCommChannel;
  ev.events = EPOLLIN | EPOLLOUT;
  result = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
  if (result == -1)
  {
    errExit("epoll_add_write");
  }
}

void ioControlRemoveNodeWrite(CockpitNodeComm_t *pCommChannel, int fd)
{
  int result;
  struct epoll_event ev;

  ev.data.ptr = (void *)pCommChannel;
  ev.events = EPOLLIN;
  result = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
  if (result == -1)
  {
    errExit("epoll_remove_write");
  }
}

void ioControlRemoveNode(CockpitNodeComm_t *pChannel, int fd)
{
}

void ioControlAddFgRead(int fd)
{
  int result;
  struct epoll_event ev;

  //  cout << "ioControlAddFgRead: " << fd << endl;

  ev.data.u32 = (uint32_t)ioFgInterfaceIn;
  ev.events = EPOLLIN;
  result = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
  if (result == -1)
  {
    errExit("epoll_fgAddRead");
  }
}

void ioControlAddFgWrite(int fd)
{
  int result;
  struct epoll_event ev;

  //  cout << "ioControlAddFgWrite: " << fd << endl;

  ev.data.u32 = (uint32_t)ioFgInterfaceOut;
  ev.events = EPOLLOUT;
  result = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
  if (result == -1)
  {
    errExit("epoll_fgAddWrite");
  }
}

void ioControlRemoveFgWrite(int fd)
{
  int result;
  struct epoll_event ev;

  //  cout << "ioControlRemoveFgWrite: " << fd << endl;

  ev.data.u32 = (uint32_t)ioFgInterfaceOut;
  ev.events = EPOLLOUT;
  result = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
  if (result == -1)
  {
    errExit("epoll_fgRemoveWrite");
  }
}

static void ioProcessFgInterfaceIn(uint32_t events)
{
  if ((events & EPOLLIN) != 0)
  {
    processFgInput();
  }
}

static void ioProcessFgInterfaceOut(uint32_t events)
{
  if ((events & EPOLLOUT) != 0)
  {
    processFgOutput();
  }
}

void ioProcessNodeEvent(CockpitNodeComm_t *pNode, uint32_t events)
{
  if ((events & EPOLLHUP) != 0)
  {
    processCockpitNodeHup(pNode);
  }

  if ((events & EPOLLOUT) != 0)
  {
    processCockpitNodeWrite(pNode);
  }

  if ((events & EPOLLIN) != 0)
  {
    processCockpitNodeRead(pNode);
  }
}

#define MAX_EVENTS  32
struct epoll_event eventList[MAX_EVENTS];

void ioControlProcess(uint32_t msTimeout)
{
  int numEvents;
  int i;
  uint32_t events;

  numEvents = epoll_wait(epfd, eventList, MAX_EVENTS, msTimeout);
  if (numEvents == -1)
  {
    errExit("epoll_wait");
  }
  else if (numEvents > 0)
  {
    //    cout << "ioControlProcess: " << numEvents << " event(s)." << endl;
    for (i = 0; i < numEvents; i++)
    {
      events = eventList[i].events;
      switch (eventList[i].data.u32)
      {
        case ioFgInterfaceIn:
	  ioProcessFgInterfaceIn(events);
	  break;
        case ioFgInterfaceOut:
	  ioProcessFgInterfaceOut(events);
	  break;
        default:
	  ioProcessNodeEvent((CockpitNodeComm_t *)eventList[i].data.ptr, events);
	  break;
      }
    }
  }
#if 0
  else
    cout << "ioControlProcess: epoll timeout" << endl;
#endif
}


#define ITERATIONS 1000000

int main()
{
  int i;

  cout << "Cockpit Controller: Start" << endl;

  initIoController();
  initIndicatorProperties();
  initControlProperties();
  initCockpitPanel();
  initFgInterfaceIn(4860);
  initFgInterfaceOut(4860);

  for (i = 0; i < ITERATIONS; i++)
  {
    if ((i % 5000) == 0)
      cout << "Main Iteration: " << i << endl;
    ioControlProcess(50);
    processCockpitIndicators();
  }

  cout << "Cockpit Controller: complete: " << i << " interations." << endl;

  return 0;
}
