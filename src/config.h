#ifndef CONFIG_H
#define CONFIG_H

#define CTRL(a) a & 0x1F
#define TITLE_TOP 1
#define TITLE_BOTTOM 0

// ------------------------------

#define BUFFER_MAX_SIZE    0      // How much memory do you need? value <= BUFFER_BLOCK_SIZE means that there was no "safe" limit set.
#define BUFFER_BLOCK_SIZE  256    // Specifies how many lines to allocate at once if needed more memory.
#define LINE_BLOCK_SIZE    32     // Specifies how many bytes to allocate for lines at once. This is clamped to buffer width


#define FONT_FILE       "Tamsyn8x16b.psf.gz"
#define TITLE_OFFSET   3
#define TITLE_POS      TITLE_BOTTOM

#define FREE_CURSOR_MOVE_RATE  2


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
#define KEY_FAST_CURSOR    CTRL('x')

#endif
