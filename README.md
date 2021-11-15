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
├─ src 
│  ├─ crms 
│  |  └─ main.c
|  └─ crms_API
│     ├─ crms_API.c
│     ├─ crms_API.h
│     ├─ mergeSort.c
│     ├─ mergeSort.h
│     ├─ PageTable.c
│     └─ PageTable.h
└─ Makefile
```

### `src/crms`

Es el archivo que compila a crms. Permite probar las distintas funciones a partir de números e instrucciones.

### `src/crms_API`

Contiene los distintos mídulos de la _API_, en particular:

* `PageTable:` Contiene la estructura que representa la tabla de páginas.

* `MergeSort:` Contiene funciones que serán útiles para ordenar un arreglo.

* `crms_API:` Contiene las principales estructuras, funciones, manejo de errores y aquello que corresponde a la API.


## Manejo de Errores

A continuación se presentan los errores genéricos a utilizar en el proyecto

| Valor |  Código | Tipo de Error | Mensaje |
|---|---|---|---|
| 1 | `not_op` | `Operation not permited`  | La operación que quieres realizar no está permitida porque no tienes los permisos necesarios |
| 2 | `not_file` | `No such file or directory` |  El archivo que buscas no existe |
| 3 | `not_proc` | `No such process` | El proceso con ese ID no existe. |
| 28 | `not_space` | `No space left on device` | No hay espacio suficiente para realizar la operación. |
| 34 | `num_range` | `Numerical result out of range` | El proceso que quieres iniciar tiene un ID muy grande |
| 84 | `not_byte` | `Bytes sequence` | N° de bytes inválidos |
| 99 | `not_assign` | `Cannot assign requested address` | La página de dirección virtual no tiene frame asignado | 

El error de valor 1: `Operation not permitted` será utilizado cuando:

* Se inicie un proceso ya iniciado
* Se lea/escriba un archivo que ya esté abierto en modo específico
* Se abra un archivo que esté cerrado

El error de valor 2: `No such file or directory` será utilizado cuando:
* Se busque un archivo que no existe

El error de valor 3: `No such process` será utilizado cuando:
* Se inicie/muestre/borre un proceso con un ID que no existe

El error de valor 28:

`No space left on device` será utilizado cuando:
* Agregar más de 16 procesos
* Agregar más de 10 archivos a un proceso

El error de valor 34: `Numerical result out of range` será utilizado cuando:
* Se introduzca un id de proceso mayor a 255.

El error de valor 84: `Byte sequence` será utilizado cuando:

* Se introduzca un secuencia de bytes inválida

El error de valor 99: `Cannot assign requested address` será utilizado cuando:
* La página de dirección virtual no tiene frame asignado
