#ifndef KGC_ERRORS_H
#define KGC_ERRORS_H

// Node creation errors
#define ERR_NOD_UNINITIALIZED 1202

// Ascent autopilot errors
#define ERR_ASC_NON_EXIST_NOUN 1301
#define ERR_ASC_UNINITIALIZED 1302
#define ERR_ASC_BAD_ASC_PROFILE 1303

void display_err_code(uint16_t err_code);

#endif //KGC_ERRORS_H