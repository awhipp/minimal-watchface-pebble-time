#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer_hour;
static TextLayer *s_time_layer_minute;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Write the current hours and minutes into a buffer
  static char s_buffer_hour[] = "00";
  static char s_buffer_min[] = "00";

  // Display this time on the TextLayer
  strftime(s_buffer_hour, sizeof(s_buffer_hour), clock_is_24h_style() ? "%H" : "%I", tick_time);
  text_layer_set_text(s_time_layer_hour, s_buffer_hour);
  
  strftime(s_buffer_min, sizeof(s_buffer_min), clock_is_24h_style() ? "%M" : "%M", tick_time);
  text_layer_set_text(s_time_layer_minute, s_buffer_min);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer_hour = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(32, 29), bounds.size.w, 50));
  text_layer_set_text_color(s_time_layer_hour, GColorRed);
  text_layer_set_background_color(s_time_layer_hour, GColorWhite);
  text_layer_set_font(s_time_layer_hour, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer_hour, GTextAlignmentCenter);
  
  s_time_layer_minute = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(90, 81), bounds.size.w, 50));
  text_layer_set_text_color(s_time_layer_minute, GColorBlue);
  text_layer_set_background_color(s_time_layer_minute, GColorWhite);
  text_layer_set_font(s_time_layer_minute, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer_minute, GTextAlignmentCenter);
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  tick_handler(current_time, MINUTE_UNIT);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_hour));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer_minute));
}

static void main_window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  // Destroy TextLayer
  text_layer_destroy(s_time_layer_hour);
  text_layer_destroy(s_time_layer_minute);
}


static void init() {

  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
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