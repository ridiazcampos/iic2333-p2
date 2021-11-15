#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include "comunication.h"
#include "conection.h"
#include "players.h"


void next_player(Player** players, int* attention)
{
    int next = (*attention + 1) % 4;
    while (players[next]->status == -1)
    {
        next = (next + 1) % 4;
    }
    *attention = next;
}

int check_winner(Player** players_info)
{
    int count = 0;
    int winner;
    for (int i = 0; i < 4; i++)
    {
        if (players_info[i]->status == 1)
        {
            winner = i;
            count++;
        }
    }
    if (count == 1)
    {
        server_send_stdmessage(players_info, winner, 13, 0, (void*) NULL);
        return 1;
    }
    else
    {
        return 0;
    }
}


int main(int argc, char *argv[]){
    // Se define una IP y un puerto
    char* IP;
    int PORT;
    if (argc != 5)
    {
        printf("El uso es ./client -i <ip_address> -p <port>\n");
        return 0;
    }
    else if (strcmp(argv[1], "-i") == 0 && strcmp(argv[3], "-p") == 0)
    {
        IP = argv[2];
        PORT = atoi(argv[4]);
    }
    else if (strcmp(argv[1], "-p") == 0 && strcmp(argv[3], "-i") == 0)
    {
        IP = argv[4];
        PORT = atoi(argv[2]);
    }
    else
    {
        printf("El uso es ./client -i <ip_address> -p <port>\n");
        return 0;
    }

    // Se crea el servidor y se obtienen los sockets de ambos clientes.
    pthread_t thread;
    Lock* lock = malloc(sizeof(lock));
    lock->value = 0;
    socklen_t* addr_size = malloc(sizeof(socklen_t));
    Args* arguments = malloc(sizeof(Args));
    Player** players_info = prepare_sockets_and_get_clients(IP, PORT, addr_size, &thread, lock, arguments);
    // Le enviamos al primer cliente un mensaje de bienvenida
    //char * welcome = "Bienvenido Cliente 1!!";
    //server_send_message(players_info->socket_c1, 1, welcome);
    char array[1];
    array[0] = 1;
    server_send_stdmessage(players_info, 0, 0, 1, &array[0]);

    int started = 0;
    int leader_socket;
    int leader_msg_code;
    char* client_payload;
    int num = 0;

    int everyone_is_ready = 0;

    int count = 0;


    while (started == 0)
    {
       
        leader_socket = players_info[0]->socket;
        leader_msg_code = server_receive_instruction(leader_socket);
        client_payload = server_receive_stdpayload(players_info, 0);

        

        if (leader_msg_code == 11) {

            count = 0;
           for (int player = 0; player < 4; player++)
            {

                if (players_info[player]->status == 0)
                {
                    count++;
                    char message[1];
                    message[0] = 1;
                    // server_send_stdmessage(players_info, 0, 16, 1, &message[0]);
                    // everyone_is_ready = 0;
                    break;
                }
                if (players_info[player]->status == 1)
                {
                    count++;
                }
            }



                char message[1];
                message[0] = count;
                server_send_stdmessage(players_info, 0, 16, 1, &message[0]);
              
                
        }

        
        else if (leader_msg_code == 1)
        {
            acquire(lock);
            int count = 0;
            int everyone_is_ready = 1;
            for (int player = 0; player < 4; player++)
            {
                if (players_info[player]->status == 0)
                {
                    count++;
                    char message[1];
                    message[0] = 1;
                    server_send_stdmessage(players_info, 0, 1, 1, &message[0]);
                    everyone_is_ready = 0;
                    break;
                }
                if (players_info[player]->status == 1)
                {
                    count++;
                }
            }
            if (count > 1 && everyone_is_ready)
            {
                // cancelar el thread corriendo
                // Que comience el juego
                int rc = pthread_cancel(thread);
                while (rc)
                {
                    printf("%i\n", rc);
                    rc = pthread_cancel(thread);
                }
                char message[1];
                message[0] = 0;
                server_send_stdmessage(players_info, 0, 1, 1, &message[0]);
                started = 1;
            }
            else if (everyone_is_ready)
            {
                // Solo hay 1 jugador
                char message[1];
                message[0] = 2;
                server_send_stdmessage(players_info, 0, 1, 1, &message[0]);
            }
            release(lock);

        }
        else
        {
            printf("91: Nunca deberíamos llegar aquí \n");
        }
        free(client_payload);
    }

    int my_attention = 0;
    int msg_code;

    collect_resources(players_info[0]);
    while (1)
    {
        int socket = players_info[my_attention]->socket;
        server_send_stdmessage(players_info, my_attention, 15, 0, (char*) NULL);
        // Se obtiene el paquete del cliente my_attention
        msg_code = server_receive_instruction(socket);
        client_payload = server_receive_stdpayload(players_info, my_attention);


        if (players_info[my_attention]->status == 0 && msg_code != 0)
        {
            // Nunca deberíamos llegar aquí
            // ERROR: no se puede enviar una instrucción si el jugador no está iniciado
            printf("SERVER/MAIN109: Nunca deberíamos llegar aquí, msg_code %i \n", msg_code);
        }

        if (msg_code == 1)
        {
            // Nunca deberíamos llegar aquí
            printf("SERVER/MAIN115: Nunca deberíamos llegar aquí, msg_code %i \n", msg_code);
        }

        if (msg_code == 0)
        {
            printf("SERVER/MAIN120: Nunca deberíamos llegar aquí, msg_code %i \n", msg_code);
        }

        else if (msg_code == 2)
        {
            char* message = malloc((12 + 4 * 50));
            message[0] = players_info[my_attention] -> farmers;
            message[1] = players_info[my_attention] -> miners;
            message[2] = players_info[my_attention] -> engineers;
            message[3] = players_info[my_attention] -> warriors;
            message[4] = players_info[my_attention] -> gold;
            message[5] = players_info[my_attention] -> food;
            message[6] = players_info[my_attention] -> science;
            message[7] = players_info[my_attention] -> farmers_level;
            message[8] = players_info[my_attention] -> miners_level;
            message[9] = players_info[my_attention] -> engineers_level;
            message[10] = players_info[my_attention] -> attack_level;
            message[11] = players_info[my_attention] -> defense_level;
            for (int i = 0; i < 4; i++)
            {
                if (players_info[i] -> status == 1)
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message[12 + 50 * i + j] = players_info[i]->name[j];
                    }
                }
                else
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message[12 + 50 * i + j] = '\0';
                    }
                }
            }
            server_send_stdmessage(players_info, my_attention, 2, 12 + 4 * 50, &message[0]);
            free(client_payload);
            free(message);
        }

        else if (msg_code == 3)
        {
            int return_value = create_villager(players_info[my_attention], (int) client_payload[0]);
            char message3[1];
            message3[0] = return_value;
            server_send_stdmessage(players_info, my_attention, 3, 1, &message3[0]);
        }
        else if (msg_code == 4)
        {
            int return_value = level_up(players_info[my_attention], (int) client_payload[0]);
            char message4[1];
            message4[0] = return_value;
            server_send_stdmessage(players_info, my_attention, 4, 1, &message4[0]);
        }

        else if (msg_code == 5) //Atacar
        {
            free(client_payload);
            char* message5 = malloc((12 + 4 * 50));
            message5[0] = players_info[my_attention] -> farmers;
            message5[1] = players_info[my_attention] -> miners;
            message5[2] = players_info[my_attention] -> engineers;
            message5[3] = players_info[my_attention] -> warriors;
            message5[4] = players_info[my_attention] -> gold;
            message5[5] = players_info[my_attention] -> food;
            message5[6] = players_info[my_attention] -> science;
            message5[7] = players_info[my_attention] -> farmers_level;
            message5[8] = players_info[my_attention] -> miners_level;
            message5[9] = players_info[my_attention] -> engineers_level;
            message5[10] = players_info[my_attention] -> attack_level;
            message5[11] = players_info[my_attention] -> defense_level;
            for (int i = 0; i < 4; i++)
            {
                if (players_info[i] -> status == 1)
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message5[12 + 50 * i + j] = players_info[i]->name[j];
                    }
                }
                else
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message5[12 + 50 * i + j] = '\0';
                    }
                }
            }
            server_send_stdmessage(players_info, my_attention, 2, 12 + 4 * 50, message5);
            free(message5);

            msg_code = server_receive_instruction(socket);
            if (msg_code != 5)
            {
                printf("SERVER/MAIN190: Nunca deberiamos llegar aquí");
            }
            client_payload = server_receive_stdpayload(players_info, my_attention);
            
            int defender = ((uint8_t) client_payload[0]) - 1;
            free(client_payload);
            int return_value;
            // Modificar: puede no haber 4 jugadores
            if (my_attention == defender || defender >= 4 || defender < 0)
            {
                return_value = 2;
            }
            else if (players_info[defender]->status != 1)
            {
                return_value = 2;
            }
            else
            {
                return_value = attack(players_info[my_attention], players_info[defender]);
            }
            char message5_2[1];
            message5_2[0]= return_value;
            server_send_stdmessage(players_info, my_attention, 5, 1, &message5_2[0]);

            if (return_value != 2)
            {
                char* message5_3 = malloc(51);
                message5_3[0] = return_value;
                for (int j = 0; j < 50; j++)
                {
                    message5_3[1 + j] = players_info[my_attention]->name[j];
                }
                server_send_stdmessage(players_info, defender, 6, 51, message5_3);
                free(message5_3);

                char* message5_4 = malloc(1 + 2 * 50);
                message5_4[0] = return_value;
                for (int j = 0; j < 50; j++)
                {                 
                    message5_4[1 + j] = players_info[my_attention]->name[j];
                    message5_4[51 + j] = players_info[defender]->name[j];
                }

                for (int i = 0; i < 4; i++)
                {
                    if (players_info[i] -> socket != -1)
                    {
                        server_send_stdmessage(players_info, i, 7, 1 + 2 * 50, message5_4);
                    }
                }
                free(message5_4);

                if (return_value == 1)
                {
                    players_info[defender]->status = -1;
                }

                
                if(check_winner(players_info))
                {
                    char* message5_5 = malloc(51);
                    message5_5[0] = 2;
                    for (int i = 0; i < 50; i++)
                    {
                        message5_5[1 + i] = players_info[my_attention]->name[i];
                    }

                    for (int i = 0; i < 4; i++)
                    {
                        if (players_info[i] -> socket != -1)
                        {
                            server_send_stdmessage(players_info, i, 12, 1 + 50, message5_5);
                        }
                    }

                    free(message5_5);
                    break;
                }

            }

        }

        else if (msg_code == 6)
        {
            free(client_payload);
            char* message6 = malloc((12 + 4 * 50));
            message6[0] = players_info[my_attention] -> farmers;
            message6[1] = players_info[my_attention] -> miners;
            message6[2] = players_info[my_attention] -> engineers;
            message6[3] = players_info[my_attention] -> warriors;
            message6[4] = players_info[my_attention] -> gold;
            message6[5] = players_info[my_attention] -> food;
            message6[6] = players_info[my_attention] -> science;
            message6[7] = players_info[my_attention] -> farmers_level;
            message6[8] = players_info[my_attention] -> miners_level;
            message6[9] = players_info[my_attention] -> engineers_level;
            message6[10] = players_info[my_attention] -> attack_level;
            message6[11] = players_info[my_attention] -> defense_level;
            for (int i = 0; i < 4; i++)
            {
                if (players_info[i] -> status == 1)
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message6[12 + 50 * i + j] = players_info[i]->name[j];
                    }
                }
                else
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message6[12 + 50 * i + j] = '\0';
                    }
                }
            }
            server_send_stdmessage(players_info, my_attention, 2, 12 + 4 * 50, message6);
            free(message6);

            msg_code = server_receive_instruction(socket);
            if (msg_code != 6)
            {
                printf("SERVER/MAIN190: Nunca deberiamos llegar aquí");
            }
            client_payload = server_receive_stdpayload(players_info, my_attention);
            
            int spied = ((uint8_t) client_payload[0]) - 1;
            free(client_payload);

            int* return_value = spy(players_info, my_attention, spied);
            if (return_value[0] == 0)
            {
                char* message = malloc((4));
                message[0] = return_value[0];
                message[1] = return_value[1];
                message[2] = return_value[2];
                message[3] = return_value[3];
                server_send_stdmessage(players_info, my_attention, 8, 4, &message[0]);
                free(message);
            }
            else
            {
                char message = return_value[0];
                server_send_stdmessage(players_info, my_attention, 8, 1, &message);
            }
            free(return_value);
        }

        else if (msg_code == 7)
        {

            free(client_payload);
            char* message7 = malloc((12 + 4 * 50));
            message7[0] = players_info[my_attention] -> farmers;
            message7[1] = players_info[my_attention] -> miners;
            message7[2] = players_info[my_attention] -> engineers;
            message7[3] = players_info[my_attention] -> warriors;
            message7[4] = players_info[my_attention] -> gold;
            message7[5] = players_info[my_attention] -> food;
            message7[6] = players_info[my_attention] -> science;
            message7[7] = players_info[my_attention] -> farmers_level;
            message7[8] = players_info[my_attention] -> miners_level;
            message7[9] = players_info[my_attention] -> engineers_level;
            message7[10] = players_info[my_attention] -> attack_level;
            message7[11] = players_info[my_attention] -> defense_level;
            for (int i = 0; i < 4; i++)
            {
                if (players_info[i] -> status == 1)
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message7[12 + 50 * i + j] = players_info[i]->name[j];
                    }
                }
                else
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message7[12 + 50 * i + j] = '\0';
                    }
                }
            }
            server_send_stdmessage(players_info, my_attention, 2, 12 + 4 * 50, message7);
            free(message7);

            msg_code = server_receive_instruction(socket);
            if (msg_code != 7)
            {
                printf("SERVER/MAIN190: Nunca deberiamos llegar aquí");
            }
            client_payload = server_receive_stdpayload(players_info, my_attention);
            
            int defender = ((uint8_t) client_payload[0]) - 1;

            char return_value;
            if (defender >= 4 || defender < 0)
            {
                char message7_1 = (char) 2;
                server_send_stdmessage(players_info, my_attention, 9, 1, &message7_1);
            }

            else if (players_info[defender]->status != 1 || my_attention == defender)
            {
                char message7_1 = (char) 2;
                server_send_stdmessage(players_info, my_attention, 9, 1, &message7_1);
            }

            else if ((int) client_payload[1] < 0 || (int) client_payload[1] > 1)
            {
                char message7_1 = (char) 2;
                server_send_stdmessage(players_info, my_attention, 9, 1, &message7_1);
            }

            else
            {
                return_value = (char) steal(players_info[my_attention], players_info[defender], (int) client_payload[1]);
                if (return_value > 0)
                {
                    char message7_1 = 0;
                    server_send_stdmessage(players_info, my_attention, 9, 1, &message7_1);
                    char* message_players = malloc(50); // nombre más un 1 o 0
                    for (int i = 0; i < 50; i++)
                    {
                        message_players[i] = players_info[defender]->name[i];
                    }
                    for (int i = 0; i < 4; i++)
                    {
                        if (players_info[i]->socket != -1 && i != my_attention) // se envía el mensaje 11 a todos menos al ladrón
                        {
                            server_send_stdmessage(players_info, i, 11, 50, &message_players[0]);
                        }
                    }
                    free(message_players);

                    char* robbed_message = malloc(2); // 1 char más 1 int
                    robbed_message[0] = client_payload[1];
                    robbed_message[1] = return_value;
                    server_send_stdmessage(players_info, defender, 10, 2, &robbed_message[0]);
                    free(robbed_message);
                }
                else
                {
                    char message7_1 = 1;
                    server_send_stdmessage(players_info, my_attention, 9, 1, &message7_1);
                }
            }

        }

        else if (msg_code == 8)
        {
            free(client_payload);
            next_player(players_info, &my_attention);
            collect_resources(players_info[my_attention]);
        }

        else if (msg_code == 9)
        {
            free(client_payload);
            players_info[my_attention] -> status = -1;
            char* message9_1 = malloc(51);
            message9_1[0] = 1;
            for (int i = 0; i < 50; i++)
            {
                message9_1[1 + i] = players_info[my_attention]->name[i];
            }

            for (int i = 0; i < 4; i++)
            {
                if (players_info[i] -> socket != -1)
                {
                    server_send_stdmessage(players_info, i, 12, 1 + 50, message9_1);
                }
            }
            free(message9_1);

            next_player(players_info, &my_attention);
            collect_resources(players_info[my_attention]);
            if(check_winner(players_info))
            {
                char* message9_2 = malloc(51);
                message9_2[0] = 2;
                for (int i = 0; i < 50; i++)
                {
                    message9_2[1 + i] = players_info[my_attention]->name[i];
                }

                for (int i = 0; i < 4; i++)
                {
                    if (players_info[i] -> socket != -1)
                    {
                        server_send_stdmessage(players_info, i, 12, 1 + 50, message9_2);
                    }
                }

                free(message9_2);
                break;
            }
        }

        else if (msg_code == 10) //Negociar
        {

            free(client_payload);
            char* message10 = malloc((12 + 4 * 50));
            message10[0] = players_info[my_attention] -> farmers;
            message10[1] = players_info[my_attention] -> miners;
            message10[2] = players_info[my_attention] -> engineers;
            message10[3] = players_info[my_attention] -> warriors;
            message10[4] = players_info[my_attention] -> gold;
            message10[5] = players_info[my_attention] -> food;
            message10[6] = players_info[my_attention] -> science;
            message10[7] = players_info[my_attention] -> farmers_level;
            message10[8] = players_info[my_attention] -> miners_level;
            message10[9] = players_info[my_attention] -> engineers_level;
            message10[10] = players_info[my_attention] -> attack_level;
            message10[11] = players_info[my_attention] -> defense_level;
            for (int i = 0; i < 4; i++)
            {
                if (players_info[i] -> status == 1)
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message10[12 + 50 * i + j] = players_info[i]->name[j];
                    }
                }
                else
                {
                    for (int j = 0; j < 50; j++)
                    {
                        message10[12 + 50 * i + j] = '\0';
                    }
                }
            }
            server_send_stdmessage(players_info, my_attention, 2, 12 + 4 * 50, message10);
            free(message10);

            msg_code = server_receive_instruction(socket);
            if (msg_code != 10)
            {
                printf("SERVER/MAIN650: Nunca deberiamos llegar aquí");
            }
            client_payload = server_receive_stdpayload(players_info, my_attention);
            char* response_deal_message = malloc(6+50);
            response_deal_message[0] = client_payload[0];
            response_deal_message[1] = client_payload[1];
            response_deal_message[2] = client_payload[2];
            response_deal_message[3] = client_payload[3];
            response_deal_message[4] = client_payload[4];
            response_deal_message[5] = client_payload[5];
            for (int i = 0; i < 50; i++)
            {
                response_deal_message[6+i] = players_info[my_attention]->name[i];
            }
            while(response_deal_message[1] == 1)
            {
                if(response_deal_message[0]==my_attention)
                {
                    response_deal_message[0] = my_attention;
                    server_send_stdmessage(players_info, my_attention, 14, 56, response_deal_message);
                }else
                {
                    response_deal_message[0] = client_payload[0];
                    server_send_stdmessage(players_info, my_attention, 14, 56, response_deal_message);
                }
                msg_code = server_receive_instruction(socket);
                client_payload = server_receive_stdpayload(players_info, my_attention);
                response_deal_message[0] = client_payload[0];
                response_deal_message[1] = client_payload[1];
                response_deal_message[2] = client_payload[2];
                response_deal_message[3] = client_payload[3];
                response_deal_message[4] = client_payload[4];
                response_deal_message[5] = client_payload[5];
            }
            if(response_deal_message[1] == 0) //Éxito en la negociacion
            {
                finished_negociation(players_info[my_attention], players_info[client_payload], response_deal_message[2], response_deal_message[3], response_deal_message[4], response_deal_message[5]);
                server_send_stdmessage(players_info, my_attention, 14, 56, response_deal_message);
            }
            if(response_deal_message[1] == 2) // Negociación rechazada
            {
                server_send_stdmessage(players_info, my_attention, 14, 56, response_deal_message);
            }
            free(client_payload);
            free(response_deal_message);
            
        }

    }

    for (int i = 0; i < 4; i++)
    {
        player_destroy(players_info[i]);
    }
    free(players_info);
    free(addr_size);
    free(lock);
    free(arguments);
    return 0;
}
