// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Ni�o D�az (AntonioND)

#include <stdio.h>
#include <string.h>

#include <gba.h>

#include "multiboot_handler.h"
#include "gb_dump_receiver.h"
//#include "save.h"

// --------------------------------------------------------------------

#define PACKED __attribute__((packed))
#define ALWAYS_INLINE __attribute__((always_inline)) static inline
#define MAX_DUMP_SIZE 0x20000
#define REG_WAITCNT		*(vu16*)(REG_BASE + 0x204)  // Wait state Control
#define SAVE_TYPES 4
#define SRAM_SAVE_TYPE 0
#define FLASH_64_SAVE_TYPE 1
#define FLASH_128_SAVE_TYPE 2
#define ROM_SAVE_TYPE 3

// --------------------------------------------------------------------

const char* save_strings[] = {"SRAM 64KiB", "Flash 64 KiB", "Flash 128 KiB", "Inside ROM"};
const char* person_strings[] = {" - You", " - Other"};
const char* actor_strings[] = {"Master", "Slave"};
const char* region_strings[] = {"Int", "Jap"};
const char* target_strings[] = {"Gen 1", "Gen 2", "Gen 3"};

// void save_to_memory(int size) {
    // int chosen_save_type = 0;
    // u8 copied_properly = 0;
    // int decided = 0, completed = 0;
    // u16 keys;
    // REG_WAITCNT |= 3;
    
    // while(!completed) {
        // while(!decided) {
            // iprintf("\x1b[2J");
            // iprintf("Save type: %s\n\n", save_strings[chosen_save_type]);
            // iprintf("LEFT/RIGHT: Change save type\n\n");
            // iprintf("START: Save\n\n");
        
            // scanKeys();
            // keys = keysDown();
            
            // while ((!(keys & KEY_START)) && (!(keys & KEY_LEFT)) && (!(keys & KEY_RIGHT))) {
                // VBlankIntrWait();
                // scanKeys();
                // keys = keysDown();
            // }
            
            // if(keys & KEY_START)
                // decided = 1;
            // else if(keys & KEY_LEFT)
                // chosen_save_type -= 1;
            // else if(keys & KEY_RIGHT)
                // chosen_save_type += 1;
            
            // if(chosen_save_type < 0)
                // chosen_save_type = SAVE_TYPES - 1;
            // if(chosen_save_type >= SAVE_TYPES)
                // chosen_save_type = 0;
        // }
        
        // iprintf("\x1b[2J");
        
        // switch (chosen_save_type) {
            // case SRAM_SAVE_TYPE:
                // sram_write((u8*)EWRAM, size);
                // copied_properly = is_sram_correct((u8*)EWRAM, size);
                // break;
            // case FLASH_64_SAVE_TYPE:
                // flash_write((u8*)EWRAM, size, 0);
                // copied_properly = is_flash_correct((u8*)EWRAM, size, 0);
                // break;
            // case FLASH_128_SAVE_TYPE:
                // flash_write((u8*)EWRAM, size, 1);
                // copied_properly = is_flash_correct((u8*)EWRAM, size, 1);
                // break;
            // case ROM_SAVE_TYPE:
                // rom_write((u8*)EWRAM, size);
                // copied_properly = is_rom_correct((u8*)EWRAM, size);
            // default:
                // break;
        // }
        
        // if(copied_properly) {
            // iprintf("All went well!\n\n");
            // completed = 1;
        // }
        // else {
            // iprintf("Not everything went right!\n\n");
            // iprintf("START: Try saving again\n\n");
            // iprintf("SELECT: Abort\n\n");
        
            // scanKeys();
            // keys = keysDown();
            
            // while ((!(keys & KEY_START)) && (!(keys & KEY_SELECT))) {
                // VBlankIntrWait();
                // scanKeys();
                // keys = keysDown();
            // }
            
            // if(keys & KEY_SELECT) {
                // completed = 1;
                // iprintf("\x1b[2J");
            // }
            // else
                // decided = 0;
        // }
    // }
    
    // if(chosen_save_type == ROM_SAVE_TYPE)
        // iprintf("Save location: 0x%X\n\n", get_rom_address()-0x8000000);
// }

#include "sio.h"
#define VCOUNT_TIMEOUT 28 * 8

const u8 gen2_start_trade_states[] = {0x01, 0xFE, 0x61, 0xD1, 0xFE};
const u8 gen2_next_trade_states[] = {0xFE, 0x61, 0xD1, 0xFE, 0xFE};
const u8 gen_2_states_max = 4;
void start_gen2(void)
{
    u8 index = 0;
    u8 received;
    init_sio_normal(SIO_MASTER, SIO_8);
    
    while(2) {
        received = timed_sio_normal_master(gen2_start_trade_states[index], SIO_8, VCOUNT_TIMEOUT);
        if(received == gen2_next_trade_states[index] && index < gen_2_states_max)
            index++;
        else
            iprintf("0x%X\n", received);
    }
}
const u8 gen2_start_trade_slave_states[] = {0x02, 0x61, 0xD1, 0x00, 0xFE};
const u8 gen2_next_trade_slave_states[] = {0x01, 0x61, 0xD1, 0x00, 0xFE};
void start_gen2_slave(void)
{
    u8 index = 0;
    u8 received;
    init_sio_normal(SIO_SLAVE, SIO_8);
    
    sio_normal_prepare_irq_slave(0x02, SIO_8);
    
    while(3) {
        iprintf("0x%X\n", REG_SIODATA8 & 0xFF);
    }
    
    while(2) {
        received = sio_normal(gen2_start_trade_slave_states[index], SIO_SLAVE, SIO_8);
        iprintf("0x%X\n", received);
        if(received == gen2_next_trade_slave_states[index] && index < gen_2_states_max)
            index++;
    }
}

#define FLASH_BANK_CMD *(((vu8*)SRAM)+0x5555) = 0xAA;*(((vu8*)SRAM)+0x2AAA) = 0x55;*(((vu8*)SRAM)+0x5555) = 0xB0;
#define BANK_SIZE 0x10000

#define SAVE_SLOT_SIZE 0xE000
#define SAVE_SLOT_INDEX_POS 0xDFFC
#define SECTION_ID_POS 0xFF4
#define SECTION_SIZE 0x1000
#define SECTION_TOTAL 14
#define SECTION_TRAINER_INFO_ID 0
#define SECTION_PARTY_INFO_ID 1
#define SECTION_GAME_CODE 0xAC
#define FRLG_GAME_CODE 0x1
#define RSE_PARTY 0x234
#define FRLG_PARTY 0x34

#include "party_handler.h"
#include "text_handler.h"
#include "sprite_handler.h"

u8 trainer_name[OT_NAME_GEN3_SIZE+1];
u8 other_name[OT_NAME_GEN3_SIZE+1];
u8 trainer_gender;
u32 trainer_id;
u8 is_gen2_valid;
u8 is_gen1_valid;
struct gen3_party parties_3[2];
struct gen2_party parties_2[2];
struct gen1_party parties_1[2];

u8 current_bank;

void init_bank(){
    current_bank = 2;
}

void bank_check(u32 address){
    u8 bank = 0;
    if(address >= BANK_SIZE)
        bank = 1;
    if(bank != current_bank) {
        FLASH_BANK_CMD
        *((vu8*)SRAM) = bank;
        current_bank = bank;
    }
}

u32 read_int(u32 address){
    bank_check(address);
    return (*(vu8*)(SRAM+address)) + ((*(vu8*)(SRAM+address+1)) << 8) + ((*(vu8*)(SRAM+address+2)) << 16) + ((*(vu8*)(SRAM+address+3)) << 24);
}

u16 read_short(u32 address){
    bank_check(address);
    return (*(vu8*)(SRAM+address)) + ((*(vu8*)(SRAM+address+1)) << 8);
}

void copy_to_ram(u32 base_address, u8* new_address, int size){
    bank_check(base_address);
    for(int i = 0; i < size; i++)
        new_address[i] = (*(vu8*)(SRAM+base_address+i));
}

u32 read_slot_index(int slot) {
    if(slot != 0)
        slot = 1;
    
    return read_int((slot * SAVE_SLOT_SIZE) + SAVE_SLOT_INDEX_POS);
}

u32 read_section_id(int slot, int section_pos) {
    if(slot != 0)
        slot = 1;
    if(section_pos < 0)
        section_pos = 0;
    if(section_pos >= SECTION_TOTAL)
        section_pos = SECTION_TOTAL - 1;
    
    return read_short((slot * SAVE_SLOT_SIZE) + (section_pos * SECTION_SIZE) + SECTION_ID_POS);
}

u32 read_game_code(int slot) {
    if(slot != 0)
        slot = 1;
    
    u32 game_code = -1;

    for(int i = 0; i < SECTION_TOTAL; i++)
        if(read_section_id(slot, i) == SECTION_TRAINER_INFO_ID) {
            copy_to_ram((slot * SAVE_SLOT_SIZE) + (i * SECTION_SIZE), trainer_name, OT_NAME_GEN3_SIZE+1);
            copy_to_ram((slot * SAVE_SLOT_SIZE) + (i * SECTION_SIZE) + 8, &trainer_gender, 1);
            copy_to_ram((slot * SAVE_SLOT_SIZE) + (i * SECTION_SIZE) + 10, (u8*)&trainer_id, 4);
            game_code = read_int((slot * SAVE_SLOT_SIZE) + (i * SECTION_SIZE) + SECTION_GAME_CODE);
            break;
        }
    
    return game_code;
}

void read_party(int slot) {
    if(slot != 0)
        slot = 1;
    
    u32 game_code = read_game_code(slot);
    u16 add_on = RSE_PARTY;
    if(game_code == FRLG_GAME_CODE)
        add_on = FRLG_PARTY;

    for(int i = 0; i < SECTION_TOTAL; i++)
        if(read_section_id(slot, i) == SECTION_PARTY_INFO_ID) {
            copy_to_ram((slot * SAVE_SLOT_SIZE) + (i * SECTION_SIZE) + add_on, (u8*)(&parties_3[0]), sizeof(struct gen3_party));
            break;
        }
    if(parties_3[0].total > PARTY_SIZE)
        parties_3[0].total = PARTY_SIZE;
    u8 curr_slot = 0;
    for(int i = 0; i < parties_3[0].total; i++)
        if(gen3_to_gen2(&parties_2[0].mons[curr_slot], &parties_3[0].mons[i], trainer_id)) {
            curr_slot++;
            is_gen2_valid |= (1<<i);
        }
    parties_2[0].total = curr_slot;
    curr_slot = 0;
    for(int i = 0; i < parties_3[0].total; i++)
        if(gen3_to_gen1(&parties_1[0].mons[curr_slot], &parties_3[0].mons[i], trainer_id)) {
            curr_slot++;
            is_gen1_valid |= (1<<i);
        }
    parties_1[0].total = curr_slot;
}

#define MAGIC_NUMBER 0x08012025
#define INVALID_SLOT 2
u16 summable_bytes[SECTION_TOTAL] = {3884, 3968, 3968, 3968, 3848, 3968, 3968, 3968, 3968, 3968, 3968, 3968, 3968, 2000};

u8 validate_slot(int slot) {
    if(slot != 0)
        slot = 1;
    
    u16 valid_sections = 0;
    u32 buffer[0x400];
    u16* buffer_16 = (u16*)buffer;
    u32 current_save_index = read_slot_index(slot);
    
    for(int i = 0; i < SECTION_TOTAL; i++)
    {
        copy_to_ram((slot * SAVE_SLOT_SIZE) + (i * SECTION_SIZE), (u8*)buffer, SECTION_SIZE);
        if(buffer[0x3FE] != MAGIC_NUMBER)
            return 0;
        if(buffer[0x3FF] != current_save_index)
            return 0;
        u16 section_id = buffer_16[0x7FA];
        if(section_id >= SECTION_TOTAL)
            return 0;
        u16 prepared_checksum = buffer_16[0x7FB];
        u32 checksum = 0;
        for(int j = 0; j < (summable_bytes[section_id] >> 2); j++)
            checksum += buffer[j];
        checksum = ((checksum & 0xFFFF) + (checksum >> 16)) & 0xFFFF;
        if(checksum != prepared_checksum)
            return 0;
        valid_sections |= (1 << section_id);
    }
    
    if(valid_sections != (1 << (SECTION_TOTAL))-1)
        return 0;

    return 1;
}

u8 get_slot(){
    u32 last_valid_save_index = 0;
    u8 slot = INVALID_SLOT;

    for(int i = 0; i < 2; i++) {
        u32 current_save_index = read_slot_index(i);
        if((current_save_index >= last_valid_save_index) && validate_slot(i)) {
            slot = i;
            last_valid_save_index = current_save_index;
        }
    }
    return slot;
}

void read_gen_3_data(){
    REG_WAITCNT |= 3;
    init_bank();
    parties_1[0].total = 0;
    parties_2[0].total = 0;
    parties_3[0].total = 0;
    is_gen2_valid = 0;
    is_gen1_valid = 0;
    parties_1[1].total = 0;
    parties_2[1].total = 0;
    parties_3[1].total = 0;
    
    u8 slot = get_slot();
    if(slot == INVALID_SLOT) {
        return;
    }
    
    read_party(slot);
    
    // Terminate the names
    trainer_name[OT_NAME_GEN3_SIZE] = GEN3_EOL;
    for(int i = 0; i < (OT_NAME_GEN3_SIZE+1); i++)
        other_name[i] = GEN3_EOL;
}

u8 get_valid_options() {
    return (parties_1[0].total > 0 ? 1: 0) | (parties_2[0].total > 0 ? 2: 0) | (parties_3[0].total > 0 ? 4: 0);
}

void fill_options_array(u8* options) {
    u8 curr_slot = 0;
    for(int i = 0; i < 3; i++)
        options[i] = 0;
    if(parties_1[0].total > 0) {
        options[curr_slot++] = 1;
        for(int i = curr_slot; i < 3; i++)
            options[i] = 1;
    }
    if(parties_2[0].total > 0) {
        options[curr_slot++] = 2;
        for(int i = curr_slot; i < 3; i++)
            options[i] = 2;
    }
    if(parties_3[0].total > 0) {
        options[curr_slot++] = 3;
        for(int i = curr_slot; i < 3; i++)
            options[i] = 3;
    }
}

u8 get_number_of_higher_options(u8* options, u8 curr_option) {
    u8 curr_num = 0;
    u8 highest_found = curr_option;
    for(int i = 0; i < 3; i++)
        if(options[i] > highest_found) {
            curr_num++;
            highest_found = options[i];
        }
    return curr_num;
}

u8 get_number_of_lower_options(u8* options, u8 curr_option) {
    u8 curr_num = 0;
    u8 highest_found = curr_option;
    for(int i = 2; i >= 0; i--)
        if(options[i] < highest_found) {
            curr_num++;
            highest_found = options[i];
        }
    return curr_num;
}

void fill_trade_options(u8* options, u8 curr_gen, u8 is_own) {
    struct gen3_party* party = &(parties_3[1]);
    if(is_own)
        party = &(parties_3[0]);
    for(int i = 0; i < party->total; i++)
        options[i] = i;
    for(int i = party->total; i < PARTY_SIZE; i++)
        options[i] = 0xFF;
    if(is_own && (curr_gen == 1 || curr_gen == 2)) {
        u8 valid_options = is_gen2_valid;
        if(curr_gen == 1)
            valid_options = is_gen1_valid;
        u8 curr_slot = 0;
        for(int i = 0; i < party->total; i++)
            if(valid_options & (1 << i))
                options[curr_slot++] = i;
        for(int i = curr_slot; i < PARTY_SIZE; i++)
            options[i] = 0xFF;
    }
}

#define X_TILES 30
#define BASE_Y_SPRITE_TRADE_MENU 0
#define BASE_Y_SPRITE_INCREMENT_TRADE_MENU 24
#define BASE_X_SPRITE_TRADE_MENU 8

void print_trade_menu(u8 update, u8 curr_gen, u8 load_sprites) {
    if(!update)
        return;
    
    iprintf("\x1b[2J");
    
    u8 printable_string[X_TILES+1];
    u8 tmp_buffer[X_TILES+1];
    
    text_generic_terminator_fill(printable_string, X_TILES+1);
    text_gen3_to_generic(trainer_name, tmp_buffer, OT_NAME_GEN3_SIZE+1, X_TILES, 0, 0);
    text_generic_concat(tmp_buffer, person_strings[0], printable_string, OT_NAME_GEN3_SIZE, (X_TILES >> 1)-OT_NAME_GEN3_SIZE, X_TILES>>1);
    text_gen3_to_generic(other_name, tmp_buffer, OT_NAME_GEN3_SIZE+1, X_TILES, 0, 0);
    text_generic_concat(tmp_buffer, person_strings[1], printable_string + (X_TILES >> 1), OT_NAME_GEN3_SIZE, (X_TILES >> 1)-OT_NAME_GEN3_SIZE, X_TILES>>1);
    text_generic_replace(printable_string, X_TILES, GENERIC_EOL, GENERIC_SPACE);
    iprintf("%s", printable_string);

    if(load_sprites)
        reset_sprites_to_cursor();

    u8 options[2][PARTY_SIZE];
    fill_trade_options(options[0], curr_gen, 1);
    fill_trade_options(options[1], curr_gen, 0);
    for(int i = 0; i < PARTY_SIZE; i++) {
        text_generic_terminator_fill(printable_string, X_TILES+1);
        for(int j = 0; j < 2; j++)
            if(options[j][i] != 0xFF) {
                struct gen3_mon* mon = &parties_3[j].mons[options[j][i]];
                text_generic_copy(get_pokemon_name_raw(mon), printable_string + 5 + ((X_TILES>>1)*j), NAME_SIZE, (X_TILES>>1) - 5);
                if(load_sprites)
                    load_pokemon_sprite_raw(mon, BASE_Y_SPRITE_TRADE_MENU + (i*BASE_Y_SPRITE_INCREMENT_TRADE_MENU), (((X_TILES >> 1) << 3)*j) + BASE_X_SPRITE_TRADE_MENU);
            }
        text_generic_replace(printable_string, X_TILES, GENERIC_EOL, GENERIC_SPACE);
        iprintf("\n%s\n", printable_string);
    }
    iprintf("  Cancel");
}

#define BASE_Y_CURSOR_MAIN_MENU 8
#define BASE_Y_CURSOR_INCREMENT_MAIN_MENU 16

void print_main_menu(u8 update, u8 curr_gen, u8 is_jp, u8 is_master) {
    if(!update)
        return;
    
    u8 valid_options = get_valid_options();
    u8 options[3];

    iprintf("\x1b[2J");
    
    if(!valid_options)
        iprintf("\n  Error reading the data!\n\n\n\n\n\n\n");
    else {
        fill_options_array(options);
        if(curr_gen >= 3)
            curr_gen = 2;
        curr_gen = options[curr_gen];
        if(get_number_of_higher_options(options, curr_gen) > 0 && get_number_of_lower_options(options, curr_gen) > 0)
            iprintf("\n  Target: <%s>\n", target_strings[curr_gen-1]);
        else if(get_number_of_higher_options(options, curr_gen) > 0)
            iprintf("\n  Target:  %s>\n", target_strings[curr_gen-1]);
        else if(get_number_of_lower_options(options, curr_gen) > 0)
            iprintf("\n  Target: <%s\n", target_strings[curr_gen-1]);
        else
            iprintf("\n  Target:  %s\n", target_strings[curr_gen-1]);
        if(curr_gen < 3) {
            if(!is_jp)
                iprintf("\n  Target Region:  %s>\n", region_strings[0]);
            else
            iprintf("\n  Target Region: <%s\n", region_strings[1]);
        }
        else
            iprintf("\n\n");
        if(!is_master)
            iprintf("\n  Act as:  %s>\n", actor_strings[1]);
        else
            iprintf("\n  Act as: <%s\n", actor_strings[0]);
        iprintf("\n  Start Trade\n");
    }
    iprintf("\n  Send Multiboot");
}

#define START_MULTIBOOT 0x49

u8 handle_input_main_menu(u8* cursor_y_pos, u16 keys, u8* update, u8* target, u8* region, u8* master) {

    u8 options[3];
    fill_options_array(options);
    u8 curr_gen = *target;
    if(curr_gen >= 3)
        curr_gen = 2;
    curr_gen = options[curr_gen];
    
    switch(*cursor_y_pos) {
        case 0:
            if((keys & KEY_RIGHT) && (get_number_of_higher_options(options, curr_gen) > 0)) {
                (*target) += 1;
                (*update) = 1;
            }
            else if((keys & KEY_LEFT) && (get_number_of_lower_options(options, curr_gen) > 0)) {
                (*target) -= 1;
                (*update) = 1;
            }
            else if((keys & KEY_A) && (get_number_of_higher_options(options, curr_gen) > 0)) {
                (*target) += 1;
                (*update) = 1;
            }
            else if((keys & KEY_A) && (get_number_of_lower_options(options, curr_gen) > 0)) {
                (*target) -= 1;
                (*update) = 1;
            }
            else if(keys & KEY_DOWN) {
                if(curr_gen == 3)
                    (*cursor_y_pos) += 2;
                else
                    (*cursor_y_pos) += 1;
            }
            else if(keys & KEY_UP) {
                (*cursor_y_pos) = 4;
            }
            break;
        case 1:
            if(curr_gen == 3)
                (*cursor_y_pos) += 1;
            else if((keys & KEY_RIGHT) && (!(*region))) {
                (*region) = 1;
                (*update) = 1;
            }
            else if((keys & KEY_LEFT) && ((*region))) {
                (*region) = 0;
                (*update) = 1;
            }
            else if((keys & KEY_A) && (!(*region))) {
                (*region) = 1;
                (*update) = 1;
            }
            else if((keys & KEY_A) && ((*region))) {
                (*region) = 0;
                (*update) = 1;
            }
            else if(keys & KEY_DOWN) {
                (*cursor_y_pos) += 1;
            }
            else if(keys & KEY_UP) {
                (*cursor_y_pos) -= 1;
            }
            break;
        case 2:
            if((keys & KEY_RIGHT) && (!(*master))) {
                (*master) = 1;
                (*update) = 1;
            }
            else if((keys & KEY_LEFT) && ((*master))) {
                (*master) = 0;
                (*update) = 1;
            }
            else if((keys & KEY_A) && (!(*master))) {
                (*master) = 1;
                (*update) = 1;
            }
            else if((keys & KEY_A) && ((*master))) {
                (*master) = 0;
                (*update) = 1;
            }
            else if(keys & KEY_DOWN)
                (*cursor_y_pos) += 1;
            else if(keys & KEY_UP) {
                if(curr_gen == 3)
                    (*cursor_y_pos) -= 2;
                else
                    (*cursor_y_pos) -= 1;
            }
            break;
        case 3:
            if((keys & KEY_A) && (get_valid_options()))
                return curr_gen;
            else if(keys & KEY_DOWN)
                (*cursor_y_pos) += 1;
            else if(keys & KEY_UP)
                (*cursor_y_pos) -= 1;
            break;
        default:
            if(keys & KEY_A) {
                return START_MULTIBOOT;
            }
            else if((keys & KEY_DOWN) && (get_valid_options())) {
                (*cursor_y_pos) = 0;
            }
            else if((keys & KEY_UP) && (get_valid_options())) {
                (*cursor_y_pos) = 3;
            }
            break;
    }
    return 0;
}

int main(void)
{
    int val = 0;
    u16 keys;
    enum MULTIBOOT_RESULTS result;
    
    init_oam_palette();
    init_sprite_counter();
    irqInit();
    irqEnable(IRQ_VBLANK);
    irqSet(IRQ_SERIAL, sio_handle_irq_slave);
    irqEnable(IRQ_SERIAL);

    consoleDemoInit();
    REG_DISPCNT |= OBJ_ON | OBJ_1D_MAP;
    
    iprintf("\x1b[2J");
    
    read_gen_3_data();
    
    u8 update = 1;
    u8 target = 1;
    u8 region = 0;
    u8 master = 0;
    u8 cursor_y_pos = 0;
    
    if(!get_valid_options())
        cursor_y_pos = 4;
    
    print_main_menu(update, target, region, master);
    
    init_item_icon();
    init_cursor(cursor_y_pos);
    u8 counter = 0;
    
    update = 0;
    
    while(1) {
        scanKeys();
        keys = keysDown();
        
        while ((!(keys & KEY_LEFT)) && (!(keys & KEY_RIGHT)) && (!(keys & KEY_A)) && (!(keys & KEY_UP)) && (!(keys & KEY_DOWN))) {
            VBlankIntrWait();
            move_sprites(counter);
            move_cursor_x(counter);
            counter++;
            scanKeys();
            keys = keysDown();
        }
        handle_input_main_menu(&cursor_y_pos, keys, &update, &target, &region, &master);
        print_main_menu(update, target, region, master);
        //print_trade_menu(update, 2, 1);
        update_cursor_y(BASE_Y_CURSOR_MAIN_MENU + (BASE_Y_CURSOR_INCREMENT_MAIN_MENU * cursor_y_pos));
        update = 0;
    }

    // while (1) {
    
        // scanKeys();
        // keys = keysDown();
        
        // while ((!(keys & KEY_START)) && (!(keys & KEY_SELECT))) {
            // VBlankIntrWait();
            // scanKeys();
            // keys = keysDown();
        // }
        
        // if(keys & KEY_START) {        
            // result = multiboot_normal((u16*)payload, (u16*)(payload + PAYLOAD_SIZE));
            
            // iprintf("\x1b[2J");
            
            // if(result == MB_SUCCESS)
                // iprintf("Multiboot successful!\n\n");
            // else if(result == MB_NO_INIT_SYNC)
                // iprintf("Couldn't sync.\nTry again!\n\n");
            // else
                // iprintf("There was an error.\nTry again!\n\n");
        // }
        // else {
            // val = read_dump(MAX_DUMP_SIZE);
            
            // iprintf("\x1b[2J");
            
            // if(val == GENERIC_DUMP_ERROR)
                // iprintf("There was an error!\nTry again!\n\n");
            // else if(val == SIZE_DUMP_ERROR)
                // iprintf("Dump size is too great!\n\n");
            // else {
                // save_to_memory(val);
            // }
        // }
    // }

    //start_gen2_slave();
    //start_gen2();

    return 0;
}
