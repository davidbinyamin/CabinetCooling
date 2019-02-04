#ifndef _FANCOTROL_H_
#define _FANCOTROL_H_
#include <Arduino.h>

class FanControl {
  public:
    FanControl(const int pin, int smoothingCount, int minimumPwm=0);
	void init();
	void startFans();
	void stopFans();
	void updateFanDuty(double temperature);
	int getFanDuty();
	void setFanSpeed ();

  private:
	void pwm25kHzBegin();
	void pwmDuty(byte ocrb);

  int m_minDuty;
  int m_pin;
	int m_fanDutyPercentage = 0;
	int m_smoothCount;
	int m_count;
	int m_fansState = LOW;

};

#endif //_FANCOTROL_H_
