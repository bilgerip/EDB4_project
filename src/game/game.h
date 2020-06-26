/*
 * game.h
 *
 *  Created on: 25.06.2020
 *      Author: Patrick Bilgeri
 */

#ifndef SRC_GAME_GAME_H_
#define SRC_GAME_GAME_H_

typedef struct{
	uint8_t* buff;
	uint16_t size;
}presentationpacket_t;

typedef enum{
	REGISTER_PLAYER = 0x01,
	PLAYER_CONTROLL = 0x10,
	DROP_FOOD = 0x20,
	REQUEST_PLAYER = 0x30,
	CHAT_MESSAGE = 0x40
}CommandID_e;

presentationpacket_t* createplayer(char* name, uint16_t transectionID);
presentationpacket_t* creategamepacket(uint16_t version, CommandID_e type, uint16_t transectionID, uint16_t packetlenght);
presentationpacket_t* createcontroll(bool up, bool down, bool left, bool right);
presentationpacket_t* dropfood();
presentationpacket_t* createmessage(char* message);
#endif /* SRC_GAME_GAME_H_ */
