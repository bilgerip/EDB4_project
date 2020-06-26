/*
 * game.c
 *
 *  Created on: 25.06.2020
 *      Author: Patrick Bilgeri
 */
#include "../communicator/communicator.h"
#include "game.h"
#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include <string.h>
#include "../input/input_service.h"


static GAME_HEADER_LENGHT = 8;

presentationpacket_t* createplayer(char* name, uint16_t transectionID){
	uint8_t namelenght = strlen(name);
	uint32_t geslenght = 7 + 1 + 2 + namelenght;
	presentationpacket_t* pcp = creategamepacket(0, REGISTER_PLAYER, 0x5555, geslenght);
	pcp->buff[7] = 0x01; //Typ name
	write16Bufmsb(&pcp->buff[8],namelenght);
	memcpy(&pcp->buff[10],name,namelenght);
	return pcp;
}

presentationpacket_t* createcontroll(bool up, bool down, bool left, bool right){
	uint32_t geslenght = GAME_HEADER_LENGHT;
	presentationpacket_t* pcc = creategamepacket(0, PLAYER_CONTROLL, 0x5555, geslenght);
	pcc->buff[7]=0;
	if(up == true){
		pcc->buff[7] = (pcc->buff[7] | INPUT_KEY_MASK_KEY_UP);
	}
	if(right == true){
		pcc->buff[7] = (pcc->buff[7] | INPUT_KEY_MASK_KEY_RIGHT);
	}
	if(down == true){
		pcc->buff[7] = (pcc->buff[7] | INPUT_KEY_MASK_KEY_DOWN);
	}
	if(left == true){
		pcc->buff[7] = (pcc->buff[7] | INPUT_KEY_MASK_KEY_LEFT);
	}
	return pcc;
}

presentationpacket_t* dropfood(){
	uint32_t geslenght = GAME_HEADER_LENGHT;
	presentationpacket_t* pdf = creategamepacket(0, DROP_FOOD, 0x5555, geslenght);
	pdf->buff[7] = 0x00;

	return pdf;
}

presentationpacket_t* createmessage(char* message){
	uint16_t namelenght = strlen(message);
	uint32_t geslenght = GAME_HEADER_LENGHT - 1 + namelenght;
	presentationpacket_t* pmes = creategamepacket(0, CHAT_MESSAGE, 0x5555, geslenght);
	memcpy(&pmes->buff[7],message,geslenght);

	return pmes;

}

presentationpacket_t* creategamepacket(uint16_t version, CommandID_e type, uint16_t transectionID, uint16_t packetlenght){
	presentationpacket_t* ppacket = malloc(sizeof(presentationpacket_t));
	if (ppacket == NULL){
		return 0;
		}
	ppacket->size = packetlenght;
	ppacket->buff = malloc(ppacket->size);
	if (ppacket == NULL){
		return 0;
		}
	memset(ppacket->buff,0x00, ppacket->size);
	ppacket->buff[0]=0x01;
	switch(type){
	case REGISTER_PLAYER:
		write16Bufmsb(&ppacket->buff[1],ppacket->size-7);
		write16Bufmsb(&ppacket->buff[3],REGISTER_PLAYER);
		write16Bufmsb(&ppacket->buff[5], transectionID);
		break;
	case PLAYER_CONTROLL:
		write16Bufmsb(&ppacket->buff[1],ppacket->size-7);
		write16Bufmsb(&ppacket->buff[3],PLAYER_CONTROLL);
		write16Bufmsb(&ppacket->buff[5], transectionID);
		break;
	case DROP_FOOD:
		write16Bufmsb(&ppacket->buff[1],ppacket->size-7);
		write16Bufmsb(&ppacket->buff[3],DROP_FOOD);
		write16Bufmsb(&ppacket->buff[5], transectionID);
		break;
	case REQUEST_PLAYER:
		break;
	case CHAT_MESSAGE:
		write16Bufmsb(&ppacket->buff[1],ppacket->size-7);
		write16Bufmsb(&ppacket->buff[3],CHAT_MESSAGE);
		write16Bufmsb(&ppacket->buff[5], transectionID);
		break;
	}
	return ppacket;
}

