/*
  Author: Tairen Dunham
  Date: June 30, 2022
  Description: 'Build Your Own Text Editor' by snaptoken, inspired by antirez's 'Kilo'
                editor.
*/

/*** includes ***/

#include <asm-generic/ioctls.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(key) ((key) & 0x1f)

/*** data ***/

/*
  Create struct for original terminal flags
  https://www.man7.org/linux/man-pages/man3/termios.3.html <- termios docs
  
  Create global struct for editor state that contains data for terminal 
    width and height, this is gathered by getWindowSize function using 
    ioctl()

  Initilialize a config struct 'E'
*/
struct editorConfig {

  int screenrows;
  int screencols;
  struct termios orig_termios;

};

struct editorConfig E;

/*** terminal ***/

/*
  Prints error message thats acquired from const char * argument and exits
*/
void die (const char *s) {

    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
  
}

void disableRawMode () {
  
  /*
    Set orig_termios flags as user_default - called at exit
    TCSAFLUSH option discard any unread input before applying 
    changes to the terminal
  
    if tcsetattr returns -1 which is a an error/failure then
      print error message "tcsetattr" and exit
  */
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) {
    
    die("tcsetattr");
    
  }
  
}

void enableRawMode () {

  /*
    Get user default termios and return to defaults at exit
    exit with message "tcgetattr" on failure
  */
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);
  
  /*
    Set raw termios to user defualt before adding flag changes
  */
    struct termios raw = E.orig_termios;
  
  /*
  local flags c_lflag -
    Disable canonical mode with ICANON - input will be read byte-by-byte instead of
      line-by-line now
    Disable echo with ECHO 
    Disable SIGINT and SIGTSTP with ISIG - ctrl-c and ctrl-z will be read as a '3'
      byte and '26' byte respectively.
    Disbale XON and XOFF software flow control using ctrl-s and ctrl-q - ctrl-s 
      and ctrl-q will be read as a '19' byte and '17' byte respectiely
    Disable ctrl-v / IEXTEN flag - ctrl-v read as a '22' byte now, and on mac
      ctrl-o will now be a '15' byte
  
  input flags c_iflag - 
    Disable flags by setting flag (c_lflag ie. control local flag, c_iflag - input 
      flags), then set flag attributes to termios raw
    Disable translation of carriage returns ('13') to new lines('10') with ICRNL 
      (input carriage return new line) - ctrl-m and enter now read as a '13' byte 
      and not '10'
  output flags c_oflag - 
    Disable post-processing of output with OPOST - \r\n neccesarry for a newline
      now
  
  miscellaneous flags -
    includes BRKINT, INPCK, ISTRIP and CS8
    refer to miscellaneous flag section of https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
      for some more info. Most of these flags shouldn't have nay noticable effect
      in modern terminals, but switching them off apparently used to be commonplace 
      when enabling 'raw mode'
  
  control characters c_cc - 
    set VMIN and VTIME - VMIN sets minimum amount of bytes needed before read() can
      return, VTIME sets maximum amount of time to wait before read() returns in 100
      ms intervals
  */
  raw.c_iflag &= ~( BRKINT | ICRNL | INPCK | ISTRIP | IXON );
  raw.c_oflag &= ~( OPOST );
  raw.c_cflag &= ~( CS8 );
  raw.c_lflag &= ~( ECHO | ICANON | IEXTEN |ISIG );
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  
  /*
    set flag attributes
    exit with message "tcsetattr" on failure
  */
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
    
}

/*
  Waits for one keypress and returns it
*/
char editorReadKey () {
  /*
    Initialize char c to use as a buffer
  */  
  int nread;
  char c;
  
  /*
    Read STDIN_FILENO to address of buffer 'c' 1 byte at a time
    exit with message "read" on failure
    
    from 'snaptoken' regarding EAGAIN 
      "In Cygwin, when read() times out it returns -1 with an errno of EAGAIN, 
      instead of just returning 0 like it’s supposed to. To make it work in 
      Cygwin, we won’t treat EAGAIN as an error."
  */ 
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }
  
  return c;

}

/*
  ioctl(), TIOCGWINSZ, and struct winsize come from <sys/ioctl.h>
  If ioctl has a failure it will return -1 and a poss-
    -ible erroneous outcome of 0

  If it succeeded, we pass the values back by setting the int 
    references that were passed to the function. (This is a 
    common approach to having functions return multiple values 
    in C. It also allows you to use the return value to indicate 
    success or failure.)
*/
int getWindowSize (int *rows, int *cols) {

  struct winsize ws;
  
  //TIOCGWINSZ = terminal i/o control get window size  
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {

    return -1;

  } else {
    
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
    
  }

}

/*** output ***/

/*
  Draw tildes on all rows visable on screen (ie. cols size of 
    terminal)

  String '~\r\n' is 3 bytes that will write to STDOUT a tilde + carriage
    return + newline

*/
void editorDrawRows () {
  
  int y;
  for (y = 0; y < E.screenrows; ++y) {

    write(STDOUT_FILENO, "~\r\n", 3);
        
  }
  
}

/*
  \x1b = ESC character / ASCII dec = '27'
  
  terminal escape sequences always start with ESC followed by a '[' char
  the 'J' command - the 'J' command is for 'Erase In Display' / clear screen
    and the parameter of 2 is to clear the entire screen

  the H command is for cursor position - can take two arguments of row and 
    column number, for example /x1b[12;40H would center the cursor in the 
    center of the screen on a 80x24 size terminal. The default arguments 
    for H are (1;1). Rows and columns start at 1, not 0.

  refer to https://vt100.net/docs/vt100-ug/chapter3.html#ED for info on 
  'Erase In Display'

  VT100 escape sequences will mostly be used within this editor
*/
void editorRefreshScreen() {
  
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  
  //draw tildes on blank lines
  editorDrawRows();
  
  //reposition cursor after drawing tildes
  write(STDOUT_FILENO, "\x1b[H", 3);
  
}

/*** input ***/

/*
  Waits for a keypress and handles it - deals with mapping keys to editor 
    functions at a higher level
*/
void editorProcessKeypress () {
  
  char c = editorReadKey();
  
  switch (c) {

    //if input is 'ctrl + q' then exit
    //clear and reposition on exit
    case CTRL_KEY('q'):
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;
    
  }
  
}

/*** init ***/

void initEditor () {
  
  if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
  
}

int main () {
  /*
    Enable raw mode
    then initialize fields in editorConfig struct 'E'
  
    Refer to https://pubs.opengroup.org/onlinepubs/7908799/xbd/termios.html
      'General Terminal Interface - The Single UNIX ® Specification, Version 2'
      for info on canonical and raw input modes.
  */
  enableRawMode();
  initEditor();
  
  while (1) {
    
      editorRefreshScreen();
      editorProcessKeypress();
    
  }
  
  return 0;
  
} 
