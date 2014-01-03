#include <pebble.h>
#include "xprintf.h"
	
static Window *_window;

static TextLayer *_countALayer;
static TextLayer *_countBLayer;
static TextLayer *_timerLayer;
static GBitmap *_image;
static BitmapLayer *_background;

//Allows cancelling the timer using app_timer_cancel
AppTimer *_longPressUpTimerHandle;
AppTimer *_longPressDownTimerHandle;

#define LongPressRepeatMilliseconds 500
#define LongClickDelayMilliseconds 700
#define InitialCount 20
#define MaxHours 9
#define VibesEnabled true
#define FirstVibeMinute 45
#define LastVibeMinute 50
	
static int _countA = InitialCount;
static int _countB = InitialCount;
static bool _timerEnabled = false;
static int _timerHours = 0;
static int _timerMinutes = 0;
static int _timerSeconds = 0;


void DisplayCountA() {
	static char countA[] = "20";
	xsprintf(countA, "%d", _countA);
	text_layer_set_text(_countALayer, countA);
}

void DisplayCountB() {
	static char countB[] = "20";
	xsprintf(countB, "%d", _countB);
	text_layer_set_text(_countBLayer, countB);
}

void DisplayTimer()
{
	static char timerText[] = "0:00:00";
	static char hoursText[] = "00";
	static char minutesText[] = "00";
	static char secondsText[] = "00";
		
	xsprintf(hoursText, "%d", _timerHours);
	
	if(_timerMinutes < 10)
		xsprintf(minutesText, "%s%d", "0", _timerMinutes);
	else
		xsprintf(minutesText, "%d", _timerMinutes);				
	
	if(_timerSeconds < 10)
		xsprintf(secondsText, "%s%d", "0", _timerSeconds);
	else
		xsprintf(secondsText, "%d", _timerSeconds);
	
	//strcat(words, TENS[tens_val]);
	
	xsprintf(timerText, "%s%s%s%s%s", hoursText, ":", minutesText, ":", secondsText);
  	
	text_layer_set_text(_timerLayer, timerText);
}

void StopAndResetTimer()
{
	_timerEnabled = false;
	_timerHours = 0;
	_timerMinutes = 0;
	_timerSeconds = 0;
	DisplayTimer();
}

void ResetHandler() {
    _countA = InitialCount;
    _countB = InitialCount;
	DisplayCountA();
	DisplayCountB();
}

void IncrementAndDisplayA()
{
    _countA += 1;
    DisplayCountA();
}

void LongPressUpTimerHandler(void *data) 
{
	//increment A & schedule next timer
	IncrementAndDisplayA();
	_longPressUpTimerHandle = app_timer_register(LongPressRepeatMilliseconds, LongPressUpTimerHandler, NULL);
}

void IncrementAHandler() {
	//set the first timer
	_longPressUpTimerHandle = app_timer_register(LongPressRepeatMilliseconds, LongPressUpTimerHandler, NULL);
	
	IncrementAndDisplayA();
}

void DecrementAHandler() {
    if(_countA > 0)
        _countA -= 1;
    DisplayCountA();
}

void IncrementAndDisplayB()
{
    _countB += 1;
    DisplayCountB();
}

void LongPressDownTimerHandler(void *data) 
{
	//increment B & schedule next timer
	IncrementAndDisplayB();
	_longPressDownTimerHandle = app_timer_register(LongPressRepeatMilliseconds, LongPressDownTimerHandler, NULL);
}

void IncrementBHandler() {
	//set the first timer
	_longPressDownTimerHandle = app_timer_register(LongPressRepeatMilliseconds, LongPressDownTimerHandler, NULL);
		
	IncrementAndDisplayB();
}

void DecrementBHandler() {
    if(_countB > 0)
        _countB -= 1;
    DisplayCountB();
}

void LongReleaseUpHandler() 
{
	//Cancel the current timer
	app_timer_cancel(_longPressUpTimerHandle);
}

void LongReleaseDownHandler() 
{
	//Cancel the current timer
	app_timer_cancel(_longPressDownTimerHandle);
}


void SecondsTickHandler(struct tm *tick_time, TimeUnits units_changed) {

	if(_timerEnabled)
	{
		if(_timerSeconds+1 >= 60)
		{
			_timerSeconds = 0;
			
			if(_timerMinutes+1 >= 60)
			{			
				_timerMinutes = 0;
				_timerHours += 1;
			}
			else
			{
				_timerMinutes += 1;
				
				//Only cause vibes during the first hour
				if(VibesEnabled && _timerHours == 0)
				{
					if(_timerMinutes == FirstVibeMinute)
						vibes_long_pulse();
					else if(_timerMinutes > FirstVibeMinute && _timerMinutes < LastVibeMinute)
						vibes_short_pulse();
					else if(_timerMinutes == LastVibeMinute)
					{
						// Vibe pattern: three 200ms vibes with a 100ms pause between them
						static const uint32_t const segments[] = { 200, 100, 200, 100, 200 };
						VibePattern pat = {
						  .durations = segments,
						  .num_segments = ARRAY_LENGTH(segments),
						};
						vibes_enqueue_custom_pattern(pat);
					}
				}
			}
		}
		else	
		{	
			_timerSeconds += 1;
		}
		
		//If we pass the max # hours/minutes/seconds, stop and reset the timer (so we don't overflow the buffer) 
		if(_timerHours > MaxHours)
			StopAndResetTimer();
		else
			DisplayTimer();	
	}
}

void ToggleTimerHandler() {
   
	if(_timerEnabled)
		StopAndResetTimer();
	else
		_timerEnabled = true;
}

/*
void ConfigProvider(ClickConfig **config, Window *window) {
    config[BUTTON_ID_UP]->click.handler = (ClickHandler)DecrementAHandler;
    config[BUTTON_ID_UP]->long_click.handler = (ClickHandler)IncrementAHandler;
    config[BUTTON_ID_UP]->long_click.delay_ms = LongClickDelayMilliseconds;
	config[BUTTON_ID_UP]->long_click.release_handler = (ClickHandler)LongReleaseUpHandler;
	
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler)DecrementBHandler;
    config[BUTTON_ID_DOWN]->long_click.handler = (ClickHandler)IncrementBHandler;
    config[BUTTON_ID_DOWN]->long_click.delay_ms = LongClickDelayMilliseconds;
	config[BUTTON_ID_DOWN]->long_click.release_handler = (ClickHandler)LongReleaseDownHandler;
}
*/

void clickConfigProvider(void *context) {
    window_set_click_context(BUTTON_ID_UP, context);
	
	//Up button single click
	window_single_click_subscribe(BUTTON_ID_UP, DecrementAHandler);
	//Up button long press
	window_long_click_subscribe(BUTTON_ID_UP, LongClickDelayMilliseconds, IncrementAHandler, LongReleaseUpHandler);

	//Down button single click
	window_single_click_subscribe(BUTTON_ID_DOWN, DecrementBHandler);
	//Down button long press
	window_long_click_subscribe(BUTTON_ID_DOWN, LongClickDelayMilliseconds, IncrementBHandler, LongReleaseDownHandler);

	//Select button single click
	window_single_click_subscribe(BUTTON_ID_SELECT, ToggleTimerHandler);
	//Select button long click
    window_long_click_subscribe(BUTTON_ID_SELECT, LongClickDelayMilliseconds, ResetHandler, NULL /* Button release handler */);

}

void init() {
    
	_window = window_create();
	window_stack_push(_window, true /* Animated */);
    window_set_background_color(_window, GColorBlack);
	Layer *windowLayer = window_get_root_layer(_window);
	GRect bounds = layer_get_frame(windowLayer);
	
	//XXwindow_init(&window, "MTG Life Counter");
    //XXwindow_set_fullscreen(&window, false);
	
	// Add background image
  	_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
  	_background = bitmap_layer_create(bounds);
  	bitmap_layer_set_bitmap(_background, _image);
  	bitmap_layer_set_alignment(_background, GAlignCenter);
  	layer_add_child(windowLayer, bitmap_layer_get_layer(_background));

	//Setup top count label
	//Not sure why, but centering would not work with an x origin of 0
	_countALayer = text_layer_create((GRect){ .origin = { -5, 10 }, .size = { 154 , 60 } });
	text_layer_set_background_color(_countALayer, GColorClear);
	text_layer_set_font(_countALayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_color(_countALayer, GColorBlack);
    text_layer_set_text_alignment(_countALayer, GTextAlignmentCenter);
	layer_add_child(windowLayer, text_layer_get_layer(_countALayer));
	
	//Setup bottom count label
	//Not sure why, but centering would not work with an x origin of 0
	_countBLayer = text_layer_create((GRect){ .origin = { -5, 90 }, .size = { 154 , 60 } });
  	text_layer_set_background_color(_countBLayer, GColorClear);
	text_layer_set_font(_countBLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_color(_countBLayer, GColorBlack);
    text_layer_set_text_alignment(_countBLayer, GTextAlignmentCenter);
	layer_add_child(windowLayer, text_layer_get_layer(_countBLayer));
		
	//Setup bottom count label
	//Not sure why, but centering would not work with an x origin of 0
	_timerLayer = text_layer_create((GRect){ .origin = { -5, 59 }, .size = { 154 , 26 } });
  	text_layer_set_background_color(_timerLayer, GColorClear);
	text_layer_set_font(_timerLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_color(_timerLayer, GColorWhite);
    text_layer_set_text_alignment(_timerLayer, GTextAlignmentCenter);
	layer_add_child(windowLayer, text_layer_get_layer(_timerLayer));

	
    DisplayCountA();
	DisplayCountB();
	DisplayTimer();
	
	//Subscribe to tick events
	tick_timer_service_subscribe(SECOND_UNIT, SecondsTickHandler);
	
	// Arrange for user input
    window_set_click_config_provider(_window, (ClickConfigProvider) clickConfigProvider);
    
}

void deinit() {
	tick_timer_service_unsubscribe();
	
	text_layer_destroy(_countALayer);
	text_layer_destroy(_countBLayer);
	text_layer_destroy(_timerLayer);
	
 	gbitmap_destroy(_image);
  	bitmap_layer_destroy(_background);

	window_destroy(_window);
}

int main(void) {
	
	init();
	
	/*PebbleAppHandlers handlers = {
		.tick_info = {
			.tick_handler = &TickHandler,
			.tick_units = SECOND_UNIT
		},
		.timer_handler = &TimerHandler
	};
	*/
	
	app_event_loop();
	
	deinit();
}
