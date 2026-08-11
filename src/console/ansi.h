#pragma once


#define ANSI_ESC(code) "\033[" #code "m"

// Full reset
#define ANSI_RESET          ANSI_ESC(0)

/* Styles */
#define ANSI_STYLE_BOLD     ANSI_ESC(1) // Also known as "bright"
#define ANSI_STYLE_THIN     ANSI_ESC(2) // Also known as "faint"

/* Fore colors (by standard) */
#define ANSI_FORE_RESET     ANSI_ESC(39)
#define ANSI_FORE_BLACK     ANSI_ESC(30)
#define ANSI_FORE_RED       ANSI_ESC(31)
#define ANSI_FORE_GREEN     ANSI_ESC(32)
#define ANSI_FORE_YELLOW    ANSI_ESC(33)
#define ANSI_FORE_BLUE      ANSI_ESC(34)
#define ANSI_FORE_MAGENTA   ANSI_ESC(35)
#define ANSI_FORE_CYAN      ANSI_ESC(36)
#define ANSI_FORE_WHITE     ANSI_ESC(37)

/* Light fore colors (usually works, but not defined in standard) */
#define ANSI_FORE_LBLACK    ANSI_ESC(90)
#define ANSI_FORE_LRED      ANSI_ESC(91)
#define ANSI_FORE_LGREEN    ANSI_ESC(92)
#define ANSI_FORE_LYELLOW   ANSI_ESC(93)
#define ANSI_FORE_LBLUE     ANSI_ESC(94)
#define ANSI_FORE_LMAGENTA  ANSI_ESC(95)
#define ANSI_FORE_LCYAN     ANSI_ESC(96)
#define ANSI_FORE_LWHITE    ANSI_ESC(97)

/* Back colors (standard) */
#define ANSI_BACK_RESET     ANSI_ESC(49)
#define ANSI_BACK_BLACK     ANSI_ESC(40)
#define ANSI_BACK_RED       ANSI_ESC(41)
#define ANSI_BACK_GREEN     ANSI_ESC(42)
#define ANSI_BACK_YELLOW    ANSI_ESC(43)
#define ANSI_BACK_BLUE      ANSI_ESC(44)
#define ANSI_BACK_MAGENTA   ANSI_ESC(45)
#define ANSI_BACK_CYAN      ANSI_ESC(46)
#define ANSI_BACK_WHITE     ANSI_ESC(47)

/* Light back colors (usually works, but not defined in standard) */
#define ANSI_BACK_LBLACK    ANSI_ESC(100)
#define ANSI_BACK_LRED      ANSI_ESC(101)
#define ANSI_BACK_LGREEN    ANSI_ESC(102)
#define ANSI_BACK_LYELLOW   ANSI_ESC(103)
#define ANSI_BACK_LBLUE     ANSI_ESC(104)
#define ANSI_BACK_LMAGENTA  ANSI_ESC(105)
#define ANSI_BACK_LCYAN     ANSI_ESC(106)
#define ANSI_BACK_LWHITE    ANSI_ESC(107)
