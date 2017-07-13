#ifndef RTC_ZERO_H
#define RTC_ZERO_H
#include "Arduino.h"
typedef void(*voidFuncPtr)(void);
class RTCZero {
public:
    enum Alarm_Match: uint8_t {
        MATCH_OFF          = RTC_MODE2_MASK_SEL_OFF_Val,          // Never
        MATCH_SS           = RTC_MODE2_MASK_SEL_SS_Val,           // Every Minute
        MATCH_MMSS         = RTC_MODE2_MASK_SEL_MMSS_Val,         // Every Hour
        MATCH_HHMMSS       = RTC_MODE2_MASK_SEL_HHMMSS_Val,       // Every Day
        MATCH_DHHMMSS      = RTC_MODE2_MASK_SEL_DDHHMMSS_Val,     // Every Month
        MATCH_MMDDHHMMSS   = RTC_MODE2_MASK_SEL_MMDDHHMMSS_Val,   // Every Year
        MATCH_YYMMDDHHMMSS = RTC_MODE2_MASK_SEL_YYMMDDHHMMSS_Val  // Once, on a specific date and a specific time
    };
    RTCZero();
    void begin(bool resetTime = false);
    void enableAlarm(Alarm_Match match);
    void disableAlarm();
    void attachInterrupt(voidFuncPtr callback);
    void detachInterrupt();
    void standbyMode();
    uint8_t getSeconds();
    uint8_t getMinutes();
    uint8_t getHours();
    uint8_t getDay();
    uint8_t getMonth();
    uint8_t getYear();
    uint8_t getAlarmSeconds();
    uint8_t getAlarmMinutes();
    uint8_t getAlarmHours();
    uint8_t getAlarmDay();
    uint8_t getAlarmMonth();
    uint8_t getAlarmYear();
    void setSeconds(uint8_t seconds);
    void setMinutes(uint8_t minutes);
    void setHours(uint8_t hours);
    void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
    void setDay(uint8_t day);
    void setMonth(uint8_t month);
    void setYear(uint8_t year);
    void setDate(uint8_t day, uint8_t month, uint8_t year);
    void setAlarmSeconds(uint8_t seconds);
    void setAlarmMinutes(uint8_t minutes);
    void setAlarmHours(uint8_t hours);
    void setAlarmTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
    void setAlarmDay(uint8_t day);
    void setAlarmMonth(uint8_t month);
    void setAlarmYear(uint8_t year);
    void setAlarmDate(uint8_t day, uint8_t month, uint8_t year);
    uint32_t getEpoch();
    uint32_t getY2kEpoch();
    void setEpoch(uint32_t ts);
    void setY2kEpoch(uint32_t ts);
    void setAlarmEpoch(uint32_t ts);
    bool isConfigured() {
        _configured = RTC->MODE2.CTRL.reg & RTC_MODE2_CTRL_ENABLE;
        configureClock();
        return _configured;
    }
private:
    bool _configured;
    void config32kOSC(void);
    void configureClock(void);
    void RTCreadRequest();
    bool RTCisSyncing(void);
    void RTCdisable();
    void RTCenable();
    void RTCreset();
    void RTCresetRemove();
};

#endif // RTC_ZERO_H
