#include "FanControl.h"


FanControl::FanControl(const int pin, int smoothingCount, int minimumPwm=0)
{
	m_pin = pin;
	m_smoothCount = smoothingCount;
  if (minimumPwm <= 79 && minimumPwm >=0)
  {
    m_minDuty = minimumPwm;
  }
  else
  {
    m_minDuty = 0;
  }
}

void FanControl::init()
{
	pinMode(m_pin, OUTPUT);
	pinMode(3, OUTPUT);
	stopFans();
	pwm25kHzBegin();
}

void FanControl::startFans()
{
	m_fansState = HIGH;
	digitalWrite(m_pin, m_fansState);
}

void FanControl::stopFans()
{
	m_fansState = LOW;
	digitalWrite(m_pin, m_fansState);
}

void FanControl::updateFanDuty(double temperature)
{
	int newFanDutyPercentage = 0;

    if (temperature < 27)
      newFanDutyPercentage = 0;
    if (temperature >= 28.0 && temperature < 30.0)
      newFanDutyPercentage = 20;
    if (temperature >= 30.0 && temperature < 32.0)
      newFanDutyPercentage = 30;
    if (temperature >= 32.0 && temperature < 34.0)
      newFanDutyPercentage = 50;
    if (temperature >= 34.0 && temperature < 37.0)
      newFanDutyPercentage = 70;
    if (temperature >= 37.0 && temperature < 39.0)
      newFanDutyPercentage = 80;
    if (temperature >= 39.0 && temperature < 40.0)
      newFanDutyPercentage = 90;
    if (temperature >= 40.0)
      newFanDutyPercentage = 100;

    if (newFanDutyPercentage < m_fanDutyPercentage)
    {
      m_count--;
    }
    else
    {
      m_fanDutyPercentage = newFanDutyPercentage;
      m_count = m_smoothCount;
    }

    if (m_count == 0)
    {
      m_count = m_smoothCount;
      m_fanDutyPercentage = newFanDutyPercentage;
    }
}

int FanControl::getFanDuty()
{
	return m_fanDutyPercentage;
}

void FanControl::setFanSpeed()
{
	int duty;
	m_fanDutyPercentage == 0 ? duty = 0 : duty = m_fanDutyPercentage * 55/100 + m_minDuty;
	
	if(duty == 0 && m_fansState == HIGH)
	{
		stopFans();
	}
	else if (duty != 0 && m_fansState == LOW)
	{
		startFans();
		pwmDuty(79);
		delay(500);
	}
	
	pwmDuty(duty);
}

void FanControl::pwm25kHzBegin() {
  TCCR2A = 0;                               // TC2 Control Register A
  TCCR2B = 0;                               // TC2 Control Register B
  TIMSK2 = 0;                               // TC2 Interrupt Mask Register
  TIFR2 = 0;                                // TC2 Interrupt Flag Register
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // OC2B cleared/set on match when up/down counting, fast PWM
  TCCR2B |= (1 << WGM22) | (1 << CS21);     // prescaler 8
  OCR2A = 79;                               // TOP overflow value (Hz)
  OCR2B = 0;
}

void FanControl::pwmDuty(byte ocrb) {
  OCR2B = ocrb;                             // PWM Width (duty)
}
