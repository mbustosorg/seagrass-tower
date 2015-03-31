#include <TinyGPS.h>                                // Required.
#include "gps2rtc.h"                                // Required.

const int gps_1pps_pin = 9;
const int ledPin = 13;
elapsedMillis t;
GPS2RTC gps2rtc;                                    // Required.

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  gps2rtc.begin(&Serial3, 9600, gps_1pps_pin);      // Required.
//RTC_TCR = 0xdff;
//RTC_CR = RTC_CR_SC16P | RTC_CR_SC4P | RTC_CR_OSCE;
//RTC_TCR = 0;
//RTC_CR = RTC_CR_SC16P | RTC_CR_SC8P | RTC_CR_SC4P | RTC_CR_OSCE;
}

void loop()
{
  uint l_tpr;
  static int i;
  static int l_1pps_on = -1;
  static int l_1pps_off = -1;
  static boolean l_waiting_for_1pps = true;
  int l_tcr;
  int l_seconds;

  // Turn on the LED for about 100 ms starting at the beginning of the second.
  l_tpr = RTC_TPR;                // The prescaler register goes from 0-32767 every second.

  if (l_waiting_for_1pps)
  {
    if (digitalRead(gps_1pps_pin) == HIGH)
    {
      l_1pps_on = l_tpr;
      l_waiting_for_1pps = false;
    }
  }
  else
  {
    if (digitalRead(gps_1pps_pin) == LOW)
    {
      l_1pps_off = l_tpr;
      l_waiting_for_1pps = true;
    }
  }

  if (l_tpr >= 0 && l_tpr < 20)
    digitalWrite(ledPin, HIGH);
  
  if (l_tpr >= 3277 && l_tpr < 3290)
    digitalWrite(ledPin, LOW);

  if (false)
  {
    Serial.print("\r");
    l_seconds = rtc_get();
    Serial.print(l_seconds);
    Serial.print("   ");
    Serial.print(gps2rtc.gps_time);
    Serial.print("   ");
    Serial.print(int(l_seconds / 3600));
    Serial.print(":");
    l_seconds = l_seconds - int(l_seconds / 3600) * 3600;
    Serial.print(int(l_seconds / 60));
    Serial.print(":");
    Serial.print(l_seconds - int(l_seconds / 60) * 60);
    Serial.print("     ");
  }

  // Print the time every 333 ms.
  if (t >= 333 && true)
  {
    t = t - 333;
    l_tcr = RTC_TCR;
    Serial.print("\r");
    Serial.print(i);
    Serial.print("   ");
    Serial.print(gps2rtc.gps_time);
    Serial.print("   ");
    Serial.print(rtc_get());
    Serial.print("   ");
    Serial.print(l_tcr >> 24);                          // CIC
    Serial.print("       ");
    Serial.print((l_tcr >> 16) & 0xff, HEX);            // TCV
    Serial.print("       ");
    Serial.print(l_1pps_on);
    Serial.print("   ");
    Serial.print(l_1pps_off);
    Serial.print("       ");
    Serial.print(l_tcr & 0xffff, HEX);
    Serial.print("       ");
    Serial.print(gps2rtc.state);
    Serial.print("       ");
    i++;
  }
}

void my_isr(int a_q)
{
  pinMode(ledPin, OUTPUT);
  while (1)
  {
    delay(1000);
    for (int i = 0; i < a_q; i++)
    {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }
  }
};

void nmi_isr(void)
{
  my_isr(1);
};
void hard_fault_isr(void)
{
  my_isr(2);
};
void memmanage_fault_isr(void)
{
  my_isr(3);
};
void bus_fault_isr(void)
{
  my_isr(4);
};
void usage_fault_isr(void)
{
  my_isr(5);
};
void svcall_isr(void)
{
  my_isr(6);
};
void debugmonitor_isr(void)
{
  my_isr(7);
};
void pendablesrvreq_isr(void)
{
  my_isr(8);
};
void uart0_error_isr(void)
{
  my_isr(10);
};
void uart1_error_isr(void)
{
  my_isr(12);
};
void uart2_error_isr(void)
{
  my_isr(14);
};
void software_isr(void)
{
  my_isr(15);
};
void mcg_isr(void)
{
  my_isr(16);
};

