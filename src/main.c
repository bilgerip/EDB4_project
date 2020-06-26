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

	if(m == INPUT_KEY_MASK_KEY_SPACE){
		presentationpacket_t* pdrop = dropfood();
		sendapplicationmessage(pdrop->buff, pdrop->size);

	}


		bool up = (m & INPUT_KEY_MASK_KEY_UP);
		bool down = (m & INPUT_KEY_MASK_KEY_DOWN);
		bool left = (m & INPUT_KEY_MASK_KEY_LEFT);
		bool right = (m & INPUT_KEY_MASK_KEY_RIGHT);
		if (!(((up == true) && (down == true)) || ((left == true) && (right == true)))) {
		presentationpacket_t* pcontroll = createcontroll(up,right,down,left);
		sendapplicationmessage(pcontroll->buff, pcontroll->size);
		}


	/*if (m == INPUT_KEY_MASK_KEY_RIGHT) {
		uint8_t up = 0;
		uint8_t down = 0;
		uint8_t left = 0;
		uint8_t right = 2;
		presentationpacket_t* pcontroll = createcontroll(up,down,left,right);
		sendapplicationmessage(pcontroll->buff, pcontroll->size);
   	}

	if (m == INPUT_KEY_MASK_KEY_DOWN) {
		uint8_t up = 0;
		uint8_t down = 3;
		uint8_t left = 0;
		uint8_t right = 0;
		presentationpacket_t* pcontroll = createcontroll(up,down,left,right);
		sendapplicationmessage(pcontroll->buff, pcontroll->size);

   	}

	if (m == INPUT_KEY_MASK_KEY_LEFT) {
		uint8_t up = 0;
		uint8_t down = 0;
		uint8_t left = 4;
		uint8_t right = 0;
		presentationpacket_t* pcontroll = createcontroll(up,down,left,right);
		sendapplicationmessage(pcontroll->buff, pcontroll->size);

   	}
*/

}
