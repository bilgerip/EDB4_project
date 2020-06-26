/*
 * communicator.c
 *
 *  Created on: 07.05.2020
 *      Author: Patrick Bilgeri
 */

#include "communicator.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define DEFAULT_SERVER_IP_ADDRESS					("195.34.89.241")
#define DEFAULT_SERVER_PORT							(7)
#define DEFAULT_GAME_SERVER_IP_ADRESS				("52.57.105.0")
#define DEFAULT_GAME_SERVER_PORT					(44444)

static void cbNetworkReceive(uint8_t* pBuffer, uint32_t len);
static void write32Bufmsb(uint8_t* pBuf, uint32_t data);
static void write32Bufmsb(uint8_t* pBuf, uint32_t data);
static uint16_t Buf16BitToInt(uint8_t* pBuf);
static uint32_t Buf32BitToInt(uint8_t* pBuf);
static uint16_t generatHash(uint8_t* pnouce, uint32_t nlenght, uint8_t* psecret, uint32_t slenght);
static uint16_t generateHMAC(uint8_t* pbuff, uint32_t lenght);
static sendPacket_t* createPacket(uint16_t version, Types_e type, uint16_t sessionID, uint16_t sequenzenb, uint32_t lenghtpayload);

static uint16_t sessionID;
static uint16_t sequenznbtx;
static uint8_t secret[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x01};
static uint16_t cresp;

void write16Bufmsb(uint8_t* pBuf, uint16_t data){
	pBuf[0] = (data & 0xFF00) >> 8;
	pBuf[1] = data & 0xFF;
}

void write32Bufmsb(uint8_t* pBuf, uint32_t data){
	pBuf[0] = (data & 0xFF000000) >> 24;
	pBuf[1] = (data & 0x00FF0000) >> 16;
	pBuf[2] = (data & 0x0000FF00) >> 8;
	pBuf[3] = (data & 0x000000FF);
}

uint16_t Buf16BitToInt(uint8_t* pBuf){
	uint16_t data = (pBuf[0] << 8) | pBuf[1];
	return data;
}

uint32_t Buf32BitToInt(uint8_t* pBuf){
	uint32_t data = (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | pBuf[3];
	return data;
}

int communicator_connect(Server_e server) {
	if (!network_init(cbNetworkReceive)) {
		return -1;
	}
	switch (server) {
	case MAIN_SERVER:
		return -3;
		break;
	case TEST_SERVER:
		if (!network_connect(DEFAULT_SERVER_IP_ADDRESS, DEFAULT_SERVER_PORT)) {
			return -2;
		}
		break;
	case GAME_SERVER:
		if (!network_connect(DEFAULT_GAME_SERVER_IP_ADRESS,
				DEFAULT_GAME_SERVER_PORT)) {
			return -2;
		}
		break;
	default:
		return -3;
		break;
	}

	return 0;
}

void communicator_createSession() {
	sendPacket_t* pcs = createPacket(0, REQUEST_SESSION, 0, 0, 0);
	network_send(pcs->pbuff, pcs->size);
	while (!sessionID) {
		Sleep(10);
	}
	//..wait untill session is crated...
}

static void cbNetworkReceive(uint8_t* pBuffer, uint32_t len) {
	printf("Packet received with len=%d.\r\n", len);

	for (int i = 0; i < len; i++) {
		printf("%02X  ", pBuffer[i]);
	}
	printf("\n");
	if (pBuffer[0] == 0x04) {
		cresp = generatHash(&pBuffer[7], 4, secret, 8);
		sendPacket_t* pcr = createPacket(0,RESPONSE_SESSION,0,0,4);
		write16Bufmsb(&pcr->pbuff[9], cresp);
		network_send(pcr->pbuff, pcr->size);
	}
	if (pBuffer[0] == 0x10) {
		sessionID = (pBuffer[3] << 8) | pBuffer[4];
		printf("%d\n", sessionID);
	}
}

void heartbeat(){
	sendPacket_t* phb = createPacket(0,HEARTBEAT_SESSION,sessionID,sequenznbtx,0);
		uint16_t hmac = generateHMAC(phb->pbuff, phb->size-2);
		write16Bufmsb(&phb->pbuff[7], hmac);
		network_send(phb->pbuff, phb->size);
		free(phb->pbuff);
		free(phb);
}

void sendapplicationmessage(uint8_t* buff, uint32_t lenght){
	sendPacket_t* pam = createPacket(0,APPLICATION_SESSION,sessionID, sequenznbtx,lenght);
	memcpy(&pam->pbuff[7],buff,lenght);
	uint16_t hmac = generateHMAC(pam->pbuff, pam->size-2);
	write16Bufmsb(&pam->pbuff[pam->size-2], hmac);
	network_send(pam->pbuff, pam->size);
	free(pam->pbuff);
	free(pam);
}

void sessioninvallidate(){
	sendPacket_t* psi = createPacket(0,INVALIDATE_SESSION,sessionID, sequenznbtx,0);
	uint16_t hmac = generateHMAC(psi->pbuff, psi->size-2);
	write16Bufmsb(&psi->pbuff[7], hmac);
	network_send(psi->pbuff, psi->size);
	free(psi->pbuff);
	free(psi);
}

sendPacket_t* createPacket(uint16_t version, Types_e type, uint16_t sessionID, uint16_t sequenzenb, uint32_t lenghtpayload){
	sendPacket_t* ppacket = malloc(sizeof(sendPacket_t));
	if (ppacket== NULL){
			return 0;
		}
	ppacket->size = 9 + lenghtpayload;
	ppacket->pbuff = malloc(ppacket->size);
	if (ppacket->pbuff== NULL){
			return NULL;
		}
	memset(ppacket->pbuff, 0x00, ppacket->size);
	sessionFlags_t* pHeader = (sessionFlags_t*) &ppacket->pbuff[0];
	switch(type){
	case REQUEST_SESSION:
		pHeader->session_request=1;
		break;
	case RESPONSE_SESSION:
		pHeader->session_challenge_response = 1;
		ppacket->pbuff[2]=4;
		break;
	case HEARTBEAT_SESSION:
		pHeader->heartbeat = 1;
		write16Bufmsb(&ppacket->pbuff[3], sessionID);
		write16Bufmsb(&ppacket->pbuff[5], ++sequenznbtx);
		break;
	case APPLICATION_SESSION:
		write16Bufmsb(&ppacket->pbuff[1], lenghtpayload);
		write16Bufmsb(&ppacket->pbuff[3], sessionID);
		write16Bufmsb(&ppacket->pbuff[5], ++sequenznbtx);
		break;
	case INVALIDATE_SESSION:
		pHeader->session_invalidate=1;
		write16Bufmsb(&ppacket->pbuff[3], sessionID);
		write16Bufmsb(&ppacket->pbuff[5], ++sequenznbtx);

		break;
	default:
		break;
	}
	return ppacket;
}

uint16_t generatHash(uint8_t* pnouce, uint32_t nlenght, uint8_t* psecret, uint32_t slenght){
	uint32_t glenght = nlenght + slenght;
	uint8_t *pbuff = malloc(glenght);
	if (pbuff== NULL){
		return 0;
	}
	memcpy(pbuff, pnouce, nlenght);
	memcpy(&pbuff[4], psecret, slenght);
	uint16_t hash = generateCRC(pbuff, glenght);
	write16Bufmsb(pbuff, hash);
	uint16_t hash2 = generateCRC(pbuff, 2);
	return hash2;
}

uint16_t generateHMAC(uint8_t* pbuff, uint32_t lenght){
	uint16_t totallen = 8 + 2 + lenght;
	uint8_t* phmacbuff = malloc(totallen);
	if (phmacbuff == NULL){
			return 0;
		}
	write16Bufmsb(phmacbuff, cresp);
	memcpy(&phmacbuff[2], secret, 8);
	memcpy(&phmacbuff[10], pbuff, lenght);
	uint16_t hash = generateCRC(phmacbuff, totallen);
	write16Bufmsb(phmacbuff, hash);
	uint16_t hmac = generateCRC(phmacbuff, 2);
	return hmac;
}

uint16_t generateCRC(uint8_t* pBuf, uint32_t len) {
	uint16_t crc = 0xFFFF;
	for (int i = 0; i < len; i++) {
		crc = crc ^ pBuf[i];
		for (int j = 0; j < 8; j++) {
			if (crc & 0b1) {
				crc = crc >> 1;
				crc = crc ^ 0xA001;
			} else {
				crc = crc >> 1;
			}
		}
	}
	return crc;
}

