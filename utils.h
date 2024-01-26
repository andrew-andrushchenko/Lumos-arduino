#pragma once

inline void m_delay(int delayMillis)
{
  unsigned long prevMillis = millis();
  while (millis() - prevMillis <= delayMillis)
    ;
}