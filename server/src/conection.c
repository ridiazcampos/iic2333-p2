#include "conection.h"

//LINKS REFERENCIAS:
//https://www.man7.org/linux/man-pages/man2/socket.2.html
//https://man7.org/linux/man-pages/man7/socket.7.html
//https://www.man7.org/linux/man-pages/man3/setsockopt.3p.html
//https://man7.org/linux/man-pages/man2/setsockopt.2.html
//https://linux.die.net/man/3/htons
//https://linux.die.net/man/3/inet_aton
//https://www.howtogeek.com/225487/what-is-the-difference-between-127.0.0.1-and-0.0.0.0/
//https://www.man7.org/linux/man-pages/man2/bind.2.html
//https://www.man7.org/linux/man-pages/man2/accept.2.html

int test_and_set(Lock* lock)
{
    int old_value = lock->value;
    lock->value = 1;
    return old_value;
}

void acquire(Lock* lock)
{     
    while(test_and_set(lock))
    {
        sleep(2);
    }
}
void release(Lock* lock) {
  lock->value = 0;
}


void client_setting(Player** players, int player, Lock* lock)
{
    int msg_code = server_receive_instruction(players[player]->socket);
    if (msg_code != 0)
    {
        printf("SERVER/CONECTION.c38: Nunca deberíamos llegar aquí \n");
    }
    char* payload = server_receive_stdpayload(players, player);
    char* name = malloc(50);
    int farmers = (int) payload[0];
    int miners = (int) payload[1];
    int engineers = (int) payload[2];
    int warriors = (int) payload[3];
    for (int i = 0; i < 50; i++)
    {
        name[i] = payload[4 + i];
    }
    if (lock == NULL)
    {
        set_player(players[player], name, farmers, miners, engineers,
                warriors, 0, 0, 0, 1, 
                1, 1, 1, 1);
        free(name);
        free(payload);
    }
    else
    {
        acquire(lock);
        set_player(players[player], name, farmers, miners, engineers,
                warriors, 0, 0, 0, 1, 
                1, 1, 1, 1);
        free(name);
        free(payload);
        release(lock);
    }
}


void* wait_connections(void * ARGS)
{
    struct args* arguments = (struct args*) ARGS;

    struct sockaddr_in client2_addr;
    struct sockaddr_in client3_addr;
    struct sockaddr_in client4_addr;

    Player** array = arguments->players;
    int server_socket = arguments->server_socket;
    socklen_t* addr_pointer = arguments->addr_pointer;
    Lock* lock = arguments->lock;

    int client_socket;

    client_socket = accept(server_socket, (struct sockaddr *) &client2_addr, addr_pointer);
    while (client_socket == -1)
    {
        client_socket = accept(server_socket, (struct sockaddr *) &client2_addr, addr_pointer);
    }
    acquire(lock);
    connect_player(array[1], client_socket);
    release(lock);
    client_setting(array, 1, lock);

    client_socket = accept(server_socket, (struct sockaddr *) &client3_addr, addr_pointer);
    while (client_socket == -1)
    {
        client_socket = accept(server_socket, (struct sockaddr *) &client3_addr, addr_pointer);
    }
    acquire(lock);
    connect_player(array[2], client_socket);
    release(lock);
    client_setting(array, 2, lock);

    client_socket = accept(server_socket, (struct sockaddr *) &client4_addr, addr_pointer);
    while (client_socket == -1)
    {
        client_socket = accept(server_socket, (struct sockaddr *) &client4_addr, addr_pointer);
    }
    acquire(lock);
    connect_player(array[3], client_socket);
    release(lock);
    client_setting(array, 3, lock);

    pthread_exit(0);

}


Player** prepare_sockets_and_get_clients(char * IP, int port, socklen_t* addr_size, pthread_t* thread, Lock* lock, Args* arguments){
    // Se define la estructura para almacenar info del socket del servidor al momento de su creación
    struct sockaddr_in server_addr;

    // Se solicita un socket al SO, que se usará para escuchar conexiones entrantes
    // AF_INET -> IPv4 Internet protocols
    // SOCK_STREAM -> Provides sequenced, reliable, two-way, connection-based byte streams.  An out-of-band data transmission mechanism may be supported.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Se configura el socket a gusto (recomiendo fuertemente el REUSEPORT!)
    int opt = 1;
    int ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // Se guardan el puerto e IP en la estructura antes definida
    memset(&server_addr, 0, sizeof(server_addr)); //Llena con 0s tantas veces 
    server_addr.sin_family = AF_INET;
    inet_aton(IP, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    // Se le asigna al socket del servidor un puerto y una IP donde escuchar
    int ret2 = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Se coloca el socket en modo listening
    int ret3 = listen(server_socket, 1);

    // Se definen las estructuras para almacenar info sobre los sockets de los clientes
    struct sockaddr_in client1_addr;
    *addr_size = sizeof(client1_addr);


    // OJO esto obliga a recibir 4 jugadores, no menos
    // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
    Player** players = malloc(4 * sizeof(Player*));
    players[0] = player_init();
    players[1] = player_init();
    players[2] = player_init();
    players[3] = player_init();

    // Se aceptan a los primeros 2 clientes que lleguen. "accept" retorna el n° de otro socket asignado para la comunicación
    int aux_socket = accept(server_socket, (struct sockaddr *)&client1_addr, addr_size);
    while (aux_socket == -1)
    {
        aux_socket = accept(server_socket, (struct sockaddr *)&client1_addr, addr_size);
    }
    connect_player(players[0], aux_socket);
    client_setting(players, 0, NULL);
    
    arguments->players = players;
    arguments->server_socket = server_socket;
    arguments->addr_pointer = addr_size;
    arguments->lock = lock;
    
    int rc;
    rc = pthread_create(thread, NULL, wait_connections, (void *) arguments);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    return players;
}
