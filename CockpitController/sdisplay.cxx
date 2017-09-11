/// Cockpit Controller display

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/time.h>
#include <time.h>

#include "CockpitPanel.hxx"
#include "ProcessIndicators.hxx"

static int screenHeight = 0;
static int screenWidth = 0;

static int nNodeRows = 0;
static int nPropertyRows = 0;
static int nStatusRows = 0;
static int nPropertyScroll = 0;

static const int minPropertyRows = 10;
static const int minNodeRows = 8;

const char chHorizontalLine = ACS_HLINE;
const char chVerticalLine = ACS_VLINE;


static void drawScreenBox(int start, int length)
{
  mvhline(start, 1, chHorizontalLine, screenWidth - 2);   // top line
  mvhline(start + length - 1, 1, chHorizontalLine, screenWidth - 2);  // bottom line
  mvvline(start + 1, 0, chVerticalLine, length - 1);  // left line
  mvvline(start + 1, screenWidth - 1, chVerticalLine, length - 2);  // right line

  mvaddch(start, 0, ACS_ULCORNER);  // upper left
  mvaddch(start, screenWidth - 1, ACS_URCORNER);  // upper right

  mvaddch(start + length - 1, 0, ACS_LLCORNER);  // lower left
  mvaddch(start + length - 1, screenWidth - 1, ACS_LRCORNER);  // lower right
}

static void drawDividerConnector(int yLoc)
{
  mvaddch(yLoc, 0, ACS_LTEE);  // upper left
  mvaddch(yLoc, screenWidth - 1, ACS_RTEE);  // upper right
}

/// Create/update the size and position of the display windows.
static void displayWindowUpdate(void)
{
  if (nNodeRows > 0)
  {
    drawScreenBox(0, nNodeRows);                      // Draw node status pane
    drawScreenBox(nNodeRows - 1, nPropertyRows + 2);  // Draw property status pane
    drawDividerConnector(nNodeRows - 1);
  }
  else
  {
    drawScreenBox(0, nPropertyRows);
  }

  // Draw controller status pane
  drawScreenBox(nNodeRows + nPropertyRows, nStatusRows);
  drawDividerConnector(nNodeRows + nPropertyRows);
}

static void displayReset(void)
{
  // From the current screen size, determine the window sizes and draw the frames
  nNodeRows = maxIndicatorTable + 2;   // indicator rows, plus top and bottom separator lines
  nPropertyRows = minPropertyRows;
  nStatusRows = 4;

  if (screenHeight > (nNodeRows + nPropertyRows + nStatusRows))
  {
    nPropertyRows = screenHeight - nNodeRows - nStatusRows;
  }

  displayWindowUpdate();
}

void setScreenSize(int h, int w)
{
  if ((h != screenHeight) || (w != screenWidth))
  {
    clear();

    screenHeight = h;
    screenWidth = w;

    displayReset();
  }
}

static void displayResize(void)
{
  int nh, nw;

  nh = LINES;
  nw = COLS;

  setScreenSize(nh, nw);
}

static int nPropertyRowsRequired = 0;
static char propertyStr[64][256];

char getControlPropertyStatus(int i, char *str);

static void updatePropertyDisplay(void)
{
  int i;
  char str[256];

  nPropertyRowsRequired = 0;

  // Gather "indicator" property rows.
  for (i = 0; ; i++)
  {
    if (getIndicatorPropertyStatus(i, str))
    {
      str[screenWidth - 2] = '\0';
      strcpy(propertyStr[nPropertyRowsRequired], str);
      nPropertyRowsRequired += 1;
    }
    else
    {
      break;
    }
  }

  // Gather "control" property rows.
  for (i = 0; ; i++)
  {
    if (getControlPropertyStatus(i, str))
    {
      str[screenWidth - 2] = '\0';
      strcpy(propertyStr[nPropertyRowsRequired], str);
      nPropertyRowsRequired += 1;
    }
    else
    {
      break;
    }
  }

  // Display all property rows
  for (i = 0; i < nPropertyRows; i++)
  {
    mvhline(nNodeRows + i, 1, ' ', screenWidth - 2);
    if (i + nPropertyScroll < nPropertyRowsRequired)
    {
      move(nNodeRows + i, 1);
      addstr(propertyStr[i + nPropertyScroll]);
    }
  }
}

static void scrollPropertyUp(void)
{
  if (nPropertyRowsRequired > nPropertyRows)
  {
    if (nPropertyScroll < (nPropertyRowsRequired - nPropertyRows))
    {
      nPropertyScroll += 1;
      updatePropertyDisplay();
    }
  }
}

static void scrollPropertyDown(void)
{
  if (nPropertyScroll > 0)
  {
    nPropertyScroll -= 1;
    updatePropertyDisplay();
  }
}

void getCockpitIndicatorStatus(int i, char *str);

static void updateNodeDisplay(void)
{
  int i, j;
  int nChannels;
  char str[256];

  if (nNodeRows > 2)
  {
    nChannels = nNodeRows - 2;
    for (i = 0; i < nChannels; i++)
    {
      mvhline(i + 1, 1, ' ', screenWidth - 2);

      getCockpitIndicatorStatus(i, str);
      str[screenWidth - 2] = '\0';

      move(i + 1, 1);
      //      addstr(str);
      for (j = 0; (j < 180) && (str[j] != '\0'); j++)
	addch(str[j]);
    }
  }
}

static void displayUpdate(void)
{
  if (nNodeRows > 0)
  {
    updateNodeDisplay();
  }

  ///  mvprintw(nNodeRows + 2, 1,  "Screen size[%u]: height: %d, width: %d", count++, screenHeight, screenWidth);

  if (nPropertyRows > 0)
  {
    updatePropertyDisplay();
  }

  wrefresh(stdscr);
}

/// This time elapse implementation slips a little because it is simple, but that is ok for this application.
static unsigned long GetMillisecondTime(void)
{
  struct timeval tv;
  int err;

  err = gettimeofday(&tv, 0);
  if (err != 0)
  {
    mvprintw(nNodeRows + 3, 1,  "Error: Problem getting time-of-day value: %d", err);
    return 0;
  }
  return (unsigned long)((tv.tv_sec * 1000ul) + (tv.tv_usec / 1000ul));
}

static unsigned long GetMillisecondsElapsed(void)
{
  static unsigned long lastMillisecondTime = 0;
  unsigned long millisecondTime;
  unsigned elapsedMilliseconds;

  millisecondTime = GetMillisecondTime();
  if (millisecondTime == 0)
  {
    elapsedMilliseconds = 0;
  }
  else
  {
    elapsedMilliseconds = millisecondTime - lastMillisecondTime;
    if (elapsedMilliseconds < 10ul)
      elapsedMilliseconds = 0;
    else
      lastMillisecondTime = millisecondTime;
  }
  return elapsedMilliseconds;
}

void displayComplete(void)
{
  endwin();
}

static void displayHandleKey(int ch)
{
  switch (ch)
  {
    case KEY_RESIZE:
      displayResize();
      break;
    case 'u':
      scrollPropertyUp();
      break;
    case 'd':
      scrollPropertyDown();
      break;
    case 'q':
      displayComplete();
      exit(0);
      break;
    default:
      break;
  }
}

static time_t startTime;

char *getVersionString(void);
int getRxPort(void);
char *getTxIpAddr(void);
int getTxPort(void);

static void statusUpdate(void)
{
  time_t t;
  uint32_t upTime;
  uint32_t days, hours, minutes, seconds;
  char timeStr[64];
  int len;
  char str[256];

  time(&t);

  // Create time string without \n
  strcpy(timeStr, ctime(&t));
  len = strlen(timeStr);
  if (len > 0)
    timeStr[len - 1] = '\0';

  upTime = (uint32_t)difftime(t, startTime);
  seconds = upTime % 60;
  upTime /= 60;
  minutes = upTime % 60;
  upTime /= 60;
  hours = upTime % 24;
  days = upTime / 24;

  sprintf(str, "%s     Up: %3d:%02d:%02d:%02d   Cockpit Controller: %s,    Rx Port: %d    Tx IP: %s  Tx Port: %d",
	  timeStr,
	  days, hours, minutes, seconds,
          getVersionString(),
          getRxPort(),
          getTxIpAddr(),
          getTxPort());

  str[screenWidth - 2] = '\0';
  mvhline(nNodeRows + nPropertyRows + 1, 1, ' ', screenWidth - 2);
  move(nNodeRows + nPropertyRows + 1, 1);
  addstr(str);
}

void displayDebug(char *str)
{
  move(nNodeRows + nPropertyRows + 2, 1);
  addstr(str);
  wrefresh(stdscr);
}


void displayAddFgWrite(uint32_t count)
{
  char str[128];

  sprintf(str, "FgWrite: success %u", count);
  move(nNodeRows + nPropertyRows + 2, 1);
  addstr(str);
}

void displayFgWriteMsg(int nItems, char *msg)
{
  char str[256];

  move(nNodeRows + nPropertyRows + 2, 24);
  sprintf(str, "FgMsg(%d): [%s]", nItems, msg);
  addstr(str);
}

void displayProcess(void)
{
  static unsigned long ms = 0;
  static unsigned long msStat = 0;
  int ch;

  if ((ch = getch()) != ERR)
  {
    displayHandleKey(ch);
    displayUpdate();
  }
  else
  {
    ms += GetMillisecondsElapsed();
    if (ms >= 100)
    {
      msStat += ms;
      if (msStat >= 500)
	statusUpdate();
      ms = 0;
      displayUpdate();
    }
  }
}

void displayInit(void)
{
  time(&startTime);
 
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  
  displayResize();
}
