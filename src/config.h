#ifndef CONFIG_H
#define CONFIG_H

#define CTRL(a) a & 0x1F
#define TITLE_BOTTOM 0

// ------------------------------

#define BUFFER_MAX_SIZE      0      // How much memory do you need? value <= BUFFER_BLOCK_SIZE means that there was no limit set.
#define BUFFER_BLOCK_SIZE    256    // Specifies how many lines to allocate at once if needed more memory.
#define LINE_BLOCK_SIZE      32     // Specifies how many bytes to allocate for lines at once. This is clamped to buffer width
#define MAX_FRAME_TITLE_LEN  32
#define MAX_FRAMES           32
#define MAX_BUFFERS          16
#define MAX_MSG_LEN          512

#define TITLE_OFFSET   3
#define TITLE_POS      0     // 1 for top and 0 for bottom.

#define FONT_FILE      "Tamsyn8x16b.psf.gz"    // NOTE: currently only supports psf2.  (TODO: add more support.)
#define TAB_WIDTH      4     // Tab character width.
#define FONT_SPACING   1     // Space between each character.
#define FONT_SCALE     1

#define KEY_SAVE_CURRENT   CTRL('w')
#define KEY_QUIT_EDITOR    CTRL('q')

#define KEY_COMMAND_INPUT  CTRL('c')
#define KEY_SWITCH_MODE    CTRL('x')
#define KEY_INSERT_MODE    'i'
#define KEY_REPLACE_MODE   'r'
#define KEY_SELECT_MODE    's'

#define KEY_FILE_BROWSER   CTRL('f')
#define KEY_SWAP_LINE_UP   CTRL('o')
#define KEY_SWAP_LINE_DOWN CTRL('p')
#define KEY_GOTO_EOF       CTRL('e')  // Goto end of the file.
#define KEY_GOTO_SOF       CTRL('g')  // Goto start of the file.
#define KEY_GOTO_EOL       CTRL('d')  // Goto end of the current line.
#define KEY_GOTO_SOL       CTRL('a')  // Goto start of the current line.

#endif
