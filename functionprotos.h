// Definitionen der Sende- und Empfangszustände
#define 	FIRE			'f'
#define 	IDENT			'i'
#define 	REPEAT			'r'
#define 	ERROR			'e'
#define 	PARAMETERS		'p'
#define 	ACKNOWLEDGED	'a'
#define 	SETUP			's'
#define 	IDLE			0


// Bitflags
typedef union{
	struct{
	unsigned uart_active :1;
	unsigned uart_config :1;
	unsigned fire :1;
	unsigned send :1;
	unsigned transmit :1;
	unsigned receive :1;
	unsigned list :1;
	unsigned lcd_update:1;
	unsigned tx_post:1;
	unsigned rx_post:1;
	unsigned show_only:1;
	unsigned reset_fired:1;
	unsigned reset_device:1;
	unsigned clear_list:1;
	unsigned temp:1;
	unsigned : 1;
	} b;
	uint16_t complete;
} bitfeld_t;
