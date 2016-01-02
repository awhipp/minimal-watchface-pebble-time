#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer_hour;
static TextLayer *s_time_layer_minute;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;

#define KEY_COUNTRY     0

// Write the current hours and minutes into a buffer
static char s_buffer_hour[] = "0000";
static char s_buffer_min[] = "0000";
static char s_date_buffer[] = "XXXX XX";
static char s_day_buffer[] = "XXX";

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *country_t = dict_find(iter, KEY_COUNTRY);
  if(country_t  && country_t->value->int8 == 0) {
    window_set_background_color(s_main_window, GColorWhite);
  } else {
    window_set_background_color(s_main_window, GColorBlack);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

  // Display this time on the TextLayer
  strftime(s_buffer_hour, sizeof(s_buffer_hour), clock_is_24h_style() ? "%H" : "%I", tick_time);
  
  char *s;

s = s_buffer_hour;
  if(!clock_is_24h_style()){
while (*s && *s == '0') s++; /* find the first non '0' element */
printf("%s\n",s); /* now use s instead of str */
  }
  
  text_layer_set_text(s_time_layer_hour, s);
  
  strftime(s_buffer_min, sizeof(s_buffer_min), clock_is_24h_style() ? "%M" : "%M", tick_time);
  text_layer_set_text(s_time_layer_minute, s_buffer_min);
  
  strftime(s_date_buffer, sizeof(s_date_buffer), "%b %e", tick_time);
  text_layer_set_text(s_date_layer, s_date_buffer);
  
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", tick_time);
  text_layer_set_text(s_day_layer, s_day_buffer);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  
  s_day_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(5, 5), bounds.size.w, 50));
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_background_color(s_day_layer, GColorBlack);
  text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  text_layer_set_text(s_day_layer, "Day");
  
  s_time_layer_hour = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(35, 35), bounds.size.w, 50));
  text_layer_set_text_color(s_time_layer_hour, GColorRed);
  text_layer_set_background_color(s_time_layer_hour, GColorWhite);
  text_layer_set_font(s_time_layer_hour, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(s_time_layer_hour, GTextAlignmentCenter);
  
  s_time_layer_minute = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(95, 90), bounds.size.w, 50));
  text_layer_set_text_color(s_time_layer_minute, GColorBlue);
  text_layer_set_background_color(s_time_layer_minute, GColorWhite);
  text_layer_set_font(s_time_layer_minute, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(s_time_layer_minute, GTextAlignmentCenter);
  
  s_date_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(145, 135), bounds.size.w, 50));
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text(s_date_layer, "Date");  
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  tick_handler(current_time, MINUTE_UNIT);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_hour));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_minute));
}

static void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  // Destroy TextLayer
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_time_layer_hour);
  text_layer_destroy(s_time_layer_minute);
  text_layer_destroy(s_date_layer);
}


static void init() {

  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}