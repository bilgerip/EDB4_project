/*
 * communicator.h
 *
 *  Created on: 07.05.2020
 *      Author: Patrick Bilgeri
 */

#ifndef SRC_COMMUNICATOR_COMMUNICATOR_H_
#define SRC_COMMUNICATOR_COMMUNICATOR_H_

#include"../network/network.h"
#include<stdbool.h>

typedef enum{
	TEST_SERVER,
	MAIN_SERVER,
	GAME_SERVER
}Server_e;

typedef enum{
	HEARTBEAT_SESSION,
	REQUEST_SESSION,
	RESPONSE_SESSION,
	INVALIDATE_SESSION,
	APPLICATION_SESSION

}Types_e;

typedef struct {
	uint8_t heartbeat: 1;
	uint8_t session_request: 1;
	uint8_t session_challenge : 1;
	uint8_t session_challenge_response: 1;
	uint8_t session_result : 1;
	uint8_t session_invalidate : 1;
	uint8_t version : 2;

}sessionFlags_t;

typedef struct {
	uint32_t size;
	uint8_t *pbuff;
}sendPacket_t;

int communicator_connect(Server_e);
uint16_t generateCRC(uint8_t* pBuf, uint32_t len);
void communicator_createSession();
void heartbeat();
void sendapplicationmessage(uint8_t* buff, uint32_t lenght);
void sessioninvallidate();
#endif /* SRC_COMMUNICATOR_COMMUNICATOR_H_ */
