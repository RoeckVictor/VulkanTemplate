#pragma once

// From glfw3.h
#define MFE_KEY_UNKNOWN            -1

/* Printable keys */
#define MFE_KEY_SPACE              32
#define MFE_KEY_APOSTROPHE         39  /* ' */
#define MFE_KEY_COMMA              44  /* , */
#define MFE_KEY_MINUS              45  /* - */
#define MFE_KEY_PERIOD             46  /* . */
#define MFE_KEY_SLASH              47  /* / */
#define MFE_KEY_0                  48
#define MFE_KEY_1                  49
#define MFE_KEY_2                  50
#define MFE_KEY_3                  51
#define MFE_KEY_4                  52
#define MFE_KEY_5                  53
#define MFE_KEY_6                  54
#define MFE_KEY_7                  55
#define MFE_KEY_8                  56
#define MFE_KEY_9                  57
#define MFE_KEY_SEMICOLON          59  /* ; */
#define MFE_KEY_EQUAL              61  /* = */
#define MFE_KEY_A                  65
#define MFE_KEY_B                  66
#define MFE_KEY_C                  67
#define MFE_KEY_D                  68
#define MFE_KEY_E                  69
#define MFE_KEY_F                  70
#define MFE_KEY_G                  71
#define MFE_KEY_H                  72
#define MFE_KEY_I                  73
#define MFE_KEY_J                  74
#define MFE_KEY_K                  75
#define MFE_KEY_L                  76
#define MFE_KEY_M                  77
#define MFE_KEY_N                  78
#define MFE_KEY_O                  79
#define MFE_KEY_P                  80
#define MFE_KEY_Q                  81
#define MFE_KEY_R                  82
#define MFE_KEY_S                  83
#define MFE_KEY_T                  84
#define MFE_KEY_U                  85
#define MFE_KEY_V                  86
#define MFE_KEY_W                  87
#define MFE_KEY_X                  88
#define MFE_KEY_Y                  89
#define MFE_KEY_Z                  90
#define MFE_KEY_LEFT_BRACKET       91  /* [ */
#define MFE_KEY_BACKSLASH          92  /* \ */
#define MFE_KEY_RIGHT_BRACKET      93  /* ] */
#define MFE_KEY_GRAVE_ACCENT       96  /* ` */
#define MFE_KEY_WORLD_1            161 /* non-US #1 */
#define MFE_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define MFE_KEY_ESCAPE             256
#define MFE_KEY_ENTER              257
#define MFE_KEY_TAB                258
#define MFE_KEY_BACKSPACE          259
#define MFE_KEY_INSERT             260
#define MFE_KEY_DELETE             261
#define MFE_KEY_RIGHT              262
#define MFE_KEY_LEFT               263
#define MFE_KEY_DOWN               264
#define MFE_KEY_UP                 265
#define MFE_KEY_PAGE_UP            266
#define MFE_KEY_PAGE_DOWN          267
#define MFE_KEY_HOME               268
#define MFE_KEY_END                269
#define MFE_KEY_CAPS_LOCK          280
#define MFE_KEY_SCROLL_LOCK        281
#define MFE_KEY_NUM_LOCK           282
#define MFE_KEY_PRINT_SCREEN       283
#define MFE_KEY_PAUSE              284
#define MFE_KEY_F1                 290
#define MFE_KEY_F2                 291
#define MFE_KEY_F3                 292
#define MFE_KEY_F4                 293
#define MFE_KEY_F5                 294
#define MFE_KEY_F6                 295
#define MFE_KEY_F7                 296
#define MFE_KEY_F8                 297
#define MFE_KEY_F9                 298
#define MFE_KEY_F10                299
#define MFE_KEY_F11                300
#define MFE_KEY_F12                301
#define MFE_KEY_F13                302
#define MFE_KEY_F14                303
#define MFE_KEY_F15                304
#define MFE_KEY_F16                305
#define MFE_KEY_F17                306
#define MFE_KEY_F18                307
#define MFE_KEY_F19                308
#define MFE_KEY_F20                309
#define MFE_KEY_F21                310
#define MFE_KEY_F22                311
#define MFE_KEY_F23                312
#define MFE_KEY_F24                313
#define MFE_KEY_F25                314
#define MFE_KEY_KP_0               320
#define MFE_KEY_KP_1               321
#define MFE_KEY_KP_2               322
#define MFE_KEY_KP_3               323
#define MFE_KEY_KP_4               324
#define MFE_KEY_KP_5               325
#define MFE_KEY_KP_6               326
#define MFE_KEY_KP_7               327
#define MFE_KEY_KP_8               328
#define MFE_KEY_KP_9               329
#define MFE_KEY_KP_DECIMAL         330
#define MFE_KEY_KP_DIVIDE          331
#define MFE_KEY_KP_MULTIPLY        332
#define MFE_KEY_KP_SUBTRACT        333
#define MFE_KEY_KP_ADD             334
#define MFE_KEY_KP_ENTER           335
#define MFE_KEY_KP_EQUAL           336
#define MFE_KEY_LEFT_SHIFT         340
#define MFE_KEY_LEFT_CONTROL       341
#define MFE_KEY_LEFT_ALT           342
#define MFE_KEY_LEFT_SUPER         343
#define MFE_KEY_RIGHT_SHIFT        344
#define MFE_KEY_RIGHT_CONTROL      345
#define MFE_KEY_RIGHT_ALT          346
#define MFE_KEY_RIGHT_SUPER        347
#define MFE_KEY_MENU               348

#define MFE_KEY_LAST               MFE_KEY_MENU

// Mouse buttons
#define MFE_MOUSE_BUTTON_1         0
#define MFE_MOUSE_BUTTON_2         1
#define MFE_MOUSE_BUTTON_3         2
#define MFE_MOUSE_BUTTON_4         3
#define MFE_MOUSE_BUTTON_5         4
#define MFE_MOUSE_BUTTON_6         5
#define MFE_MOUSE_BUTTON_7         6
#define MFE_MOUSE_BUTTON_8         7
#define MFE_MOUSE_BUTTON_LAST      MFE_MOUSE_BUTTON_8
#define MFE_MOUSE_BUTTON_LEFT      MFE_MOUSE_BUTTON_1
#define MFE_MOUSE_BUTTON_RIGHT     MFE_MOUSE_BUTTON_2
#define MFE_MOUSE_BUTTON_MIDDLE    MFE_MOUSE_BUTTON_3

// Joysticks
#define MFE_JOYSTICK_1             0
#define MFE_JOYSTICK_2             1
#define MFE_JOYSTICK_3             2
#define MFE_JOYSTICK_4             3
#define MFE_JOYSTICK_5             4
#define MFE_JOYSTICK_6             5
#define MFE_JOYSTICK_7             6
#define MFE_JOYSTICK_8             7
#define MFE_JOYSTICK_9             8
#define MFE_JOYSTICK_10            9
#define MFE_JOYSTICK_11            10
#define MFE_JOYSTICK_12            11
#define MFE_JOYSTICK_13            12
#define MFE_JOYSTICK_14            13
#define MFE_JOYSTICK_15            14
#define MFE_JOYSTICK_16            15
#define MFE_JOYSTICK_LAST          MFE_JOYSTICK_16

// Gamepad
#define MFE_GAMEPAD_BUTTON_A               0
#define MFE_GAMEPAD_BUTTON_B               1
#define MFE_GAMEPAD_BUTTON_X               2
#define MFE_GAMEPAD_BUTTON_Y               3
#define MFE_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define MFE_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define MFE_GAMEPAD_BUTTON_BACK            6
#define MFE_GAMEPAD_BUTTON_START           7
#define MFE_GAMEPAD_BUTTON_GUIDE           8
#define MFE_GAMEPAD_BUTTON_LEFT_THUMB      9
#define MFE_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define MFE_GAMEPAD_BUTTON_DPAD_UP         11
#define MFE_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define MFE_GAMEPAD_BUTTON_DPAD_DOWN       13
#define MFE_GAMEPAD_BUTTON_DPAD_LEFT       14
#define MFE_GAMEPAD_BUTTON_LAST            MFE_GAMEPAD_BUTTON_DPAD_LEFT

#define MFE_GAMEPAD_BUTTON_CROSS       MFE_GAMEPAD_BUTTON_A
#define MFE_GAMEPAD_BUTTON_CIRCLE      MFE_GAMEPAD_BUTTON_B
#define MFE_GAMEPAD_BUTTON_SQUARE      MFE_GAMEPAD_BUTTON_X
#define MFE_GAMEPAD_BUTTON_TRIANGLE    MFE_GAMEPAD_BUTTON_Y

#define MFE_GAMEPAD_AXIS_LEFT_X        0
#define MFE_GAMEPAD_AXIS_LEFT_Y        1
#define MFE_GAMEPAD_AXIS_RIGHT_X       2
#define MFE_GAMEPAD_AXIS_RIGHT_Y       3
#define MFE_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define MFE_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define MFE_GAMEPAD_AXIS_LAST          MFE_GAMEPAD_AXIS_RIGHT_TRIGGER