#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "xprintf.h"
	
#define MY_UUID { 0xC2, 0x5A, 0x8D, 0x50, 0x10, 0x5F, 0x45, 0xBF, 0xC9, 0x92, 0xCE, 0xF9, 0x58, 0xAC, 0x93, 0xAD }
PBL_APP_INFO(MY_UUID,
             "M:TG Life", "Rick Schrader",
             1, 1, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_STANDARD_APP);

static Window window;
AppContextRef app;

static TextLayer _countALayer;
static TextLayer _countBLayer;
static TextLayer _timerLayer;
static BmpContainer _background;
//static GFont _bigFont;

//#define CounterFont RESOURCE_ID_FONT_DEJAVU_SANS_BOLD_SUBSET_36
#define InitialCount 20
#define MaxHours 9
	
static int _countA = InitialCount;
static int _countB = InitialCount;
static bool _timerEnabled = false;
static int _timerHours = 0;
static int _timerMinutes = 0;
static int _timerSeconds = 0;

void DisplayCountA() {
	static char countA[] = "20";
	xsprintf(countA, "%d", _countA);
	text_layer_set_text(&_countALayer, countA);
}

void DisplayCountB() {
	static char countB[] = "20";
	xsprintf(countB, "%d", _countB);
	text_layer_set_text(&_countBLayer, countB);
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
  	
	text_layer_set_text(&_timerLayer, timerText);
}

void StopAndResetTimer()
{
	_timerEnabled = false;
	_timerHours = 0;
	_timerMinutes = 0;
	_timerSeconds = 0;
	DisplayTimer();
}

void ResetHandler(ClickRecognizerRef recognizer, Window *window) {
    _countA = InitialCount;
    _countB = InitialCount;
	DisplayCountA();
	DisplayCountB();
	StopAndResetTimer();
}

void IncrementAHandler(ClickRecognizerRef recognizer, Window *window) {
    _countA += 1;
    DisplayCountA();
}

void DecrementAHandler(ClickRecognizerRef recognizer, Window *window) {
    if(_countA > 0)
        _countA -= 1;
    DisplayCountA();
}
void IncrementBHandler(ClickRecognizerRef recognizer, Window *window) {
    _countB += 1;
    DisplayCountB();
}

void DecrementBHandler(ClickRecognizerRef recognizer, Window *window) {
    if(_countB > 0)
        _countB -= 1;
    DisplayCountB();
}

void LongReleaseHandler(ClickRecognizerRef recognizer, Window *window) 
{
	//Sharing this handler for all buttons
	//Without this, the first click after a long press gets lost
}

void TickHandler(AppContextRef ctx, PebbleTickEvent *t) {

	(void)ctx;
	
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

void ToggleTimerHandler(ClickRecognizerRef recognizer, Window *window) {
    _timerEnabled = !_timerEnabled;
}

void ConfigProvider(ClickConfig **config, Window *window) {
    config[BUTTON_ID_UP]->click.handler = (ClickHandler)DecrementAHandler;
    config[BUTTON_ID_UP]->long_click.handler = (ClickHandler)IncrementAHandler;
    config[BUTTON_ID_UP]->long_click.delay_ms = 700;
	config[BUTTON_ID_UP]->long_click.release_handler = (ClickHandler)LongReleaseHandler;
	
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler)DecrementBHandler;
    config[BUTTON_ID_DOWN]->long_click.handler = (ClickHandler)IncrementBHandler;
    config[BUTTON_ID_DOWN]->long_click.delay_ms = 700;
	config[BUTTON_ID_DOWN]->long_click.release_handler = (ClickHandler)LongReleaseHandler;
	
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler)ToggleTimerHandler;
    config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler)ResetHandler;
    config[BUTTON_ID_SELECT]->long_click.delay_ms = 1000;
	config[BUTTON_ID_SELECT]->long_click.release_handler = (ClickHandler)LongReleaseHandler;
	
    (void)window;
}

void InitHandler(AppContextRef ctx) {
    app = ctx;

    window_init(&window, "MTG Life Counter");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);
    window_set_fullscreen(&window, false);

    resource_init_current_app(&APP_RESOURCES);

    // Arrange for user input.
    window_set_click_config_provider(&window, (ClickConfigProvider) ConfigProvider);

    //// Get our fonts
    //_bigFont = fonts_load_custom_font(resource_get_handle(CounterFont));

    // Root layer
    Layer *rootLayer = window_get_root_layer(&window);
    
	// Add background image
    bmp_init_container(RESOURCE_ID_IMAGE_BG, &_background);
    //layer_set_frame(&_background.layer.layer, GRect(0, 0, 144, 140));
    layer_set_frame(&_background.layer.layer, GRect(0, 0, 144, 152));
    layer_add_child(rootLayer, &_background.layer.layer);
    
	text_layer_init(&_countALayer, window.layer.frame);
    text_layer_set_background_color(&_countALayer, GColorClear);
	text_layer_set_font(&_countALayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
    //text_layer_set_font(&_countALayer, _bigFont);
    //Not sure why, but centering would not work with an x origin of 0
    layer_set_frame(&_countALayer.layer, GRect(-5, 10, 154, 60));
  	text_layer_set_text_color(&_countALayer, GColorBlack);
    text_layer_set_text_alignment(&_countALayer, GTextAlignmentCenter);
    layer_add_child(rootLayer, &_countALayer.layer);
	
    text_layer_init(&_countBLayer, window.layer.frame);
    text_layer_set_background_color(&_countBLayer, GColorClear);
    //text_layer_set_font(&_countBLayer, _bigFont);
    text_layer_set_font(&_countBLayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD));
	//Not sure why, but centering would not work with an x origin of 0
    layer_set_frame(&_countBLayer.layer, GRect(-5, 90, 154, 60));
  	text_layer_set_text_color(&_countBLayer, GColorBlack);
    text_layer_set_text_alignment(&_countBLayer, GTextAlignmentCenter);
    layer_add_child(rootLayer, &_countBLayer.layer);

	text_layer_init(&_timerLayer, window.layer.frame);
    text_layer_set_background_color(&_timerLayer, GColorClear);
    text_layer_set_font(&_timerLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	//Not sure why, but centering would not work with an x origin of 0
    layer_set_frame(&_timerLayer.layer, GRect(-5, 59, 154, 26));
  	text_layer_set_text_color(&_timerLayer, GColorWhite);
    text_layer_set_text_alignment(&_timerLayer, GTextAlignmentCenter);
    layer_add_child(rootLayer, &_timerLayer.layer);

    DisplayCountA();
	DisplayCountB();
	DisplayTimer();
}

void DeinitHandler(AppContextRef ctx) {
    bmp_deinit_container(&_background);
	//fonts_unload_custom_font(_bigFont);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &InitHandler,
		.deinit_handler = &DeinitHandler,
		.tick_info = {
			.tick_handler = &TickHandler,
			.tick_units = SECOND_UNIT
		}
	};
		
	app_event_loop(params, &handlers);
}
