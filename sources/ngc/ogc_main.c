/*
 * Gnuboy Main for the Nintendo Gamecube
 * Eke-Eke 2007
 * This file may be distributed under the terms of the GNU GPL.
 */

#include "defs.h"
#include "mem.h"
#include "lcd.h"
#include "font.h"
#include "config.h"

#include "history.h"
#include "file_fat.h"

#include <malloc.h>

#define MAXROMSIZE 8388608

int gbromsize;
u8 *gbrom;
int new_game;

extern void MainMenu ();
extern void legal();

extern void memfile_autosave();
extern void memfile_autoload();

void reload_rom()
{
  loader_unload();
  mem_init ();
  rom_load();
  pcm_reset();
  vid_reset();
  emu_reset();
  new_game = 1;
}

int main(int argc, char *argv[])
{
  vid_init();
  pcm_init();
  mem_init();
  legal();

  gbrom = memalign(32,MAXROMSIZE);
  gbromsize = 0;
  
  /* try to load Config File */
  config_load();
  
  /* file autoloading */
  int autoload = 0;

  int size = 0;

  /* autodetect loader arguments */
  if ((argc >= 3) && (argv[1] != NULL) && (argv[2] != NULL))
  {
    /* automatically load any file passed as argument */
    autoload = 1;

    /* add the file to the top of the history. */
    history_add_file(argv[1], argv[2]);
  }

  /* automatically load first file from history list if requested */
  if (autoload)
  {
    size = FAT_Open(TYPE_RECENT,gbrom);
    if (size)
    {
      gbromsize = size;
      memfile_autosave();
      reload_rom();
      memfile_autoload(); 
    }
  }

  /* show disclaimer before entering menu */
  if (!(autoload && size))
  {
    legal();
  }

  /* Wait for loaded rom */
  while (gbromsize == 0) MainMenu();
  
  /* go to Main Loop */
  emu_run();
 
  /* never reached */
  return 0;
}

