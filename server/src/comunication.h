#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "players.h"


int server_receive_instruction(int client_socket);
int server_receive_setting(Player** players_array, int player);
char* server_receive_stdpayload(Player** players_array, int player);
void server_send_stdmessage(Player** players_array, int player, int pkg_id, int size, char* message);