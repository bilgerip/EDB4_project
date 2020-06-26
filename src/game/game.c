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

presentationpacket_t* createplayer(char* name, uint16_t transectionID){
	uint8_t namelenght = strlen(name);
	uint32_t geslenght = 7 + 1 + 2 + namelenght;
	presentationpacket_t* pcp = creategamepacket(0, REGISTER_PLAYER, 0x5555, geslenght);
	//vor switch
	/*presentationpacket_t* ppacket = malloc(sizeof(presentationpacket_t));
	ppacket->size = geslenght;
	ppacket->buff = malloc(ppacket->size);
	memset(ppacket->buff,0x00, ppacket->size);
	ppacket->buff[0]= 0x01;
	//je nach packet: ein case
	write16Bufmsb(&ppacket->buff[1],ppacket->size-7);
	write16Bufmsb(&ppacket->buff[3],REGISTER_PLAYER);
	write16Bufmsb(&ppacket->buff[5], transectionID);*/

	//bleibt hier (payload befüllen)
	pcp->buff[7] = 0x01; //Typ name
	write16Bufmsb(&pcp->buff[8],namelenght);
	memcpy(&pcp->buff[10],name,namelenght);
	return pcp;

}
presentationpacket_t* creategamepacket(uint16_t version, CommandID_e type, uint16_t transectionID, uint16_t packetlenght){
	presentationpacket_t* ppacket = malloc(sizeof(presentationpacket_t));
	ppacket->size = packetlenght;
	ppacket->buff = malloc(ppacket->size);
	memset(ppacket->buff,0x00, ppacket->size);
	presentationpacket_t* pHeader = (presentationpacket_t*) &ppacket.buff[0];
	switch(type){
	case REGISTER_PLAYER:
		write16Bufmsb(&ppacket->buff[1],ppacket->size-7);
		write16Bufmsb(&ppacket->buff[3],REGISTER_PLAYER);
		write16Bufmsb(&ppacket->buff[5], transectionID);
		break;
	case PLAYER_CONTROLL:
		break;
	case DROP_FOOD:
		break;
	case REQUEST_PLAYER:
		break;
	case CHAT_MESSAGE:
		break;
	}
}

