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
static BmpContainer _buttonLabels;
static BmpContainer _background;
//static GFont _bigFont;

//#define CounterFont RESOURCE_ID_FONT_DEJAVU_SANS_BOLD_SUBSET_36
#define InitialCount 20
	
static int _countA = InitialCount;
static int _countB = InitialCount;
static bool _timerEnabled = false;
static int _timerHours = 0;
static int _timerMinutes = 0;
static int _timerSeconds = 0;

void itoa2(int num, char* buffer) {
    const char digits[10] = "0123456789";
    if(num > 999) {
        buffer[0] = '9';
        buffer[1] = '9';
        buffer[2] = '9';

        return;
    } else if(num > 99) {
        
        buffer[0] = digits[num / 100];
        if(num%100 > 9) {
            buffer[1] = digits[(num%100) / 10];
        } else {
            buffer[1] = '0';
        }
        buffer[2] = digits[num%10];
    }
    
    else if(num > 9) {
        buffer[0]=' ';
        buffer[1] = digits[num / 10];
        
        buffer[2] = digits[num % 10];
    } else {
        buffer[0] = ' ';

        buffer[1] = ' ';
        
        buffer[2] = digits[num % 10];
    }
}

void DisplayCountA() {
	static char countA[] = "20";
	xsprintf(countA, "%d", _countA);
	//itoa2(_countA, &countA[0]);
	text_layer_set_text(&_countALayer, countA);
}

void DisplayCountB() {
	static char countB[] = "20";
	xsprintf(countB, "%d", _countB);
	//itoa2(_countB, &countB[0]);
	text_layer_set_text(&_countBLayer, countB);
}

void ResetHandler(ClickRecognizerRef recognizer, Window *window) {
    _countA = InitialCount;
    _countB = InitialCount;
	DisplayCountA();
	DisplayCountB();
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
	
	//itoa2(_timerHours, &hoursText[0]);
	//itoa2(_timerMinutes, &minutesText[0]);
	//itoa2(_timerSeconds, &secondsText[0]);
	
	//strcat(words, TENS[tens_val]);
	
	xsprintf(timerText, "%s%s%s%s%s", hoursText, ":", minutesText, ":", secondsText);
  	
	text_layer_set_text(&_timerLayer, timerText);
}

void TickHandler(AppContextRef ctx, PebbleTickEvent *t) {

	(void)ctx;
		
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
		
	DisplayTimer();
	
	
  // Need to be static because they're used by the system later.
  //static char time_text[] = "00:00";
  //static char date_text[] = "Xxxxxxxxx 00";

  //char *time_format;


  // TODO: Only update the date when it's changed.
  //string_format_time(date_text, sizeof(date_text), "%B %e", t->tick_time);
  //text_layer_set_text(&text_date_layer, date_text);


  //if (clock_is_24h_style()) {
  //  time_format = "%R";
  //} else {
  //  time_format = "%I:%M";
 // }

  //string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  //if (!clock_is_24h_style() && (time_text[0] == '0')) {
  //  memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  //}

  //text_layer_set_text(&text_time_layer, time_text);

}

void ConfigProvider(ClickConfig **config, Window *window) {
    config[BUTTON_ID_UP]->click.handler = (ClickHandler)DecrementAHandler;
    config[BUTTON_ID_UP]->long_click.handler = (ClickHandler)IncrementAHandler;
    config[BUTTON_ID_UP]->long_click.delay_ms = 700;
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler)DecrementBHandler;
    config[BUTTON_ID_DOWN]->long_click.handler = (ClickHandler)IncrementBHandler;
    config[BUTTON_ID_DOWN]->long_click.delay_ms = 700;
    config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler)ResetHandler;
    config[BUTTON_ID_SELECT]->long_click.delay_ms = 1000;
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
    text_layer_set_font(&_timerLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	//Not sure why, but centering would not work with an x origin of 0
    layer_set_frame(&_timerLayer.layer, GRect(-5, 59, 154, 26));
  	text_layer_set_text_color(&_timerLayer, GColorWhite);
    text_layer_set_text_alignment(&_timerLayer, GTextAlignmentCenter);
    layer_add_child(rootLayer, &_timerLayer.layer);

    //// Add button labels
    //bmp_init_container(RESOURCE_ID_IMAGE_BUTTON_LABELS, &_buttonLabels);
    //layer_set_frame(&_buttonLabels.layer.layer, GRect(130, 0, 14, 140));
    //layer_add_child(rootLayer, &_buttonLabels.layer.layer);
   	
	DisplayCountA();
	DisplayCountB();
	DisplayTimer();
}

void DeinitHandler(AppContextRef ctx) {
    bmp_deinit_container(&_buttonLabels);
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
