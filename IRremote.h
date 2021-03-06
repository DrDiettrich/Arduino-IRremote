
//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Copyright 2009 Ken Shirriff
// For details, see http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
// Edited by Mitra to add new controller SANYO
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// LG added by Darryl Smith (based on the JVC protocol)
// Whynter A/C ARC-110WD added by Francesco Meschia
//******************************************************************************

#ifndef IRremote_h
#define IRremote_h

//------------------------------------------------------------------------------
// The ISR header contains several useful macros the user may wish to use
//
#include "IRremoteInt.h"

//------------------------------------------------------------------------------
// Supported IR protocols
// Each protocol you include costs memory and, during decode, costs time
// Disable (set to 0) all the protocols you do not need/want!
//
#define DECODE_RC5           1
#define SEND_RC5             1

#define DECODE_RC6           1
#define SEND_RC6             1

#define DECODE_NEC           1
#define SEND_NEC             1

#define DECODE_SONY          1
#define SEND_SONY            1

#define DECODE_PANASONIC     1
#define SEND_PANASONIC       1

#define DECODE_JVC           1
#define SEND_JVC             1

#define DECODE_SAMSUNG       1
#define SEND_SAMSUNG         1

#define DECODE_WHYNTER       1
#define SEND_WHYNTER         1

#define DECODE_AIWA_RC_T501  1
#define SEND_AIWA_RC_T501    1

#define DECODE_LG            1
#define SEND_LG              1

#define DECODE_SANYO         1
#define SEND_SANYO           0 // NOT WRITTEN

#define DECODE_MITSUBISHI    1
#define SEND_MITSUBISHI      0 // NOT WRITTEN

#define DECODE_DISH          0 // NOT WRITTEN
#define SEND_DISH            1

#define DECODE_SHARP         0 // NOT WRITTEN
#define SEND_SHARP           1

#define DECODE_DENON         1
#define SEND_DENON           1

#define DECODE_PRONTO        0 // This function doe not logically make sense
#define SEND_PRONTO          1

//------------------------------------------------------------------------------
// When sending a Pronto code we request to send either the "once" code
//                                                   or the "repeat" code
// If the code requested does not exist we can request to fallback on the
// other code (the one we did not explicitly request)
//
// I would suggest that "fallback" will be the standard calling method
// The last paragraph on this page discusses the rationale of this idea:
//   http://www.remotecentral.com/features/irdisp2.htm
//
#define PRONTO_ONCE        false
#define PRONTO_REPEAT      true
#define PRONTO_FALLBACK    true
#define PRONTO_NOFALLBACK  false

//------------------------------------------------------------------------------
// An enumerated list of all supported formats
// You do NOT need to remove entries from this list when disabling protocols!
//
typedef
	enum {
		UNKNOWN      = -1,
		UNUSED       =  0,
		RC5,
		RC6,
		NEC,
		SONY,
		PANASONIC,
		JVC,
		SAMSUNG,
		WHYNTER,
		AIWA_RC_T501,
		LG,
		SANYO,
		MITSUBISHI,
		DISH,
		SHARP,
		DENON,
		PRONTO,
	}
decode_type_t;

//------------------------------------------------------------------------------
// Set DEBUG to 1 for lots of lovely debug output
//
#define DEBUG  0

//------------------------------------------------------------------------------
// Debug directives
//
#if DEBUG
#	define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
#	define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
#	define DBG_PRINT(...)
#	define DBG_PRINTLN(...)
#endif

//------------------------------------------------------------------------------
// Mark & Space matching functions
//
int  MATCH       (int measured, int desired) ;
int  MATCH_MARK  (int measured_ticks, int desired_us) ;
int  MATCH_SPACE (int measured_ticks, int desired_us) ;

//------------------------------------------------------------------------------
// Results returned from the decoder
//
typedef unsigned long IRvalue_t;
struct decode_results
{
	public:
		IRvalue_t              value;        // Decoded value [max 32-bits]
		//volatile
		unsigned int  *rawbuf;      // Raw intervals in 50uS ticks
		unsigned int           address;      // Used by Panasonic & Sharp [16-bits]
		decode_type_t          decode_type;  // UNKNOWN, NEC, SONY, RC5, ...
		byte                    rawlen;       // Number of records in rawbuf
		byte                    bits;         // Number of bits in decoded value
		bool                    overflow;     // true iff IR raw code too long
};

//------------------------------------------------------------------------------
// Decoded value for NEC when a repeat code is received
//
#define REPEAT 0xFFFFFFFF

//------------------------------------------------------------------------------
// Main class for receiving IR
//
class IRrecv;
typedef bool (IRrecv::*decoder_t)();

// Macros for available()
//
#define irNEC	&IRrecv::decodeNEC
#define irSony	&IRrecv::decodeSony
#define irHash  &IRrecv::decodeHash
#define irRC5 &IRrecv::decodeRC5
#define irRC6 &IRrecv::decodeRC6
#define irPana &IRrecv::decodePanasonic
#define irJVC &IRrecv::decodeJVC
#define irSamsung &IRrecv::decodeSAMSUNG
#define irWhynter &IRrecv::decodeWhynter
#define irRCT501 &IRrecv::decodeAiwaRCT501
#define irLG &IRrecv::decodeLG
#define irSanyo &IRrecv::decodeSanyo
#define irMitsubishi &IRrecv::decodeMitsubishi
//#define  &IRrecv::decodeDish
//#define  &IRrecv::decodeSharp
#define irDenon &IRrecv::decodeDenon

class IRrecv
{
	public:
		decode_results results;
		IRrecv (int recvpin) ;
		IRrecv (int recvpin, int blinkpin);

		bool available();

		void begin(bool blinkflag);
		bool available(decoder_t which);
		IRvalue_t read() { return results.value; }

		void  blink13    (bool blinkflag) ;
		bool  decode     () ;
		bool  decode     (decode_results *copy) ;
		void  enableIRIn ( ) ;
		bool  isIdle     ( ) ;
		void  resume     ( ) ;

	private:
		int   compare    (unsigned int oldval, unsigned int newval) ;
		// This helper function is shared by RC5 and RC6
		int  getRClevel (int *offset,  int *used,  int t1) ;

	public:
		bool  decodeHash 			 () ;
		bool  decodeRC5        () ;
		bool  decodeRC6        () ;
		bool  decodeNEC        () ;
		bool  decodeSony       () ;
		bool  decodePanasonic  () ;
		bool  decodeJVC        () ;
		bool  decodeSAMSUNG    () ;
		bool  decodeWhynter    () ;
		bool  decodeAiwaRCT501 () ;
		bool  decodeLG         () ;
		bool  decodeSanyo      () ;
		bool  decodeMitsubishi () ;
		//bool  decodeDish () ; // NOT WRITTEN
		//bool  decodeSharp () ; // NOT WRITTEN
		bool  decodeDenon () ;
} ;

//------------------------------------------------------------------------------
// Main class for sending IR
//
class IRsend
{
	public:
		IRsend () { }

		void  custom_delay_usec (unsigned long uSecs);
		void  enableIROut 		(int khz) ;
		void  mark        		(unsigned int usec) ;
		void  space       		(unsigned int usec) ;
		void  sendRaw     		(const unsigned int buf[],  unsigned int len,  unsigned int hz) ;

		//......................................................................
#		if SEND_RC5
			void  sendRC5        (unsigned long data,  int nbits) ;
#		endif
#		if SEND_RC6
			void  sendRC6        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_NEC
			void  sendNEC        (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_SONY
			void  sendSony       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_PANASONIC
			void  sendPanasonic  (unsigned int address,  unsigned long data) ;
#		endif
		//......................................................................
#		if SEND_JVC
			// JVC does NOT repeat by sending a separate code (like NEC does).
			// The JVC protocol repeats by skipping the header.
			// To send a JVC repeat signal, send the original code value
			//   and set 'repeat' to true
			void  sendJVC        (unsigned long data,  int nbits,  bool repeat) ;
#		endif
		//......................................................................
#		if SEND_SAMSUNG
			void  sendSAMSUNG    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_WHYNTER
			void  sendWhynter    (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_AIWA_RC_T501
			void  sendAiwaRCT501 (int code) ;
#		endif
		//......................................................................
#		if SEND_LG
			void  sendLG         (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_SANYO
			void  sendSanyo      ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_MISUBISHI
			void  sendMitsubishi ( ) ; // NOT WRITTEN
#		endif
		//......................................................................
#		if SEND_DISH
			void  sendDISH       (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_SHARP
			void  sendSharpRaw   (unsigned long data,  int nbits) ;
			void  sendSharp      (unsigned int address,  unsigned int command) ;
#		endif
		//......................................................................
#		if SEND_DENON
			void  sendDenon      (unsigned long data,  int nbits) ;
#		endif
		//......................................................................
#		if SEND_PRONTO
			void  sendPronto     (char* code,  bool repeat,  bool fallback) ;
#		endif
} ;

#endif
