#ifndef __CHRONO_H__
#define __CHRONO_H__


#if 1
#   include "systick_delay.h"
#   define delay_init()
#   define delay_us systick_delay_us
#   define delay_ms systick_delay_ms
#   define delay_s systick_delay_s
#else
#   include "core_delay.h"
#   define delay_init CPU_TS_TmrInit
#   define delay_us core_delay_us
#   define delay_ms core_delay_ms
#   define delay_s core_delay_s
#endif

#define delay(value, unit) delay_ ## unit (value)


#endif //__CHRONO_H__
