/// CockpitNode controller firmware test suite.
///
/// Test the timer functionality.

#include "CppUTest/TestHarness.h"

extern "C"
{
#include "CommonDefs.h"
#include "TimerSpy.h"
}

TEST_GROUP(TimerService)
{
  void setup(void)
  {
    TimerServiceSpy_Reset();
    TimerService_Create();
  }

  void teardown(void)
  {
    TimerService_Destroy();
  }
};

/// Creation of SerialFrame module creates SerialUart module.
TEST(TimerService, Create)
{
  CHECK(TimerServiceSpy_IsTimerCreated());
  CHECK(!TimerServiceSpy_IsTimerActive());
}

TEST(TimerService, Activate)
{
  TimerService_Activate(100, TimerServiceSpy_service);
  CHECK(TimerServiceSpy_IsTimerActive());
  CHECK(TimerServiceSpy_IsCTCMode());
  CHECK(TimerServiceSpy_IsRate(100));
  TimerService_Activate(200, TimerServiceSpy_service);
  CHECK(TimerServiceSpy_IsRate(200));
}

TEST(TimerService, Rollover)
{
  TimerService_Activate(100, TimerServiceSpy_service);
  TimerServiceSpy_TimerRollover(1);
  CHECK(TimerServiceSpy_TimerProcessCalled() == 1);
  TimerServiceSpy_TimerRollover(10);
  CHECK(TimerServiceSpy_TimerProcessCalled() == 11);
  TimerServiceSpy_TimerRollover(5);
  CHECK(TimerServiceSpy_TimerProcessCalled() == 16);
}
