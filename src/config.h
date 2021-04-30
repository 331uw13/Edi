#ifndef CONFIG_H
#define CONFIG_H

#define CTRL(a) a & 0x1F
#define TITLE_TOP 1
#define TITLE_BOTTOM 0

// ------------------------------


#define FONTFILE       "Tamsyn8x16b.psf.gz"
#define TITLE_OFFSET   3
#define TITLE_POS      TITLE_BOTTOM

#define KEY_SAVE_CURRENT   CTRL('w')
#define KEY_QUIT_EDITOR    CTRL('q')
#define KEY_REPLACE_MODE   CTRL('r')
#define KEY_INSERT_MODE    CTRL('i')
#define KEY_SELECT_MODE    CTRL('s')
#define KEY_FILE_BROWSER   CTRL('f')
#define KEY_SWAP_LINE_UP   CTRL('o')
#define KEY_SWAP_LINE_DOWN CTRL('p')
#define KEY_GOTO_EOF       CTRL('e')  // goto end of the file.
#define KEY_GOTO_SOF       CTRL('g')  // goto start of the file.
#define KEY_GOTO_EOL       CTRL('d')  // goto end of the current line.
#define KEY_GOTO_SOL       CTRL('a')  // goto start of the current line.


#endif
