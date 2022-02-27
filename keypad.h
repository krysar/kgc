#ifndef KGC_KEYPAD_H
#define KGC_KEYPAD_H

#define KEY_SECTION 10
#define KEY_COMMAND 11
#define KEY_ENTER   15

extern uint8_t keypad_status, section, command;
extern uint8_t program[], prev_program[];
extern bool debounce;
extern uint8_t section_choice[], command_choice[];
static uint8_t col[4] = {28, 27, 26, 22}, row[4] = {21, 20, 16, 15};
static uint8_t keymap[4][4] = {{1,  2, 3,  10},
                               {4,  5, 6,  11},
                               {7,  8, 9,  12},
                               {13, 0, 14, 15}};

void keypad_init();

int64_t debounce_unset(alarm_id_t id, void *user_data);
int64_t display_program(alarm_id_t id, void *user_data);
uint8_t key_evaluate(uint8_t pressed_key);
void keypad_irq_handler(uint gpio, uint32_t events);

#endif //KGC_KEYPAD_H
