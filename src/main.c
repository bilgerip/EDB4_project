#include "input/input_service.h"
#include "windows.h"
#include <stdio.h>
#include "communicator/communicator.h"
#include "game/game.h"
#include <time.h>

static bool gRunning = true;
static void cbInputHandler(InputKeyMask_t);

int main(int argc, char** argv) {

	uint32_t sekunden = time(NULL);
	uint32_t lastheart= sekunden;
	setbuf(stdout, NULL);

	printf("%d\n\r",sizeof(sessionFlags_t));

	input_service_init(cbInputHandler);
	communicator_connect(GAME_SERVER);
	communicator_createSession();
	printf("Session created\n");
	heartbeat();
	printf("Heartbeat send\n");
	presentationpacket_t* pplayer = createplayer("Patrick",0x5555);
	sendapplicationmessage(pplayer->buff, pplayer->size);

	while(gRunning) {
		sekunden= time(NULL);
		if((sekunden-lastheart)>10){
			heartbeat();
			lastheart = sekunden;
		}
		//Sleep(1000);
	}
	printf("Exiting...");
	return 0;
}

static void cbInputHandler(InputKeyMask_t m) {
	printf("Input event: %d.\r\n", m);
	if (m == INPUT_KEY_MASK_KEY_ESC) {
		sessioninvallidate();
		gRunning = false;
   	}

	if(m & INPUT_KEY_MASK_KEY_SPACE){
		uint8_t test[3] = { 0, 1, 2 };
		network_send(test, 3);
		printf("Packet sent.\r\n");
	}

	if (m == INPUT_KEY_MASK_KEY_UP) {
		bool up = true;
   	}

	if (m == INPUT_KEY_MASK_KEY_DOWN) {
		bool down = true;
   	}

	if (m == INPUT_KEY_MASK_KEY_LEFT) {
		bool left = true;
   	}

	if (m == INPUT_KEY_MASK_KEY_RIGHT) {
		bool right = true;
   	}
}
