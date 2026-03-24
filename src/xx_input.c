#include "xx_input.h"
#include "raylib.h"
#include "xx_types.h"


#define KB_ASSERT_EQ(a,b,msg) _Static_assert((int)(a) == (int)(b), msg)

/*
KB_ASSERT_EQ(KC_TAB,       KEY_TAB,       "KC_TAB mismatch");
KB_ASSERT_EQ(KC_BACKSPACE, KEY_BACKSPACE, "KC_BACKSPACE mismatch");
KB_ASSERT_EQ(KC_RIGHT,     KEY_RIGHT,     "KC_RIGHT mismatch");
KB_ASSERT_EQ(KC_LEFT,      KEY_LEFT,      "KC_LEFT mismatch");
KB_ASSERT_EQ(KC_DOWN,      KEY_DOWN,      "KC_DOWN mismatch");
KB_ASSERT_EQ(KC_UP,        KEY_UP,        "KC_UP mismatch");
*/

bool IsKBPressed(int key)
{
	return IsKeyPressed(key);
}
bool IsKBDown(int key)
{
	return IsKeyDown(key);
}
