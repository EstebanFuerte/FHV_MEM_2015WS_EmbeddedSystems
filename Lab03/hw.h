/* hw.h - Pilsan Hardware Header File */

#ifndef __INChwh
#define __INChwh

#define beepAdd 0x61
#define beepMask 0xFC
#define beepOn 0x03
#define beepOff 0x00

/* IO hardware assignment */

#define aioBase 0x300

#define ioRegA 0x10
#define ioRegB 0x11
#define ioRegC 0x12
#define ioCntrlReg 0x13

#define ioClowIn 0x01
#define ioCHighOut 0x00
#define ioCUnused 0x09
#define ioBOut 0x00
#define ioABUnused 0x12
#define ioAIn 0x10
#define ioMode0 0x80

#define led5Address 0x11
#define led5Bit 0x01
/* Led5 is on Port B, Bit 0 */

#define led6Address 0x11
#define led6Bit 0x02
/* Led6 is on Port B, Bit 1 */

#define led7Address 0x11
#define led7Bit 0x04
/* Led7 is on Port B, Bit 2 */

#define led8Address 0x91
#define led8Bit 0x08
/* Led8 is on Port B, Bit 3 */

#define button1Address 0x10
#define button1Bit 0x01
/* Button1 is on Port A, Bit 0 */

#define button2Address 0x10
#define button2Bit 0x02
/* Button2 is on Port A, Bit 1 */

#define switch1Address 0x10
#define switch1Bit 0x04
/* Switch1 is on Port A, Bit 2 */

#define switch2Address 0x10
#define switch2Bit 0x08
/* Switch2 is on Port A, Bit 3 */

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

#define displayRelayAddress 0x12
#define displayRelayBitOff 0x20
#define displayRelayBitOn 0xDF
/* displayRelay is on Port C, Bit 5 */

#define motorRelayAddress 0x12
#define motorRelayBitOff 0x10
#define motorRelayBitOn 0xEF
/* motoryRelay is on Port C, Bit 4 */

#define encoderLeftAddress 0x12
#define encoderLeftBit 0x01
/* encoder Left is on Port C, Bit 0 */

#define encoderRightAddress 0x12
#define encoderRightBit 0x08
/* encoder Right is on Port C, Bit 3 */
/* This has to be changed in hardware to Port C, Bit 2 !!!!!!!!!!!
 * And Bit 3 has to be tied to Counter 2 Out (pin 50) instead of Counter
 * instead of Counter 1 Out, that already can generate an interrupt !!!!! */

#define encoderDirAddress 0x12
#define encoderDirBit 0x02
/* encoder Left is on Port C, Bit 1 */

#define boardPoti 0x3
/* Board poti is input 3 */

#define externalPoti 0x2
/* External poti is input 2 */

#define unipolarADC 0x0
#define bipolarADC 0x8
#define rangeADC5V 0x0
#define rangeADC10V 0x10
#define acqADCStart 0x20
#define acqADCStop 0x0

#define adcDataReg 0x02
#define adcCntrlReg 0x02
#define adcStatusReg 0x0
#define adcStatusMask 0x80

#define dacRefEnReg 0x18
#define dacRefEnBit 0x01
#define dacOutMotor 0x04

#define cnt0Address 0x0C
#define cnt1Address 0x0D
#define cnt2Address 0x0E
#define cntCntrlReg 0x0F

#define cnt0Sel 0x00
#define cnt1Sel 0x40
#define cnt2Sel 0x80

#define latchSel 0x00
#define lowByteSel 0x10
#define highByteSel 0x20
#define bothByteSel 0x30

#define cntMode0 0x00
#define cntMode2 0x04
#define cntMode4 0x08

#define aioIRQNum 5
#define aioIntNum 0x25
/* IRQ5 corresponds to int number 20 + 5 */

#define intAllEnBit 0x04
#define intPortC0Bit 0x08
#define intPortC3Bit 0x10
#define intCntEnBit 0x20

#define intEnAddress 0x01

#define intClearAddress 0x00

#define wdPeriodAddress 0x443
#define wdRefreshAddress 0x443
#define wdDisableAddress 0x843

#endif /* __INChwh */
