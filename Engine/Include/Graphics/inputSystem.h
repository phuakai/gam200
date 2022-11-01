#pragma once

#if false
#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vec2D.h>

namespace input
{

	class Input
	{
	public:
		enum class state
		{
			press,
			repeat,
			release
		};

		enum class mouse
		{
			mouse_button_1					= 0,
			mouse_button_2					= 1,
			mouse_button_3					= 2,
			mouse_button_4					= 3,
			mouse_button_5					= 4,
			mouse_button_6					= 5,
			mouse_button_7					= 6,
			mouse_button_8					= 7,
			mouse_button_last				= mouse_button_8,
			mouse_button_left				= mouse_button_1,
			mouse_button_right				= mouse_button_2,
			mouse_button_middle				= mouse_button_3
		};

		enum class key : uint16_t
		{
			key_unknown						= -1,
			key_space						= 32,
			key_apostrophe					= 39,			/* ' */
			key_comma						= 44,			/* , */
			key_minus						= 45,			/* - */
			key_period						= 46,			/* . */
			key_slash						= 47,			/* / */
			key_0							= 48,
			key_1							= 49,
			key_2							= 50,
			key_3							= 51,
			key_4							= 52,
			key_5							= 53,
			key_6							= 54,
			key_7							= 55,
			key_8							= 56,
			key_9							= 57,
			key_semicolon					= 59,			/* ; */
			key_equal						= 61,			/* = */
			key_a							= 65,
			key_b							= 66,
			key_c							= 67,
			key_d							= 68,
			key_e							= 69,
			key_f							= 70,
			key_g							= 71,
			key_h							= 72,
			key_i							= 73,
			key_j							= 74,
			key_k							= 75,
			key_l							= 76,
			key_m							= 77,
			key_n							= 78,
			key_o							= 79,
			key_p							= 80,
			key_q							= 81,
			key_r							= 82,
			key_s							= 83,
			key_t							= 84,
			key_u							= 85,
			key_v							= 86,
			key_w							= 87,
			key_x							= 88,
			key_y							= 89,
			key_z							= 90,
			key_left_bracket				= 91,			/* [ */
			key_backslash					= 92,			/* \ */
			key_right_bracket				= 93,			/* ] */
			key_grave_accent				= 96,			/* ` */
			key_world_1						= 161,			/* non-US #1 */
			key_world_2						= 162,			/* non-US #2 */
			key_escape						= 256,
			key_enter						= 257,
			key_tab							= 258,
			key_backspace					= 259,
			key_insert						= 260,
			key_delete						= 261,
			key_right						= 262,
			key_left						= 263,
			key_down						= 264,
			key_up							= 265,
			key_page_up						= 266,
			key_page_down					= 267,
			key_home						= 268,
			key_end							= 269,
			key_caps_lock					= 280,
			key_scroll_lock					= 281,
			key_num_lock					= 282,
			key_print_screen				= 283,
			key_pause						= 284,
			key_F1							= 290,
			key_F2							= 291,
			key_F3							= 292,
			key_F4							= 293,
			key_F5							= 294,
			key_F6							= 295,
			key_F7							= 296,
			key_F8							= 297,
			key_F9							= 298,
			key_F10							= 299,
			key_F11							= 300,
			key_F12							= 301,
			key_F13							= 302,
			key_F14							= 303,
			key_F15							= 304,
			key_F16							= 305,
			key_F17							= 306,
			key_F18							= 307,
			key_F19							= 308,
			key_F20							= 309,
			key_F21							= 310,
			key_F22							= 311,
			key_F23							= 312,
			key_F24							= 313,
			key_F25							= 314,
			key_keypad_0					= 320,
			key_keypad_1					= 321,
			key_keypad_2					= 322,
			key_keypad_3					= 323,
			key_keypad_4					= 324,
			key_keypad_5					= 325,
			key_keypad_6					= 326,
			key_keypad_7					= 327,
			key_keypad_8					= 328,
			key_keypad_9					= 329,
			key_keypad_decimal				= 330,
			key_keypad_divide				= 331,
			key_keypad_multiply				= 332,
			key_keypad_subtract				= 333,
			key_keypad_add					= 334,
			key_keypad_endter				= 335,
			key_keypad_equal				= 336,
			key_keypad_left_shift			= 340,
			key_keypad_left_control			= 341,
			key_keypad_left_alt				= 342,
			key_keypad_left_super			= 343,
			key_keypad_right_shift			= 344,
			key_keypad_right_control		= 345,
			key_keypad_right_alt			= 346,
			key_keypad_right_super			= 347,
			key_menu						= 348,
			key_last						= key_menu
		};

		static void				processInput(GLFWwindow* window);
		bool					isMouseTriggered(mouse mouse);
		bool					isKeyDown(key key);
	private:
		vector2D::vec2D			mousePos;
		vector2D::vec2D			mouseScroll;
	};

} // input namespace

#endif // INPUTSYSTEM_H

#endif