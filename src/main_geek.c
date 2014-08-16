/*  Copyright (C) 2014 Alexis Rico | XDA | Pebble

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  
*/

#include <pebble.h>

// Create Window, Time and Quote
static Window *s_main_window;
static TextLayer *s_bt_layer, *s_time_layer, *s_battery_layer, *s_quote_layer;
enum {
  KEY_QUOTE = 0,
  KEY_SHOW_QUOTE = 1,
  KEY_SHOW_BT = 2,
  KEY_SHOW_BATTERY = 3,
};

static void update_bt(bool connected) {
  text_layer_set_text(s_bt_layer, connected ? "BT ON" : "BT OFF");
}

static void update_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";
  if (charge_state.is_charging) {
    text_layer_set_text(s_battery_layer, "Charging");
  } else {
    snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
    text_layer_set_text(s_battery_layer, battery_text);
  }
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void update_quote(char *quote) {
    text_layer_set_text(s_quote_layer, quote);
}

void process_tuple(Tuple *t)
{
  //Get key
  int key = t->key;
 
  //Get integer value, if present
  int int_value = t->value->int32;
 
  //Get string value, if present
  char *string_value = t->value->cstring;
 
  //Decide what to do
  switch(key) {
    case KEY_QUOTE:
      update_quote(string_value); //TODO: Persistent
      break;
    case KEY_SHOW_QUOTE:
      if (strcmp(string_value, "false") == 0) {
        update_quote("");
      } else {
        update_quote("WIP Persistent"); //TODO: Check persistent
      }
      break;
    case KEY_SHOW_BT:
      if (strcmp(string_value, "false") == 0) {
        text_layer_set_text(s_bt_layer, "");
      } else {
        update_bt(bluetooth_connection_service_peek());
      }
      break;
    case KEY_SHOW_BATTERY:
      if (strcmp(string_value, "false") == 0) {
        text_layer_set_text(s_battery_layer, "");
      } else {
        update_battery(battery_state_service_peek());
      }
      break;
  }
}

void create_bt_layer(Window *window) {
  // Create time TextLayer
  s_bt_layer = text_layer_create(GRect(7, 0, 137, 50));
  text_layer_set_background_color(s_bt_layer, GColorClear);
  text_layer_set_text_color(s_bt_layer, GColorBlack);
  update_bt(bluetooth_connection_service_peek());

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_bt_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18)); //TODO: Themes
  text_layer_set_text_alignment(s_bt_layer, GTextAlignmentLeft); //TODO: Themes

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bt_layer));
}

void create_battery_layer(Window *window) {
  // Create time TextLayer
  s_battery_layer = text_layer_create(GRect(0, 0, 137, 50));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorBlack);
  update_battery(battery_state_service_peek());

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18)); //TODO: Themes
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight); //TODO: Themes

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
}

void create_time_layer(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 35, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT)); //TODO:Themes
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter); //TODO: Themes

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Make sure the time is displayed from the start
  update_time();
}

void create_quote_layer(Window *window) {
  // Create time TextLayer
  s_quote_layer = text_layer_create(GRect(0, 95, 144, 50));
  text_layer_set_background_color(s_quote_layer, GColorClear);
  text_layer_set_text_color(s_quote_layer, GColorBlack);
  text_layer_set_text(s_quote_layer, "May The Force Be With You");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_quote_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24)); //TODO: Themes
  text_layer_set_text_alignment(s_quote_layer, GTextAlignmentCenter); //TODO: Themes

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_quote_layer));
}

// Load and Unload
static void main_window_load(Window *window) {
  create_bt_layer(window);
  create_time_layer(window);
  create_battery_layer(window);
  create_quote_layer(window);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_bt_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_quote_layer);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  (void) context;
  //Get data
  Tuple *t = dict_read_first(iter);
  while(t != NULL)
  {
    process_tuple(t);
    //Get next
    t = dict_read_next(iter);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (strcmp(text_layer_get_text(s_bt_layer), "") != 0) {
    update_bt(bluetooth_connection_service_peek());
  }
  if (strcmp(text_layer_get_text(s_battery_layer), "") != 0) {
    update_battery(battery_state_service_peek());
  }
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Register services
  bluetooth_connection_service_subscribe(update_bt);
  battery_state_service_subscribe(update_battery);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  app_message_register_inbox_received(in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy services
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  tick_timer_service_unsubscribe();
  // Destroy Window
  window_destroy(s_main_window);
}

// Main void
int main(void) {
  init();
  app_event_loop();
  deinit();
}
