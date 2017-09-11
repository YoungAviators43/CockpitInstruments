/// CockpitNode controller firmware test suite.
///
/// Test the serial message framing.

#include "CppUTest/TestHarness.h"

extern "C"
{
#include "CommonDefs.h"
#include "ServoPwmSpy.h"
#include "ServoChannel.h"
}

TEST_GROUP(ServoChannel)
{
  ServoChannel_t *channel;
  ServoChannelConfig_t channel_1_configuration;

  void setup(void)
  {
    ServoPwm_Create();
    channel = ServoChannel_Create(SERVO_CHANNEL_1);

    channel_1_configuration.servoPwmId = ServoPwm_A;
    channel_1_configuration.domainDefaultPosition = 50;
    channel_1_configuration.domainLowerBound = -50;
    channel_1_configuration.domainUpperBound = 150;
    channel_1_configuration.servoDampingFactor = 0;
    channel_1_configuration.translationTable[0].domain = -50;
    channel_1_configuration.translationTable[0].servo = 4000;
    channel_1_configuration.translationTable[1].domain = 0;
    channel_1_configuration.translationTable[1].servo = 4600;
    channel_1_configuration.translationTable[2].domain = 150;
    channel_1_configuration.translationTable[2].servo = 6000;
  }

  void teardown(void)
  {
    ServoChannel_Destroy();
  }

  void RunServoChannelProcess(ServoChannel_t *servoChannel, uint32_t num)
  {
    while (num-- > 0)
      ServoChannel_Process(servoChannel);
  }

};

TEST(ServoChannel, Create)
{
  CHECK(ServoPwmSpy_OkInit());
  CHECK(!ServoChannel_IsActive(channel));
}

TEST(ServoChannel, NotConfigured)
{
  ServoChannel_Activate(channel);
  CHECK(!ServoChannel_IsActive(channel));  // Not configured, so can't activate.
}

TEST(ServoChannel, Configure)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  CHECK(ServoChannel_IsActive(channel));
}

TEST(ServoChannel, AtDefaultPosition)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  CHECK_EQUAL(50, ServoChannel_GetPosition(channel));
}

TEST(ServoChannel, BistInitialMidPoint)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 1);
  CHECK_EQUAL(3456, ServoPwmSpy_GetPwmValueA());
  RunServoChannelProcess(channel, 498);
  CHECK_EQUAL(3456, ServoPwmSpy_GetPwmValueA());
}

TEST(ServoChannel, BistCalibrationPointOne)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500);
  CHECK_EQUAL(3225, ServoPwmSpy_GetPwmValueA());  // still at mid-point.
}

TEST(ServoChannel, BistCalibrationPointTwo)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25);
  CHECK_EQUAL(3363, ServoPwmSpy_GetPwmValueA());  // still at mid-point.
}

TEST(ServoChannel, BistCalibrationPointThree)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25);
  CHECK_EQUAL(3686, ServoPwmSpy_GetPwmValueA());  // still at mid-point.
}

TEST(ServoChannel, SetNewPositionLow)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, -50);
  CHECK_EQUAL(-50, ServoChannel_GetPosition(channel));
  CHECK_EQUAL(4000, ServoChannel_GetServoRequestPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(4000, ServoChannel_GetServoActualPosition(channel));
}

TEST(ServoChannel, SetNewPositionHigh)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, 150);
  CHECK_EQUAL(150, ServoChannel_GetPosition(channel));
  CHECK_EQUAL(6000, ServoChannel_GetServoRequestPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(6000, ServoChannel_GetServoActualPosition(channel));
}

TEST(ServoChannel, SetNewPositionMiddle1)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, -25);
  CHECK_EQUAL(-25, ServoChannel_GetPosition(channel));
  CHECK_EQUAL(4300, ServoChannel_GetServoRequestPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(4300, ServoChannel_GetServoActualPosition(channel));
}


TEST(ServoChannel, SetNewPositionMiddle2)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, 100);
  CHECK_EQUAL(100, ServoChannel_GetPosition(channel));
  CHECK_EQUAL(5532, ServoChannel_GetServoRequestPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5532, ServoChannel_GetServoActualPosition(channel));
}

TEST(ServoChannel, SetNewPositionMiddleReverse1)
{
  // Servo actuates on reverse scale.
  channel_1_configuration.translationTable[0].domain = -50;
  channel_1_configuration.translationTable[0].servo = 6000;
  channel_1_configuration.translationTable[1].domain = 0;
  channel_1_configuration.translationTable[1].servo = 5400;
  channel_1_configuration.translationTable[2].domain = 150;
  channel_1_configuration.translationTable[2].servo = 4000;

  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, -25);
  CHECK_EQUAL(-25, ServoChannel_GetPosition(channel));
  CHECK_EQUAL(5700, ServoChannel_GetServoRequestPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5700, ServoChannel_GetServoActualPosition(channel));
}


TEST(ServoChannel, SetNewPositionMiddleReverse2)
{
  // Servo actuates on reverse scale.
  channel_1_configuration.translationTable[0].domain = -50;
  channel_1_configuration.translationTable[0].servo = 6000;
  channel_1_configuration.translationTable[1].domain = 0;
  channel_1_configuration.translationTable[1].servo = 5400;
  channel_1_configuration.translationTable[2].domain = 150;
  channel_1_configuration.translationTable[2].servo = 4000;

  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, 100);
  CHECK_EQUAL(100, ServoChannel_GetPosition(channel));
  CHECK_EQUAL(4467, ServoChannel_GetServoRequestPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(4467, ServoChannel_GetServoActualPosition(channel));
}

TEST(ServoChannel, SetPositionBoundLow)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, -100);
  CHECK_EQUAL(-50, ServoChannel_GetPosition(channel));
}

TEST(ServoChannel, SetPositionBoundHigh)
{
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, 200);
  CHECK_EQUAL(150, ServoChannel_GetPosition(channel));
}


TEST(ServoChannel, ServoPositionWithDamping)
{
  channel_1_configuration.servoDampingFactor = 128;
  ServoChannel_Configure(channel, &channel_1_configuration);
  ServoChannel_Activate(channel);
  RunServoChannelProcess(channel, 500 + 25 + 25 + 25);
  ServoChannel_SetPosition(channel, -50);
  CHECK_EQUAL(-50, ServoChannel_GetPosition(channel));
  RunServoChannelProcess(channel, 200);   // assure damping complete
  CHECK_EQUAL(4000, ServoChannel_GetServoRequestPosition(channel));
  CHECK_EQUAL(4000, ServoChannel_GetServoActualPosition(channel));
  ServoChannel_SetPosition(channel, 150);
  CHECK_EQUAL(6000, ServoChannel_GetServoRequestPosition(channel));
  CHECK_EQUAL(4000, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5000, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5500, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5750, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5875, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5937, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5968, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5984, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5992, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(5996, ServoChannel_GetServoActualPosition(channel));
  RunServoChannelProcess(channel, 10);
  CHECK_EQUAL(6000, ServoChannel_GetServoActualPosition(channel));

}

#if 0
TEST(ServoChannel, SetServoCalibrate1)
{
  ServoChannel_SetMode(channel, SERVO_MODE_CALIBRATE);
  ServoChannel_RequestServoPosition(channel, 2500U);
  ServoChannel_Process(channel);
  CHECK(ServoChannel_GetActualServoPosition(channel) == 2500U);
  CHECK_EQUAL(2880, ServoPwmSpy_GetPwmValueA());
}

TEST(ServoChannel, SetServoCalibrate2)
{
  ServoChannel_SetMode(channel, SERVO_MODE_CALIBRATE);
  ServoChannel_RequestServoPosition(channel, 5000U);
  ServoChannel_Process(channel);
  CHECK(ServoChannel_GetActualServoPosition(channel) == 5000U);
  CHECK_EQUAL(3456, ServoPwmSpy_GetPwmValueA());
}

TEST(ServoChannel, SetServoCalibrate3)
{
  ServoChannel_SetMode(channel, SERVO_MODE_CALIBRATE);
  ServoChannel_RequestServoPosition(channel, 7500U);
  ServoChannel_Process(channel);
  CHECK(ServoChannel_GetActualServoPosition(channel) == 7500U);
  CHECK_EQUAL(4032, ServoPwmSpy_GetPwmValueA());
}

//??? not right
TEST(ServoChannel, SetServoNotCalibrateFail)
{
  ServoChannel_SetMode(channel, SERVO_MODE_CALIBRATE);
  ServoChannel_RequestServoPosition(channel, 64);
  ServoChannel_Process(channel);
  ServoChannel_SetMode(channel, SERVO_MODE_ACTIVE);
  ServoChannel_RequestServoPosition(channel, 128);
  ServoChannel_Process(channel);
  CHECK(ServoChannel_GetActualServoPosition(channel) != 128);
}

TEST(ServoChannel, SetDomainPosition)
{
  ServoChannel_SetMode(channel, SERVO_MODE_CALIBRATE);
  ServoChannel_RequestServoPosition(channel, 5000U);
  ServoChannel_Process(channel);

  ServoChannel_SetMode(channel, SERVO_MODE_ACTIVE);
  ServoChannel_SetDomainLimits(channel, -500, 1500);
  ServoChannel_SetPosition(channel, 0);
  ServoChannel_Process(channel);
  CHECK_EQUAL(2880, ServoPwmSpy_GetPwmValueA());
}

TEST(ServoChannel, SetDomainPositionDamped)
{
  ServoChannel_SetMode(channel, SERVO_MODE_CALIBRATE);
  ServoChannel_RequestServoPosition(channel, 5000U);
  ServoChannel_Process(channel);

  ServoChannel_SetMode(channel, SERVO_MODE_ACTIVE);
  ServoChannel_SetDomainLimits(channel, -500, 1500);
  ServoChannel_SetDamping(channel, 160);
  ServoChannel_SetPosition(channel, 0);
  RunServoChannelProcess(channel, 200);
  CHECK_EQUAL(2880, ServoPwmSpy_GetPwmValueA());

  ServoChannel_SetPosition(channel, 250);
  RunServoChannelProcess(channel, 200);
  CHECK_EQUAL(3168, ServoPwmSpy_GetPwmValueA());

  ServoChannel_SetPosition(channel, 1250);
  RunServoChannelProcess(channel, 200);
  CHECK_EQUAL(4320, ServoPwmSpy_GetPwmValueA());

  ServoChannel_SetPosition(channel, -100);
  RunServoChannelProcess(channel, 200);
  CHECK_EQUAL(2764, ServoPwmSpy_GetPwmValueA());
}

TEST(ServoChannel, SetCalibrationTable)
{
  ServoChannel_SetCalibrationTable(channel, NULL);
  CHECK(ServoChannel_GetCalibrationTable(channel) == NULL);
}
#endif
