//
// Created by alexj on 5/1/2018.
//

#ifndef _PICARD_TYPES_H_
#define _PICARD_TYPES_H_


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ERR_UNK_CMD     (char)  0xFF
#define ERR_N_PARAMS    (char)  0XFE

#define CMD_CONNECT		"CONNECTA"
#define CMD_DISCONNECT	"DESCONNECTA"
#define	CMD_SHOW		"MOSTRA"
#define CMD_MENU		"MENU"
#define CMD_ORDER		"COMANDA"
#define CMD_REQUEST		"DEMANA"
#define CMD_REMOVE		"ELIMINA"
#define CMD_PAY			"PAGAR"

#define	MSG_INIT_CONN			"Connectant amb LsEat...\n"
#define	MSG_CONN_RECOVERY		"Iniciant recuperació automàtica...\n"
#define	MSG_CONN_DATA_OK		ANSI_COLOR_CYAN "[Connexió amb Data OK]\n" ANSI_COLOR_RESET
#define	MSG_CONN_DATA_KO		ANSI_COLOR_RED "[Connexió amb Data KO]\n" ANSI_COLOR_RESET
#define	MSG_CONN_ENT_OK			ANSI_COLOR_CYAN "[Connexió amb Enterprise OK]\n" ANSI_COLOR_RESET
#define	MSG_CONN_ENT_KO			ANSI_COLOR_RED "[Connexió amb Enterprise KO]\n" ANSI_COLOR_RESET
#define	MSG_CONN_OK				"Connexió realitzada!\n"
#define MSG_CONN_LOSS			ANSI_COLOR_RED "[Comanda KO]" ANSI_COLOR_RESET
#define	MSG_CMD_KO				ANSI_COLOR_RED "[Comanda KO]" ANSI_COLOR_RESET
#define	MSG_MENU_OK				ANSI_COLOR_CYAN "[Menu disponible]\n" ANSI_COLOR_RESET
#define	MSG_MENU_KO				ANSI_COLOR_RED "[Menu no disponible]\n" ANSI_COLOR_RESET
#define	MSG_SHOW_ORDER_OK		ANSI_COLOR_CYAN "[Comanda actual]\n" ANSI_COLOR_RESET
#define	MSG_SHOW_ORDER_KO		ANSI_COLOR_RED "[No hi ha comanda]\n" ANSI_COLOR_RESET
#define	MSG_ORDER_OK			ANSI_COLOR_CYAN "[Petició OK]" ANSI_COLOR_RESET
#define	MSG_ORDER_KO			ANSI_COLOR_RED "[Petició KO]" ANSI_COLOR_RESET
#define	MSG_PAYMENT_OK			ANSI_COLOR_CYAN "[Pagament OK]" ANSI_COLOR_RESET
#define	MSG_PAYMENT_KO			ANSI_COLOR_RED "[Pagament KO]" ANSI_COLOR_RESET

#define CODE_CONNECT		(char)  0x01
#define CODE_DISCONNECT		(char)  0x02
#define CODE_SHOWMENU		(char)  0x03
#define CODE_REQUEST		(char)  0x04
#define CODE_REMOVE			(char)  0x05
#define CODE_PAYMENT		(char)  0x06
#define CODE_SHOWORDER		(char)	0x07

#define	HEADER_INIT_CONN		"PIC_NAME"
#define	HEADER_END_CONN			"PIC_NAME"
#define	HEADER_PIC_DATA_OK		"ENT_INF"
#define	HEADER_MENU_REQ			"SHW_MENU"
#define	HEADER_MENU_END			"END_MENU"
#define	HEADER_REQ_DISH			"NEW_ORD"
#define	HEADER_DEL_DISH			"DEL_ORD"
#define	HEADER_ORDER_OK			"ORDOK"
#define	HEADER_ORDER_KO			"ORDKO"
#define	HEADER_PAYMENT			"PAY"
#define HEADER_PAYMENT_OK		"PAYOK"
#define HEADER_PAYMENT_KO		"PAYKO"

#define DATA_DISH_NOT_STOCK		1
#define DATA_DISH_NOT_FOUND		2


typedef struct {
	char* name;
	int price;
	int stock;
} Dish;

typedef struct {
	char code;
	int unitats;
	char *plat;
} Command;

typedef struct {
	char *name;
	char *ip;
	int port;
} Enterprise;

typedef struct {
	char *ip;
	int port;
} Socket;

typedef struct {
	char *name;
	int money;
	char *ip;
	int port;
} Config;

typedef struct {
	Dish *menu;
	int quantity;
} Menu;


#endif
