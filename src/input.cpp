/*
retrorun-go2 - libretro frontend for the ODROID-GO Advance
Copyright (C) 2020  OtherCrashOverride
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "input.h"

#include "globals.h"
#include "video.h"
#include "libretro.h"

#include <go2/input.h>
#include <stdio.h>


extern int opt_backlight;
extern int opt_volume;

bool input_exit_requested = false;
bool input_reset_requested = false;
bool input_pause_requested = false;
bool input_ffwd_requested = false;
go2_battery_state_t batteryState;

static go2_input_state_t* gamepadState;
static go2_input_state_t* prevGamepadState;
static go2_input_t* input;
static bool has_triggers = false;

static go2_input_button_t select_button;
static go2_input_button_t start_button;
static go2_input_button_t hotkey_button;

void configure_input_buttons_by_device()
{
	// default OGS, OGA, RGB10_MAX_TOP, RGB10_MAX2_TOP
	select_button = Go2InputButton_F1;
	start_button = Go2InputButton_F2;
	hotkey_button = Go2InputButton_F3;

	if (device == OGA_1)
	{
		start_button = Go2InputButton_F6;
	}
	else if ( (device == RGB10_MAX_NATIVE) || (device == RGB10_MAX2_NATIVE) )
	{
		hotkey_button = Go2InputButton_F1;
		select_button = Go2InputButton_F3;
		start_button = Go2InputButton_F4;
	}
}

void input_gamepad_read()
{
    if (!input)
    {
        input = go2_input_create();

        if (go2_input_features_get(input) & Go2InputFeatureFlags_Triggers)
        {
            has_triggers = true;

            printf("input: Hardware triggers enabled.\n");
        }

        gamepadState = go2_input_state_create();
        prevGamepadState = go2_input_state_create();
    }

    // Swap current/previous state
    go2_input_state_t* tempState = prevGamepadState;
    prevGamepadState = gamepadState;
    gamepadState = tempState;

    go2_input_state_read(input, gamepadState);
}

go2_input_state_t* input_gampad_current_get()
{
    return gamepadState;
}

void core_input_poll(void)
{
    if (!input)
    {
        input = go2_input_create();
    }


    // Read inputs
	input_gamepad_read();
    go2_input_battery_read(input, &batteryState);

    if (go2_input_state_button_get(gamepadState, select_button) == ButtonState_Pressed &&
        go2_input_state_button_get(gamepadState, start_button) == ButtonState_Pressed)
    {
        input_exit_requested = true;
    }

    // if (!prevGamepadState.buttons.f2 && gamepadState.buttons.f2)
    // {
    //     screenshot_requested = true;
    // }

    if (go2_input_state_button_get(gamepadState, hotkey_button) == ButtonState_Pressed)
    {
        if (go2_input_state_button_get(gamepadState, Go2InputButton_TopRight) == ButtonState_Pressed &&
            go2_input_state_button_get(prevGamepadState, Go2InputButton_TopRight) == ButtonState_Released)
        {
            input_ffwd_requested = !input_ffwd_requested;
            printf("Fast-forward %s\n", input_ffwd_requested ? "on" : "off");
        }
        if (go2_input_state_button_get(gamepadState, Go2InputButton_TriggerRight) == ButtonState_Pressed &&
            go2_input_state_button_get(prevGamepadState, Go2InputButton_TriggerRight) == ButtonState_Released)
        {
            input_pause_requested = !input_pause_requested;
            printf("%s\n", input_pause_requested ? "Paused" : "Un-paused");
        }
        /*if (go2_input_state_button_get(gamepadState, Go2InputButton_X) == ButtonState_Pressed &&
            go2_input_state_button_get(prevGamepadState, Go2InputButton_X) == ButtonState_Released)
        {
            input_reset_requested = true;
            printf("Reset requested\n");
        }*/
        if (go2_input_state_button_get(gamepadState, Go2InputButton_Y) == ButtonState_Pressed &&
            go2_input_state_button_get(prevGamepadState, Go2InputButton_Y) == ButtonState_Released)
        {
            screenshot_requested = true;
            printf("Screenshot requested\n");
        }
    }
}

int16_t core_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
    //int16_t result;

    // if (port || index || device != RETRO_DEVICE_JOYPAD)
    //         return 0;

    if (go2_input_state_button_get(gamepadState, hotkey_button) == ButtonState_Pressed)
        return 0;

    if (!Retrorun_UseAnalogStick)
    {
        // Map thumbstick to dpad
        const float TRIM = 0.35f;
        
        go2_thumb_t thumb = go2_input_state_thumbstick_get(gamepadState, Go2InputThumbstick_Left);

        if (thumb.y < -TRIM) go2_input_state_button_set(gamepadState, Go2InputButton_DPadUp, ButtonState_Pressed);
        if (thumb.y > TRIM) go2_input_state_button_set(gamepadState, Go2InputButton_DPadDown, ButtonState_Pressed);
        if (thumb.x < -TRIM) go2_input_state_button_set(gamepadState, Go2InputButton_DPadLeft, ButtonState_Pressed);
        if (thumb.x > TRIM) go2_input_state_button_set(gamepadState, Go2InputButton_DPadRight, ButtonState_Pressed);
    }

/*
#define RETRO_DEVICE_ID_JOYPAD_B        0
#define RETRO_DEVICE_ID_JOYPAD_Y        1
#define RETRO_DEVICE_ID_JOYPAD_SELECT   2
#define RETRO_DEVICE_ID_JOYPAD_START    3
#define RETRO_DEVICE_ID_JOYPAD_UP       4
#define RETRO_DEVICE_ID_JOYPAD_DOWN     5
#define RETRO_DEVICE_ID_JOYPAD_LEFT     6
#define RETRO_DEVICE_ID_JOYPAD_RIGHT    7
#define RETRO_DEVICE_ID_JOYPAD_A        8
#define RETRO_DEVICE_ID_JOYPAD_X        9
#define RETRO_DEVICE_ID_JOYPAD_L       10
#define RETRO_DEVICE_ID_JOYPAD_R       11
#define RETRO_DEVICE_ID_JOYPAD_L2      12
#define RETRO_DEVICE_ID_JOYPAD_R2      13
#define RETRO_DEVICE_ID_JOYPAD_L3      14
#define RETRO_DEVICE_ID_JOYPAD_R3      15
*/

    if (port == 0)
    {
        if (device == RETRO_DEVICE_JOYPAD)
        {
            switch (id)
            {
                case RETRO_DEVICE_ID_JOYPAD_B:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_B);
                    break;
                
                case RETRO_DEVICE_ID_JOYPAD_Y:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_Y);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_SELECT:
                    return go2_input_state_button_get(gamepadState, select_button);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_START:
                    return go2_input_state_button_get(gamepadState, start_button);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_UP:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_DPadUp);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_DOWN:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_DPadDown);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_LEFT:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_DPadLeft);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_RIGHT:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_DPadRight);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_A:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_A);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_X:
                    return go2_input_state_button_get(gamepadState, Go2InputButton_X);
                    break;

                case RETRO_DEVICE_ID_JOYPAD_L:
                    if (has_triggers)
                    {
                        return opt_triggers_left ? go2_input_state_button_get(gamepadState, Go2InputButton_TriggerLeft) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TopLeft);                    
		    }
                    else
                    {
                        return opt_triggers_left ? go2_input_state_button_get(gamepadState, Go2InputButton_TriggerLeft) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TopLeft);
                    }
                    break;

                case RETRO_DEVICE_ID_JOYPAD_R:
                    if (has_triggers)
                    {
                        return opt_triggers_right ? go2_input_state_button_get(gamepadState, Go2InputButton_TriggerRight) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TopRight);
                    }
                    else
                    {
                        return opt_triggers_right ? go2_input_state_button_get(gamepadState, Go2InputButton_TriggerRight) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TopRight);
                    }
                    break;

                case RETRO_DEVICE_ID_JOYPAD_L2:
                    if (has_triggers)
                    {
		        return opt_triggers_left ? go2_input_state_button_get(gamepadState, Go2InputButton_TopLeft) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TriggerLeft);			    
                    }
                    else
                    {
                        return opt_triggers_left ? go2_input_state_button_get(gamepadState, Go2InputButton_TopLeft) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TriggerLeft);
                    }
                    break;

                case RETRO_DEVICE_ID_JOYPAD_R2:
                    if (has_triggers)
                    {
			return opt_triggers_right ? go2_input_state_button_get(gamepadState, Go2InputButton_TopRight) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TriggerRight);                    }
                    else
                    {
                        return opt_triggers_right ? go2_input_state_button_get(gamepadState, Go2InputButton_TopRight) :
                            go2_input_state_button_get(gamepadState, Go2InputButton_TriggerRight);
                    }
                    break;

                default:
                    return 0;
                    break;
            }
        }
        else if (Retrorun_UseAnalogStick && device == RETRO_DEVICE_ANALOG && index == RETRO_DEVICE_INDEX_ANALOG_LEFT)
        {
            go2_thumb_t thumb = go2_input_state_thumbstick_get(gamepadState, Go2InputThumbstick_Left);

            if (thumb.x > 1.0f)
                thumb.x = 1.0f;
            else if (thumb.x < -1.0f)
                thumb.x = -1.0f;
            
            if (thumb.y > 1.0f)
                thumb.y = 1.0f;
            else if (thumb.y < -1.0f)
                thumb.y = -1.0f;

            switch (id)
            {
                case RETRO_DEVICE_ID_ANALOG_X:
                    return thumb.x * 0x7fff;
                    break;
                
                case RETRO_DEVICE_ID_JOYPAD_Y:
                    return thumb.y * 0x7fff;
                    break;
                    
                default:
                    return 0;
                    break;
            }
        }
        
    }

    return 0;
}
