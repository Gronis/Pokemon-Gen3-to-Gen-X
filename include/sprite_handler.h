#ifndef SPRITE_HANDLER__
#define SPRITE_HANDLER__

#include "print_system.h"
#include "party_handler.h"
#include "config_settings.h"
#include "graphics_handler.h"
#include "window_handler.h"

#define TOP_SCREEN_SPRITE_POS SCREEN_REAL_HEIGHT
#define LEFT_SCREEN_SPRITE_POS SCREEN_REAL_WIDTH

#define BASE_Y_SPRITE_INFO_PAGE 0
#define BASE_X_SPRITE_INFO_PAGE 0

#define JAPANESE_Y_AUTO_INCREASE_SPRITES (JAPANESE_Y_AUTO_INCREASE<<3)

#define BASE_Y_SPRITE_TRADE_ANIMATION_SEND ((SCREEN_HEIGHT>>1)-((POKEMON_SPRITE_Y_TILES<<3)>>1)-JAPANESE_Y_AUTO_INCREASE_SPRITES)
#define BASE_X_SPRITE_TRADE_ANIMATION ((SCREEN_WIDTH>>1)-((POKEMON_SPRITE_X_TILES<<3)>>1))
#define BASE_Y_SPRITE_TRADE_ANIMATION_RECV (TOP_SCREEN_SPRITE_POS-(POKEMON_SPRITE_Y_TILES<<3))
#define BASE_Y_SPRITE_TRADE_ANIMATION_END_RUN_INC (TOP_SCREEN_SPRITE_POS-BASE_Y_SPRITE_TRADE_ANIMATION_RECV)

#define BASE_Y_SPRITE_NATURE_PAGE (TOP_SCREEN_SPRITE_POS-8)
#define BASE_X_SPRITE_NATURE_PAGE 0

#define BASE_Y_SPRITE_IV_FIX_PAGE (TOP_SCREEN_SPRITE_POS-8)
#define BASE_X_SPRITE_IV_FIX_PAGE 0

#define BASE_Y_SPRITE_LEARN_MOVE_PAGE (TOP_SCREEN_SPRITE_POS-8)
#define BASE_X_SPRITE_LEARN_MOVE_PAGE 0

#define BASE_Y_SPRITE_EVOLUTION_PAGE (TOP_SCREEN_SPRITE_POS-8)
#define BASE_X_SPRITE_EVOLUTION_PAGE 0

#define X_OFFSET_TRADE_MENU 0
#define Y_OFFSET_TRADE_MENU -3
#define Y_OFFSET_TRADE_MENU_SPRITES (Y_OFFSET_TRADE_MENU + 2)
#define BASE_Y_SPRITE_TRADE_MENU (0 - Y_OFFSET_TRADE_MENU_SPRITES)
#define BASE_Y_SPRITE_INCREMENT_TRADE_MENU (3*8)
#define BASE_X_SPRITE_TRADE_MENU (8 - X_OFFSET_TRADE_MENU)
#define BASE_X_SPRITE_INCREMENT_TRADE_MENU (SCREEN_HALF_X << 3)
#define BASE_Y_CURSOR_TRADING_MENU ((POKEMON_SPRITE_Y_TILES<<2) + BASE_Y_SPRITE_TRADE_MENU + 1)
#define BASE_X_CURSOR_TRADING_MENU (1 - X_OFFSET_TRADE_MENU)
#define BASE_Y_CURSOR_INCREMENT_TRADING_MENU BASE_Y_SPRITE_INCREMENT_TRADE_MENU
#define BASE_X_CURSOR_INCREMENT_TRADING_MENU BASE_X_SPRITE_INCREMENT_TRADE_MENU
#define CURSOR_Y_POS_CANCEL (SCREEN_HEIGHT-8)
#define CURSOR_X_POS_CANCEL 2

#define BASE_X_CURSOR_MAIN_MENU 2
#define BASE_Y_CURSOR_MAIN_MENU 8
#define BASE_Y_CURSOR_INCREMENT_MAIN_MENU 16

#define BASE_X_CURSOR_BASE_SETTINGS_MENU 2
#define BASE_Y_CURSOR_BASE_SETTINGS_MENU 8
#define BASE_Y_CURSOR_INCREMENT_BASE_SETTINGS_MENU 16

#define BASE_X_CURSOR_GEN12_SETTINGS_MENU 2
#define BASE_Y_CURSOR_GEN12_SETTINGS_MENU 8
#define BASE_Y_CURSOR_INCREMENT_GEN12_SETTINGS_MENU 16

#define BASE_X_CURSOR_CHEATS_MENU 2
#define BASE_Y_CURSOR_CHEATS_MENU 8
#define BASE_Y_CURSOR_INCREMENT_CHEATS_MENU 16

#define BASE_X_CURSOR_CLOCK_SETTINGS_MENU 2
#define BASE_Y_CURSOR_CLOCK_SETTINGS_MENU 8
#define BASE_Y_CURSOR_INCREMENT_CLOCK_SETTINGS_MENU 8

#define BASE_X_CURSOR_CLOCK_WARNING 2
#define BASE_Y_CURSOR_CLOCK_WARNING 64
#define BASE_X_CURSOR_INCREMENT_CLOCK_WARNING (SCREEN_WIDTH>>1)

#define BASE_X_CURSOR_EVOLUTIONS_MENU (2+(EVOLUTION_WINDOW_X<<3))
#define BASE_Y_CURSOR_EVOLUTIONS_MENU (EVOLUTION_WINDOW_Y<<3)
#define BASE_Y_CURSOR_INCREMENT_EVOLUTIONS_MENU (EVOLUTION_WINDOW_Y_SIZE_INCREMENT<<3)

#define BASE_X_CURSOR_COLOURS_SETTINGS_MENU 2
#define BASE_X_CURSOR_COLOURS_SETTINGS_MENU_IN (2+112)
#define BASE_X_CURSOR_INCREMENT_COLOURS_SETTINGS_MENU_IN 40
#define BASE_Y_CURSOR_COLOURS_SETTINGS_MENU 24
#define BASE_Y_CURSOR_INCREMENT_COLOURS_SETTINGS_MENU 16

#define BASE_X_CURSOR_TRADE_OPTIONS 2
#define BASE_X_CURSOR_INCREMENT_TRADE_OPTIONS (SCREEN_HALF_X<<3)
#define BASE_Y_CURSOR_TRADE_OPTIONS (TRADE_OPTIONS_WINDOW_Y<<3)

#define BASE_X_CURSOR_LEARN_MOVE_MESSAGE_YES (((LEARN_MOVE_MESSAGE_WINDOW_X+1)<<3)+2)
#define BASE_X_CURSOR_LEARN_MOVE_MESSAGE_NO (((LEARN_MOVE_MESSAGE_WINDOW_X_SIZE<<3)>>1) + BASE_X_CURSOR_LEARN_MOVE_MESSAGE_YES)
#define BASE_Y_CURSOR_LEARN_MOVE_MESSAGE ((LEARN_MOVE_MESSAGE_WINDOW_Y+LEARN_MOVE_MESSAGE_WINDOW_Y_SIZE-1)<<3)

#define BASE_X_CURSOR_LEARN_MOVE_MENU 2
#define BASE_Y_CURSOR_INCREMENT_LEARN_MOVE_MENU 16
#define BASE_Y_CURSOR_LEARN_MOVE_MENU (SCREEN_HEIGHT-8-(BASE_Y_CURSOR_INCREMENT_LEARN_MOVE_MENU*(MOVES_SIZE-1+1)))

#define BASE_Y_SPRITE_OFFER_MENU (TOP_SCREEN_SPRITE_POS-8)
#define BASE_X_SPRITE_OFFER_MENU 0
#define BASE_X_CURSOR_INCREMENT_OFFER_OPTIONS 64
#define BASE_Y_CURSOR_INCREMENT_OFFER_OPTIONS 16
#define BASE_X_CURSOR_OFFER_OPTIONS ((OFFER_OPTIONS_WINDOW_X<<3)+2)
#define BASE_Y_CURSOR_OFFER_OPTIONS (((OFFER_OPTIONS_WINDOW_Y+OFFER_OPTIONS_WINDOW_Y_SIZE-1)<<3)-(BASE_Y_CURSOR_INCREMENT_OFFER_OPTIONS*(2-1)))

void init_sprites(void);
void enable_sprites_rendering(void);
void disable_sprites_rendering(void);
void init_sprite_counter(void);
void init_oam_palette(void);
void init_item_icon(void);
void set_pokemon_sprite(const u8*, u8, u8, u8, u8, u16, u16);
void set_party_sprite_counter(void);
void init_cursor(void);
void update_cursor_y(u16);
void update_cursor_base_x(u16);
void raw_update_sprite_y(u8, u8);
void move_sprites(u8 counter);
void move_cursor_x(u8 counter);
void disable_cursor(void);
void disable_all_cursors(void);
u8 get_next_sprite_index(void);
void reset_sprites(u8);
void disable_all_sprites(void);
void enable_all_sprites(void);
void update_normal_oam(void);
void reset_sprites_to_cursor(u8);
void reset_sprites_to_party(void);
void fade_all_sprites_to_white(u16);
void remove_fade_all_sprites(void);
void set_cursor_palette(void);

#endif
