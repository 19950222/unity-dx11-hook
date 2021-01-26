#pragma once

/// wait until STATE is true by sleeping every INTERVAL ms.
#define SleepUntil(STATE, INTERVAL) while (!##STATE##) { Sleep(##INTERVAL##); }