#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
	
#define MY_UUID { 0xC2, 0x5A, 0x8D, 0x50, 0x10, 0x5F, 0x45, 0xBF, 0xC9, 0x92, 0xCE, 0xF9, 0x58, 0xAC, 0x93, 0xAD }
PBL_APP_INFO(MY_UUID,
             "M:TG Life", "Rick Schrader",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

static Window window;
AppContextRef app;

static TextLayer _countALayer;
static TextLayer _countBLayer;
static BmpContainer _buttonLabels;
static BmpContainer _background;

#define CounterFont RESOURCE_ID_FONT_DEJAVU_SANS_BOLD_SUBSET_36
#define InitialCount 20
	
static int _countA = InitialCount;
static int _countB = InitialCount;

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
	itoa2(_countA, &countA[0]);
	text_layer_set_text(&_countALayer, countA);
}

void DisplayCountB() {
	static char countB[] = "20";
	itoa2(_countB, &countB[0]);
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

    // Get our fonts
    GFont big_font = fonts_load_custom_font(resource_get_handle(CounterFont));

    // Root layer
    Layer *rootLayer = window_get_root_layer(&window);
    
	// Add background image
    //bmp_init_container(RESOURCE_ID_IMAGE_BG, &Background);
    //layer_set_frame(&Background.layer.layer, GRect(0, 0, 144, 140));
    //layer_add_child(root_layer, &Background.layer.layer);
    
	text_layer_init(&_countALayer, window.layer.frame);
    text_layer_set_background_color(&_countALayer, GColorBlack);
    text_layer_set_font(&_countALayer, big_font);
    layer_set_frame(&_countALayer.layer, GRect(0, 20, 144, 40));
  	text_layer_set_text_color(&_countALayer, GColorWhite);
    text_layer_set_text_alignment(&_countALayer, GTextAlignmentCenter);
    layer_add_child(rootLayer, &_countALayer.layer);
	
    text_layer_init(&_countBLayer, window.layer.frame);
    text_layer_set_background_color(&_countBLayer, GColorBlack);
    text_layer_set_font(&_countBLayer, big_font);
    layer_set_frame(&_countBLayer.layer, GRect(0, 82, 144, 40));
  	text_layer_set_text_color(&_countBLayer, GColorWhite);
    text_layer_set_text_alignment(&_countBLayer, GTextAlignmentCenter);
    layer_add_child(rootLayer, &_countBLayer.layer);

    // Add button labels
    bmp_init_container(RESOURCE_ID_IMAGE_BUTTON_LABELS, &_buttonLabels);
    layer_set_frame(&_buttonLabels.layer.layer, GRect(130, 0, 14, 140));
    layer_add_child(rootLayer, &_buttonLabels.layer.layer);
   	
	DisplayCountA();
	DisplayCountB();
}

void DeinitHandler(AppContextRef ctx) {
    bmp_deinit_container(&_buttonLabels);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &InitHandler,
		.deinit_handler = &DeinitHandler
	};
		
	app_event_loop(params, &handlers);
}
