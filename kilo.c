/*
  Author: Tairen Dunham
  Date: June 30, 2022
  Description: 'Build Your Own Text Editor' by snaptoken, inspired by antirez's 'Kilo'
                editor.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*
  Create struct for original terminal flags
  https://www.man7.org/linux/man-pages/man3/termios.3.html <- termios docs
*/

struct termios orig_termios;

void disableRawMode () {
  
  /*
    Set orig_termios flags as user_default - called at exit
    TCSAFLUSH option discard any unread input before applying 
    changes to the terminal
  */
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
  
}

void enableRawMode () {

  /*
    Get user default termios and return to defaults at exit
  */
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  
  /*
    Set raw termios to user defualt before adding flag changes
  */
    struct termios raw = orig_termios;
  
  /*
  local flags -
    Disable canonical mode with ICANON - input will be read byte-by-byte instead of
      line-by-line now
    Disable echo with ECHO 
    Disable SIGINT and SIGTSTP with ISIG - ctrl-c and ctrl-z will be read as a '3'
      byte and '26' byte respectively.
    Disbale XON and XOFF software flow control using ctrl-s and ctrl-q - ctrl-s 
      and ctrl-q will be read as a '19' byte and '17' byte respectiely
    Disable ctrl-v / IEXTEN flag - ctrl-v read as a '22' byte now, and on mac
      ctrl-o will now be a '15' byte
  input flags - 
    Disable flags by setting flag (c_lflag ie. control local flag, c_iflag - input 
      flags), then set flag attributes to termios raw
    Disable translation of carriage returns ('13') to new lines('10') with ICRNL 
      (input carriage return new line) - ctrl-m and enter now read as a '13' byte 
      and not '10'
  output flags - 
    Disable post-processing of output with OPOST - \r\n neccesarry for a newline
      now
  miscellaneous flags -
    includes BRKINT, INPCK, ISTRIP and CS8
    refer to miscellaneous flag section of https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
      for some more info. Most of these flags shouldn't have nay noticable effect
      in modern terminals, but switching them off apparently used to be commonplace 
      when enabling 'raw mode'
  */
  raw.c_iflag &= ~( BRKINT | ICRNL | INPCK | ISTRIP | IXON );
  raw.c_oflag &= ~( OPOST );
  raw.c_cflag &= ~( CS8 );
  raw.c_lflag &= ~( ECHO | ICANON | IEXTEN |ISIG );
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
}


int main () {
  /*
    Enable raw mode
  
    Refer to https://pubs.opengroup.org/onlinepubs/7908799/xbd/termios.html
    'General Terminal Interface - The Single UNIX ® Specification, Version 2'
    for info on canonical and raw input modes.
  */
  enableRawMode();

  /*
    Initialize char c to use as a buffer
  */
  char c;
  
  /*
    Read STDIN_FILENO to address of buffer 'c' as long as byte is present
    and not 'q'
  */ 
  while(read(0, &c, 1) == 1 && c != 'q') {
    
    /*
      iscntrl test if input is a control character - control chars (ie. non printable 
      characters) are ASCII codes 0-31 and 127. ASCII 32-126 are printable characters
    */
    if (iscntrl(c)) {
      
      printf("%d\r\n", c);
      
    }
    else {
      
      printf("%d (%c)\r\n", c, c);
      
    }
    
  }
  
  return 0;
  
} 
