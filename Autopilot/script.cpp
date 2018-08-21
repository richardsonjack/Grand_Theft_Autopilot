/*
	Author: Anh-Dzung Doan
*/

#include "script.h"
#include "keyboard.h"
#include "../Vehicle/Vehicle.h"

/*********************************************************************/
/******************************** VARIABLES **************************/
/*********************************************************************/

// notification text
std::string _notification_text;
DWORD _notification_text_draw_ticks_max;
bool _notification_text_gxt_entry;

// Variables for processing menu
float _MENU_LINE_WIDTH = 350; // menu width size
int _current_main_menu_index = 0; // current selection index for main menu
int _current_create_trajectory_menu_index = 0; // current selection index for menu of trajectory creator
int _current_execute_trajectory_menu_index = 0; // current selection index for menu of trajectory executor

int _traj_idx = 0; // trajectory index used while executing trajectory
Cam _camera; // Our owned camera used for executing dense trajectory and collecting image data
int _order_rot = 2; // rotation order

GDIScreenCaptureWorker _screen_capture_worker; // variable for screen capture


/****************************************************************************************/
/******************************** IMPLEMENTATIONS OF FUNCTIONS **************************/
/****************************************************************************************/


void drawRect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7)
{
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}

void drawMenuLine(std::string caption, float line_width, float line_height, float line_top,
	float line_left, float text_left, bool active, bool title, bool rescale_text /*= true*/)
{
	// default values
	int text_col[4] = { 255, 255, 255, 255 },
		rect_col[4] = { 70, 95, 95, 255 };
	float text_scale = 0.35;
	int font = 0;

	// correcting values for active line
	if (active)
	{
		text_col[0] = 0;
		text_col[1] = 0;
		text_col[2] = 0;

		rect_col[0] = 218;
		rect_col[1] = 242;
		rect_col[2] = 216;

		if (rescale_text) text_scale = 0.40;
	}

	if (title)
	{
		rect_col[0] = 0;
		rect_col[1] = 0;
		rect_col[2] = 0;

		if (rescale_text) text_scale = 0.50;
		font = 1;
	}

	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	text_left += line_left;

	float line_width_scaled = line_width / (float)screen_w; // line width
	float line_top_scaled = line_top / (float)screen_h; // line top offset
	float text_left_scaled = text_left / (float)screen_w; // text left offset
	float line_height_scaled = line_height / (float)screen_h; // line height

	float line_left_scaled = line_left / (float)screen_w;

	// this is how it's done in original scripts

	// text upper part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	UI::_DRAW_TEXT(text_left_scaled, (((line_top_scaled + 0.00278f) + line_height_scaled) - 0.005f));

	// text lower part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_GXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	int num25 = UI::_0x9040DFB09BE75706(text_left_scaled, (((line_top_scaled + 0.00278f) + line_height_scaled) - 0.005f));

	// rect
	drawRect(line_left_scaled, line_top_scaled + (0.00278f),
		line_width_scaled, ((((float)(num25)* UI::_0xDB88A37483346780(text_scale, 0)) + (line_height_scaled * 2.0f)) + 0.005f),
		rect_col[0], rect_col[1], rect_col[2], rect_col[3]);
}

bool switchPressed()
{
	return IsKeyJustUp(VK_F6);
}

void getButtonState(bool *select, bool *back, bool *up, bool *down)
{
	if (select) *select = IsKeyDown(VK_NUMPAD5);
	if (back) *back = IsKeyDown(VK_NUMPAD0) || switchPressed() || IsKeyDown(VK_BACK);
	if (up) *up = IsKeyDown(VK_NUMPAD8);
	if (down) *down = IsKeyDown(VK_NUMPAD2);
}

void handleMainMenu()
{
	const int menu_item_number = 1;
	std::string menu_name = "Auto Pilot";
	std::string menu_list[menu_item_number] = { "Drive To Marker"};

	DWORD wait_time = 150;
	
	while (true)
	{
		// timed menu draw, used for pause after active line switch
		DWORD max_tick_count = GetTickCount() + wait_time;
		do
		{
			// draw menu
			drawMenuLine(menu_name, _MENU_LINE_WIDTH, 15.0, 18.0, 0.0, 5.0, false, true);
			for (int i = 0; i < menu_item_number; i++)
				if (i != _current_main_menu_index)
					drawMenuLine(menu_list[i], _MENU_LINE_WIDTH, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
			drawMenuLine(menu_list[_current_main_menu_index],
				_MENU_LINE_WIDTH + 1.0, 11.0, 56.0 + _current_main_menu_index * 36.0, 0.0, 7.0, true, false);
			WAIT(0);
		} while (GetTickCount() < max_tick_count);

		wait_time = 0;

		// listen if users press any buttons
		bool button_select, button_back, button_up, button_down;
		getButtonState(&button_select, &button_back, &button_up, &button_down);

		if (button_select) // if users select one item within menu, check what the item is
		{
			switch (_current_main_menu_index)
			{
			case 0:
				//do the path finding

				break;
			}
			wait_time = 200;
		}
		else {
			if (button_back || switchPressed()) // if users press back
			{
				break;
			}
			else {
				if (button_up) // if users press up/down
				{

					if (_current_main_menu_index == 0)
						_current_main_menu_index = menu_item_number;
					_current_main_menu_index--;
					wait_time = 150;
				}
				else {
					if (button_down)
					{

						_current_main_menu_index++;
						if (_current_main_menu_index == menu_item_number)
							_current_main_menu_index = 0;
						wait_time = 150;
					}
				}
			}
		}
	}
}

void updateNotificationText()
{
	if (GetTickCount() < _notification_text_draw_ticks_max)
	{
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(0.55, 0.55);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
		if (_notification_text_gxt_entry)
		{
			UI::_SET_TEXT_ENTRY((char *)_notification_text.c_str());
		}
		else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char *)_notification_text.c_str());
		}
		UI::_DRAW_TEXT(0.5, 0.5);
	}
}


bool getCoordsFromMarker(Vector3 &coords)
{
	Entity e = PLAYER::PLAYER_PED_ID();
	bool success = false;
	bool blipFound = false;

	// search for marker blip
	int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
	for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
	{
		if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4) // number 4 is the ID of marker on the built-in map
		{
			coords = UI::GET_BLIP_INFO_ID_COORD(i);
			success = true;;
			break;
		}
	}

	return success;
}

void ScriptMain()
{
	while (true)
	{
		if (switchPressed())
		{
			handleMainMenu();
		}
	}
}
