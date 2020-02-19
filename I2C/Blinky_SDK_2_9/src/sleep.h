

void sleep(void);

extern int sleep_block_counter[5];



typedef enum {
	EM0,
	EM1,
	EM2,
	EM3,
	EM4
} sleepstate_enum;

extern void blockSleepmode(sleepstate_enum minimumMode);

extern void unblockSleepmode(sleepstate_enum minimumMode);

