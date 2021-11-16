# IIC2333 - Sistemas Operativos y Redes

## Proyecto 1

### Grupo León

| Nombre | Correo | Número de Estudiante |
| --- | --- | --- |
| Raúl Diaz Campos | ridiaz2@uc.cl |  1720688J |
| Matías Fernández | mafernandez9@uc.cl |  17640865 |
| Paula Torres | patorres8@uc.cl | 16206886 |
| José Valenzuela |jrvalenzuela1@uc.cl | 17206847 |
| Nicolás Burgos |burgosibarra@uc.cl | 18637213 |



### Estructura

```
P1
├─ server
|  |
|  ├─ src
│  |  ├─ comunication.c
│  |  ├─ comunication.h
│  |  ├─ conection.c
│  |  ├─ conection.h
│  |  ├─ players.c
│  |  ├─ players.h
│  |  └─ main.c
|  |
|  └─ Makefile
|
├─ client
|  |
|  ├─ src
│  |  ├─ comunication.c
│  |  ├─ comunication.h
│  |  ├─ conection.c
│  |  ├─ conection.h
│  |  └─ main.c
|  |
|  └─ Makefile
|
├─ Protocolocomunicacion.txt
|
└─ README.md
```

### Instrucciones

Para empezar el programa, primero es necesario compilar cliente y servidor, esto a partir de los Makefiles de las carpetas.

* Para el caso de servidor:

```
./server -i <IP> -p <PUERTO>
```

* Para el caso de cliente:

```
./client -i <IP> -p <PUERTO>
```

Una vez que tengamos un servidor, podemos empezar con primer cliente, quien será lider.

A cada cliente se le pedirá nombre, y registrar la distribución de su aldea a partir de un panel que actualiza valores actuales.

Para el caso de lider, se le avisará cada vez que exista una nueva conexión, para luego avisar del nombre una vez que este haya registrado su distribución.

Para empezar el juego, solo debe teclear 0 y enviar el input.

Una vez que empieza el juego, los turnos aparecen, permitiendo realizar las distintas opciones.

### Protocolos de comunicación

* **Clientes > Servidor**

| ID  |   PayloadSize |   Payload |   Descripción |
| --- | --- | --- | --- |
0   |  4*4+len(nombre)|   NUMERO1NUMERO2NUMERO3NUMERO4NOMBRE  |   Envía nombre y forma de su aldea del jugador
1   |   0           |   0       |   Iniciar juego
2   |   0           |   0       | 2.5.1 Mostrar información: pedir información
3   |   1*4           |   0 ó 1 ó 2 ó 3   | 2.5.2 Crear aldeano: pide crear aldeano de tipo 0/1/2/3
4   |   1*4           |   0 ó 1 ó 2 ó 3 ó 4   | 2.5.3 Subir nivel: sube el nivel de 0/1/2/3/4
5   |   1*4           |   0 ó 1 ó 2 ó 3 ó 4   | 2.5.4 Atacar: ataca a jugador 0/1/2/3/4
6   |   1*4           |   0 ó 1 ó 2 ó 3 ó 4   | 2.5.5 Espiar: espía a jugador 0/1/2/3/4
7   |   4+4           |   0 ó 1 ó 2 ó 3 ó 4 + 0 ó 1   | 2.5.6 Robar: robar a jugador 0/1/2/3/4 el recurso 0/1
8   |   0           |   0   | 2.5.7 Pasar: pasa
  9   |   0           |   0   | 2.5.8 Rendirse: se rinde
10  |   4 + 4 + 4*4 |   0/1/2/3 + 0/1/2/3 + 0/1/2/3 + N1 + 0/1/2/3 + N2   | 4.2 Negociar: id_usuario_a_negociar + aceptar/rechazar/modificar(ofertar) + tipo_a_recibir + cantidad + tipo_a_entregar + cantidad
11  | 0             |  0       |    Envía consulta a servidor sobre nuevas conexiones |


* **Servidor > Cliente**

| ID  |   PayloadSize |   Payload |   Descripción |
| --- | --- | --- | --- |
0   |   1*4           |   0 ó 1   |   Booleano que indica si es líder o no
1   |   1*4           |   0 ó 1 ó 2   |   Estado de inicio del juego (0 comenzó, 1 no comenzó porque alguien no esta listo, 2 no comenzo porque solo hay un jugador)
2   |   12*4+len(nombre)*4 |   N1N2N3N4N5N6N7N8N9N10N11N12NOMBRE1NOMBRE2NOMBRE3NOMBRE4   |   2.5.1 Mostrar información: Cantidad de cada recurso, aldenaos y niveles
3   |   1*4           |   0 ó 1   | 2.5.2 Crear aldeano: booleano que indica si se creó o no
4   |   1*4           |   0 ó 1 ó 2   | 2.5.3 Subir nivel: 0->exito 1->faltan_recursos 2->ya es nivel maximo
5   |   1*4           |   0 ó 1 ó 2   | 2.5.4 Atacar (para atacante): 0->Se ataco con exito 1->Se ataco y perdió 2->opcion invalida
6   |   1*4 + len(nombre)           |   0 ó 1 NOMBRE1       | 2.5.4 Atacar (para defensor): 0->Te atacaron y ganaste 1->Te atacaron y perdiste
7   |   2*len(nombre) + 4               |   0/1 NOMBRE1 NOMBRE2      |   2.5.4 Atacar (para todos): NOMBRE1 atacó a NOMBRE2 0->Nombre1 ganó 1->NOMBRE2 ganó
8   |   1*4 + 3*4           |   0 ó 1 ó 2 + N1N2N3   | 2.5.4 Espiar: 0->Espiaste, se entrega info 1->No tienes recursos 2->opcion invalida
9   |   4 + 4 + 4           |   0 ó 1 ó 2   | 2.5.4 Robar (para ladron): 0->Robaste con exito, se entrega info 1->No tienes recursos 2->opcion invalida
10   |   4 + 4           |   0 ó 1 +  N1   | 2.5.4 Robar (para victima): tipo de recurso y cuanto
11  |   len(nombre) |   NOMBRE1 |   Nombre1 fue asaltado
12  |   len(nombre) |   NOMBRE1 + 0/1|  NOMBRE1 (0-> perdió, 1->se retiró, 2->ganó)
13  |   0   |   0   |   Has ganado
14  |   4 + 4 + 4*4 |   0/1/2/3 + 0/1/2/3 + 0/1/2/3 + N1 + 0/1/2/3 + N2   | 4.2 Negociar: id_usuario_a_negociar + aceptar/rechazar/modificar(ofertar) + tipo_a_recibir + cantidad + tipo_a_entregar + cantidad + nombre
15  |       0       |       |   Indica que es el turno del jugador
16  |   1*4         |   0 o 1 o 2 o 3   | Envía a lider cuantas conexiones hay actualmente.



