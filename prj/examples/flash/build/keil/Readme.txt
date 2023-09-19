1. This example code applies to MCUs of GM66xx series;
2. All flash content of first 4K size will be copied to SRAM during the startup phase;
3. Code runs on SRAM once after SRAM space is remapped to 0x000000000 address, so that 
   a mass-erase command in the code is quite possible.
4. Each time when code runs to end, you must re-download such code into flash, since
   such example code mass-erases the flash to a complete blank one.