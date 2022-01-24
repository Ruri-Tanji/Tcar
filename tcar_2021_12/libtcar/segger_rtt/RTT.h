/*
 *    RTT.h
 *    Definition of SEGGER real-time transfer(SEGGER RTT)
 */

/*
 * RTT control block  (This is the RTT specification)
 */
typedef struct {
	const B	*sName;		// Optional name. 
	B	*pBuffer;	// Pointer to start of buffer
	UW	SizeOfBuffer;	// Buffer size in bytes. Note that one byte is lost, as this implementation does not fill up the buffer in order to avoid the problem of being unable to distinguish between full and empty.
	_UW	WrOff;		// Position of next item to be written by either target.
	_UW	RdOff;		// Position of next item to be read by host. Must be volatile since it may be modified by host.
	UW	Flags;		// Contains configuration flags
} RTT_BUFFER;

#define RTT_MAX_NUM_UP_BUFFERS		(1)	// Max. number of up-buffers (T->H) available on this target    (Default: 2)
#define RTT_MAX_NUM_DOWN_BUFFERS	(1)	// Max. number of down-buffers (H->T) available on this target  (Default: 2)

typedef struct {
	B		acID[16];				// Initialized to "SEGGER RTT"
	INT		MaxNumUpBuffers;			// Initialized to RTT_MAX_NUM_UP_BUFFERS
	INT		MaxNumDownBuffers;			// Initialized to RTT_MAX_NUM_DOWN_BUFFERS
	RTT_BUFFER	aUp[RTT_MAX_NUM_UP_BUFFERS];		// Up buffers, transferring information up from target via debug probe to host
	RTT_BUFFER	aDown[RTT_MAX_NUM_DOWN_BUFFERS];	// Down buffers, transferring information down from host via debug probe to target
} RTT_CB;
