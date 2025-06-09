#ifndef KGC_KEYPAD_H
#define KGC_KEYPAD_H

#define KEY_VERB 10
#define KEY_NOUN 11
#define KEY_CLR 
#define KEY_MINUS 45
#define KEY_DOT 46
#define KEY_ENTER 15
#define KEY_PROCEED 12

#define KEY_STAT_NO_CHANGE 0
#define KEY_STAT_PRG_CHANGE 1
#define KEY_STAT_PROCEED 2
#define KEY_STAT_NUM_INSERTING 3
#define KEY_STAT_NUM_INSERTED 4

#define NO_NUM_TO_READ 0
#define READ_NUM_DISP_1 1
#define READ_NUM_DISP_2 2
#define READ_NUM_DISP_3 3
#define READ_NUM_INSERTED 4

#define INSERTED_NUM_BUF_SIZE 11

#define SAVE_PREV_VERB_NOUN {\
    prev_verb = verb; \
    prev_noun = noun; \
    }

extern uint8_t keypad_status, verb, prev_verb, noun, prev_noun;
static uint8_t col[4] = {28, 27, 26, 22}, row[4] = {21, 20, 16, 15};
static uint8_t keymap[4][4] = {{1,  2, 3,  10},     // 1, 2, 3, Verb
                               {4,  5, 6,  11},     // 4, 5, 6, Noun
                               {7,  8, 9,  12},     // 7, 8, 9, CLR
                               {45, 0, 46, 15}};    // -, 0, ., Enter 

void keypad_init();
int64_t debounce_unset(alarm_id_t id, void *user_data);
int64_t display_program(alarm_id_t id, void *user_data);
uint8_t key_evaluate(uint8_t pressed_key);
uint8_t read_number(uint8_t pressed_key);
float request_number(uint8_t chip, uint8_t display);

void keypad_irq_handler(uint gpio, uint32_t events);

#endif //KGC_KEYPAD_H
