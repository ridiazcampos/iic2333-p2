#include "players.h"


Player* player_init()
{
    Player* player = malloc(sizeof(Player));
    player -> socket = -1;
    player -> status = -1;
    player -> name = malloc(50);
    for (int i = 0; i < 50; i++)
    {
        player -> name[i] = '\0';
    }
    return player;
}

void connect_player(Player* player, int socket)
{
    player -> socket = socket;
    player -> status = 0;
}

void set_player(Player* player, char* name, int farmers, int miners, int engineers,
                int warriors, int gold, int food, int science, int farmers_level, 
                int miners_level, int engineers_level, int attack_level, int defense_level)
{
    player -> status = 1;
    for (int i = 0; i < 50; i++)
    {
        player -> name[i] = name[i];
    }
    player -> farmers = farmers;
    player -> miners = miners;
    player -> engineers = engineers;
    player -> warriors = warriors;
    player -> gold = gold;
    player -> food = food;
    player -> science = science;
    player -> farmers_level = farmers_level;
    player -> miners_level = miners_level;
    player -> engineers_level = engineers_level;
    player -> attack_level = attack_level;
    player -> defense_level = defense_level;
}

void collect_resources(Player* player) //Se suman las cantidades de recursos necesarios
{
    player -> gold += player -> miners * player -> miners_level * 2;
    player -> food += player -> farmers * player -> farmers_level * 2;
    player -> science += player -> engineers * player -> engineers_level;
}

int create_villager(Player* player, int option)// Se compra un aldeano, falta enviar mensajes de q no se pueden hacer las cosas
{
    if (option == 1) //Se crea un agricultor
    {
        if (player -> food >= 10)
        {
            player -> food -= 10; // Se descuenta recurso actual
            player -> farmers +=1; //Se añade un agricultor
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else if (option == 2) //Se crea minero
    {
        if (player -> food >= 10 && player -> gold >=5)
        {
            player -> food -= 10;
            player -> gold -= 5;
            player -> miners += 1;
            return 0;
        }
        else 
        {
            return 1;
        }

    }
    else if (option == 3) //Se crea ingeniero
    {
        if (player -> food >= 20 && player -> gold >= 10)
        {
            player -> food -= 20;
            player -> gold -= 10;
            player -> engineers +=1;
            return 0;
        }
        else 
        {
            return 1;
        }
    }
    else if (option == 4) //Se crea guerrero
    {
        if (player -> food >= 10 && player -> gold >= 10)
        {
            player -> food -= 10;
            player -> gold -= 10;
            player -> warriors +=1;
            return 0;
        }
        else 
        {
            return 1;
        }
    }
}

int level_up(Player* player, int option)
{
    int options[5];
    options[0] = player -> farmers_level;
    options[1] =  player -> miners_level;
    options[2] = player -> engineers_level;
    options[3] = player -> attack_level;
    options[4] = player -> defense_level; 
    int upgrade = options[option - 1];
    if (upgrade < 5)
    {
        if (player -> food >= 10* upgrade && player -> gold >= 10 * upgrade && player -> science >= 10* upgrade)
        {
            if (option == 1)
            {
                player -> farmers_level +=1;
                player -> food -= 10 * upgrade;
                player -> gold -= 10 * upgrade;
                player -> science -= 10 * upgrade;
            }
            if (option == 2)
            {
                player -> miners_level +=1;
                player -> food -= 10 * upgrade;
                player -> gold -= 10 * upgrade;
                player -> science -= 10 * upgrade;
            }
            if (option == 3)
            {
                player -> engineers_level +=1;
                player -> food -= 10 * upgrade;
                player -> gold -= 10 * upgrade;
                player -> science -= 10 * upgrade;
            }
            if (option == 4)
            {
                player -> attack_level +=1;
                player -> food -= 10 * upgrade;
                player -> gold -= 10 * upgrade;
                player -> science -= 10 * upgrade;
            }
            if (option == 5)
            {
                player -> defense_level +=1;
                player -> food -= 10 * upgrade;
                player -> gold -= 10 * upgrade;
                player -> science -= 10 * upgrade;
            }
            return 0;
        }
        else 
        {
            //printf("No se tienen los recursos para realizar esta acción");
            return 1;
        }
    }
    return 2;
}

void player_destroy(Player* player)
{
    free(player->name);
    free(player);
}

// 2.5.4 atacar
int attack(Player* attacker, Player* defender)
{
    int att_force = attacker->warriors*attacker->attack_level;
    int def_force = defender->warriors*defender->defense_level*2;
    if (att_force > def_force)
    {
        attacker->food = attacker->food + defender->food;
        attacker->gold = attacker->gold + defender->gold;
        attacker->science = attacker->science + defender->science;
        defender->food = 0;
        defender->gold = 0;
        defender->science = 0;
        return 1;
    }else if (att_force < def_force)
    {
        attacker->warriors = floor(attacker->warriors*0.5);
        return 0;
    }
    
}

// 2.5.5 Espiar
int* spy(Player** players_info, int attacker, int spied)
{
    int* return_value = malloc(4 * sizeof(int));

    if (spied >= 4 || spied < 0)
    {
        return_value[0] = 2;
        return return_value;
    }

    if (players_info[spied]->status != 1 || attacker == spied)
    {
        return_value[0] = 2;
        return return_value;
    }

    if (players_info[attacker]->gold < 30)
    {
        return_value[0] = 1;
        return return_value;
    }
    players_info[attacker] -> gold -= 30;
    return_value[0] = 0;
    return_value[1] = players_info[spied] -> warriors;
    return_value[2] = players_info[spied] -> attack_level;
    return_value[3] = players_info[spied] -> defense_level;
    return return_value;
}

// 2.5.6 Robar
int steal(Player* player, Player* player_robbed, int resource)
{
    if (player->science<10)
    {
        return 0;
    }
    player->science = player->science - 10;
    int amount_to_steal;
    if (resource == 0)
    {
        amount_to_steal = floor(player_robbed->food*0.1);
        player->food += amount_to_steal;
        player_robbed->food -= amount_to_steal;
        return amount_to_steal;
    }
    else if (resource == 1)
    {
        amount_to_steal = floor(player_robbed->gold*0.1);
        player->gold += amount_to_steal;
        player_robbed->gold -= amount_to_steal;
        return amount_to_steal;
    }
    else
    {
        return 0;
    }
}