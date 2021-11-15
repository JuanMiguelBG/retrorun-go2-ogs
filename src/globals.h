#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

enum RETRORUN_CORE_TYPE
{
		RETRORUN_CORE_UNKNOWN = 0,
		RETRORUN_CORE_ATARI800,
		RETRORUN_CORE_MGBA
};

enum DEVICE_TYPE
{
		OGS = 0,
		OGA = 1,
		OGA_1 = 2,
		RGB10_MAX_TOP = 3,
		RGB10_MAX_NATIVE = 4,
		RGB10_MAX2_TOP = 5,
		RGB10_MAX2_NATIVE = 6
};

extern RETRORUN_CORE_TYPE Retrorun_Core;
extern bool Retrorun_UseAnalogStick;
extern bool opt_triggers_right;
extern bool opt_triggers_left;
extern DEVICE_TYPE device;

#endif // GLOBALS_H
