/* kbd.h - Pilsan Keyboard Header File */

#ifndef __INCkbdh
#define __INCkbdh

#define aioBase 0x300

#define kbdInAddress 0x10
#define kbdInMask 0xF0
#define kbdOutAddress 0x11
#define kbdOutMask 0x0F
#define kbdOut0Bit 0xE0
/* kbdOut0 is on Port B, Bit 5 */
#define kbdOut1Bit 0xD0
/* kbdOut1 is on Port B, Bit 6 */
#define kbdOut2Bit 0xB0
/* kbdOut2 is on Port B, Bit 7 */
#define kbdOut3Bit 0x70
/* kbdOut3 is on Port B, Bit 8 */


#endif /* __INCkbdh */

char getKey (void);
