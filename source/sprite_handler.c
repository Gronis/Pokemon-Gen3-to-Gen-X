#include <gba.h>
#include "sprite_handler.h"
#include "graphics_handler.h"
#include "print_system.h"

#include "sprite_cursor_bin.h"
#include "item_icon_bin.h"
#include "sprite_palettes_bin.h"
#include "item_icon_palette_bin.h"

#define OAM_ADDR 0x7000000
#define CPUFASTSET_FILL (0x1000000)

#define DISABLE_SPRITE (1<<9)
#define OFF_SCREEN_SPRITE 0xA0

const u16* sprite_cursor_gfx = (const u16*)sprite_cursor_bin;
const u16* item_icon_gfx = (const u16*)item_icon_bin;
const u16* sprite_palettes_bin_16 = (const u16*)sprite_palettes_bin;
const u16* item_icon_palette_bin_16 = (const u16*)item_icon_palette_bin;

u8 __sprite_counter;
u8 __inner_sprite_counter;
u8 __party_sprite_counter;
u8 __party_inner_sprite_counter;
u8 cursor_sprite;
u8 inner_cursor_sprite;
u16 cursor_base_x;

void init_sprite_counter(){
    __sprite_counter = 0;
    __inner_sprite_counter = 0;
}

void set_party_sprite_counter(){
    __party_sprite_counter = __sprite_counter;
    __party_inner_sprite_counter = __inner_sprite_counter;
}

void init_sprites(){
    reset_sprites(0);
}

u8 get_sprite_counter(){
    return __sprite_counter;
}

void inc_sprite_counter(){
    __sprite_counter++;
    __inner_sprite_counter++;
}

void inc_inner_sprite_counter(){
    __inner_sprite_counter++;
}

u8 get_first_variable_palette(){
    return ((sprite_palettes_bin_size + item_icon_palette_bin_size)>>5);
}

u32 get_vram_pos(){
    return VRAM+0x10000+(__sprite_counter*0x400);
}

int set_palette_3bpp(u8* colors, int index, int palette) {
    u8 new_palette = (index>>1) + get_first_variable_palette();
    const u8 num_colors = 1<<3;
    const u8 base = num_colors*(index & 1);
    
    for(int i = 0; i < num_colors; i++) {
        if(i)
            SPRITE_PALETTE[base+i+(new_palette<<4)] = SPRITE_PALETTE[colors[i]+(palette<<4)];
        else
            SPRITE_PALETTE[i+(new_palette<<4)] = SPRITE_PALETTE[colors[i]+(palette<<4)];
    }
    
    return new_palette;
}

void init_oam_palette(){
    for(int i = 0; i < (sprite_palettes_bin_size>>1); i++)
        SPRITE_PALETTE[i] = sprite_palettes_bin_16[i];
    for(int i = 0; i < (item_icon_palette_bin_size>>1); i++)
        SPRITE_PALETTE[i+(sprite_palettes_bin_size>>1)] = item_icon_palette_bin_16[i];
}

void init_item_icon(){
    u16* vram_pos = (u16*)(get_vram_pos() + 0x20 + (sprite_cursor_bin_size));
    for(int i = 0; i < (item_icon_bin_size>>1); i++)
        vram_pos[i] = item_icon_gfx[i];
    vram_pos = (u16*)(get_vram_pos() + 0x220 + (sprite_cursor_bin_size));
    for(int i = 0; i < (item_icon_bin_size>>1); i++)
        vram_pos[i] = item_icon_gfx[i];
}

u16 get_item_icon_tile(){
    return (32*cursor_sprite) + 1 + (sprite_cursor_bin_size>>5);
}

u16 get_mail_icon_tile(){
    return (32*cursor_sprite) + 2 + (sprite_cursor_bin_size>>5);
}

#define ITEM_ICON_INC_Y 24
#define ITEM_ICON_INC_X 24

void set_item_icon(u16 y, u16 x){
    set_attributes(y + ITEM_ICON_INC_Y, x + ITEM_ICON_INC_X, get_item_icon_tile() | (get_curr_priority()<<10) | ((sprite_palettes_bin_size>>5)<<12));
    inc_inner_sprite_counter();
}

void set_mail_icon(u16 y, u16 x){
    set_attributes(y + ITEM_ICON_INC_Y, x + ITEM_ICON_INC_X, get_mail_icon_tile() | (get_curr_priority()<<10) | ((sprite_palettes_bin_size>>5)<<12));
    inc_inner_sprite_counter();
}

void set_pokemon_sprite(u32 address, u8 palette, u8 info, u8 display_item, u8 display_mail, u16 y, u16 x){
    if(display_mail)
        set_mail_icon(y, x);
    else if(display_item)
        set_item_icon(y, x);
    u8 sprite_counter = get_sprite_counter();
    u8 colors[8];
    u8 is_3bpp = load_pokemon_sprite_gfx(address, get_vram_pos(), info, sprite_counter-1, colors);
    if(is_3bpp)
        palette = set_palette_3bpp(colors, sprite_counter-1, palette);
    set_attributes(y, x |(1<<15), (32*sprite_counter)|(get_curr_priority()<<10)|(palette<<12));
    inc_sprite_counter();
}

void init_cursor(){
    u16* vram_pos = (u16*)(get_vram_pos() + 0x20);
    for(int i = 0; i < (sprite_cursor_bin_size>>1); i++)
        vram_pos[i] = sprite_cursor_gfx[i];
    vram_pos = (u16*)(get_vram_pos() + 0x220);
    for(int i = 0; i < (sprite_cursor_bin_size>>1); i++)
        vram_pos[i] = sprite_cursor_gfx[i];
    for(int i = 0; i < TOTAL_BG; i++) {
        set_attributes(OFF_SCREEN_SPRITE | DISABLE_SPRITE, 0, ((32*__sprite_counter)+1) | ((3-i)<<10));
        if(i < TOTAL_BG-1)
            inc_inner_sprite_counter();
    }
    cursor_sprite = __sprite_counter;
    inner_cursor_sprite = __inner_sprite_counter;
    update_cursor_base_x(BASE_X_CURSOR_MAIN_MENU, 0);
    inc_sprite_counter();
}

void update_cursor_y(u16 cursor_y){
    *((u16*)(OAM_ADDR + (8*(inner_cursor_sprite-get_curr_priority())) + 0)) = cursor_y;
}

void update_cursor_x(u16 cursor_x){
    *((u16*)(OAM_ADDR + (8*(inner_cursor_sprite-get_curr_priority())) + 2)) = cursor_x;
}

void update_cursor_base_x(u16 cursor_x, u8 counter){
    cursor_base_x = cursor_x;
    move_cursor_x(counter);
}

void disable_cursor(){
    update_cursor_y(OFF_SCREEN_SPRITE | DISABLE_SPRITE);
}

void disable_all_cursors(){
    for(int i = 0; i < TOTAL_BG; i++)
        *((u16*)(OAM_ADDR + (8*(inner_cursor_sprite-i)) + 0)) = OFF_SCREEN_SPRITE | DISABLE_SPRITE;
}

void set_attributes(u16 obj_attr_0, u16 obj_attr_1, u16 obj_attr_2) {
    *((u16*)(OAM_ADDR + (8*__inner_sprite_counter) + 0)) = obj_attr_0;
    *((u16*)(OAM_ADDR + (8*__inner_sprite_counter) + 2)) = obj_attr_1;
    *((u16*)(OAM_ADDR + (8*__inner_sprite_counter) + 4)) = obj_attr_2;
}

void reset_sprites_to_cursor(){
    __sprite_counter = cursor_sprite+1;
    __inner_sprite_counter = inner_cursor_sprite+1;
    reset_sprites(__inner_sprite_counter);
}

void reset_sprites(u8 start){
    for(int i = start; i < 0x80; i++) {
        *((u16*)(OAM_ADDR + (8*i) + 0)) = OFF_SCREEN_SPRITE | DISABLE_SPRITE;
        *((u16*)(OAM_ADDR + (8*i) + 2)) = 0;
        *((u16*)(OAM_ADDR + (8*i) + 4)) = 0;
    }
}

void disable_all_sprites(){
    for(int i = 0; i < 0x80; i++)
        *((u16*)(OAM_ADDR + (8*i) + 0)) |= DISABLE_SPRITE;
}

void enable_all_valid_sprites(){
    for(int i = 0; i < 0x80; i++) {
        u16 attr_0 = *((u16*)(OAM_ADDR + (8*i) + 0));
        if(((attr_0 & 0xFF) < OFF_SCREEN_SPRITE))
            *((u16*)(OAM_ADDR + (8*i) + 0)) &= ~DISABLE_SPRITE;
    }
}

void reset_sprites_to_party(){
    __sprite_counter = __party_sprite_counter;
    __inner_sprite_counter = __party_inner_sprite_counter;
    reset_sprites(__party_inner_sprite_counter);
}

void move_sprites(u8 counter){
    if(!(counter & 7)) {
        for(int i = inner_cursor_sprite+1; i < __inner_sprite_counter; i++) {
            u16 obj_attr_2 = *((u16*)(OAM_ADDR + (8*i) + 4));
            if(obj_attr_2 & 0x10)
                obj_attr_2 &= ~0x10;
            else
                obj_attr_2 |= 0x10;
            *((u16*)(OAM_ADDR + (8*i) + 4)) = obj_attr_2;
        }
    }
}

void move_cursor_x(u8 counter){
    counter = counter & 0x3F;
    u8 pos = counter >> 3;
    if(pos > 4)
        pos = 8-pos;
    update_cursor_x(cursor_base_x + pos);
}
