#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

//Comida maxima
#define comida_maxima 5
//Tiempo de espera
#define tiempo_espera_maximo 300
//Numero de filosofos
#define numero_filosofos 3
//Tiempo de trabajo
#define Tiempo_de_trabajo 1

// los valores o estados de cada filosofo
enum estados {
    PENSANDO, COMIENDO, HAMBRIENTO
};
// En este apartado vamos  unir diferentes tipos de datos en uno solo gracias 
// la la palabra clave struct -> todo estara en FilosofosComensales
struct FilosofosComensales {
    pthread_mutex_t bloquear;
    enum estados estado_actual[numero_filosofos];
    pthread_cond_t condicion[numero_filosofos];
    long tiempo_espera_total[numero_filosofos];
    long contador[numero_filosofos];
};

struct FilosofosComensales filosofos;

long tiempo_espera_unico[numero_filosofos];

// Salimos del nodo hijo sin preocuparnos de las perdidas de memoria
void salida() {
    long sum = 0;
    //printf("Tiempo total de espera: ");
    for (int k = 0; k < numero_filosofos; k++) {
        sum += filosofos.tiempo_espera_total[k];
    }
    for (int k = 0; k < numero_filosofos; k++) {
    }
    printf("\n Programa finalizado con exito ::: La comida se terminó \n");
    exit(0);
}


// Obtenemos la hora en este mismo momento
long getHora() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (long) (ts.tv_sec * 1000000000 + ts.tv_nsec);
    }
    return 0;
}


//Damos permiso a los demás filosofos sin están hambrientos y han esperado mucho tiempo
int conPermiso(int i) {
    long hora = getHora();
    long temp1 = 0, temp2 = 0, tiempo = tiempo_espera_unico[i] + hora;
    // El filosofo se inicializa como ambriento
    if (filosofos.estado_actual[(i + 1) % numero_filosofos] == HAMBRIENTO) {
        temp1 = tiempo_espera_unico[(i + 1) % numero_filosofos] + hora;
        //este if nos ayudara a controlar que los filosofos mueran por no comer 
        if (temp1 > tiempo && temp1 > tiempo_espera_maximo) {
            printf("Filosofo %d dando paso a %d a comer para evitar morir\n", i, (i + 1) % numero_filosofos);
            return 0;
        }
    }
        // El siguienrte filosofo se inicializa como ambriento
    if (filosofos.estado_actual[(i + numero_filosofos - 1) % numero_filosofos] == HAMBRIENTO) {
        temp2 = tiempo_espera_unico[(i + numero_filosofos - 1) % numero_filosofos] + hora;
        if (temp2 > tiempo && temp2 > tiempo_espera_maximo) {
            printf("Filosofo %d dando paso a %d a comer para evitar la morir\n", i, (i + numero_filosofos - 1) % numero_filosofos);
            return 0;
        }
    }
    return 1;
}

//Comprueba si los vecinos no estan comiendo y si el filosofo no esta hambriento
//Cambiamos estado_actual a comer y señalamos la condicion
void comprueba_tenedores(int i) {
    printf("Filosofo %d buscando tenedores\n", i);
    if (filosofos.estado_actual[i] == HAMBRIENTO && (filosofos.estado_actual[(i + 1) % numero_filosofos] != COMIENDO && filosofos.estado_actual[(i + numero_filosofos - 1) % numero_filosofos] != COMIENDO) && conPermiso(i)) {
        filosofos.estado_actual[i] = COMIENDO;

        //El tiempo de espera = tiempo de inicio - tiempo en este momento
        tiempo_espera_unico[i] += getHora();
        filosofos.tiempo_espera_total[i] += tiempo_espera_unico[i];
        //Esto no desbloquea el mutex, debemos hacerlo desde el padre
        pthread_cond_signal(&filosofos.condicion[i]);
    }
}

//Si el filosofo está pensando, cambiamos estado_actual de folósofo a PENSANDO
//Llamamos a los vecinos filósofos para comprobar si hay tenedores libres
void return_tenedores(int num_filosofo) {
    static int num_comida = 0;
    // desoues de ver terminado de comer cambia de estado a PENSANDO
    pthread_mutex_lock(&filosofos.bloquear);
    filosofos.contador[num_filosofo]++;
    filosofos.estado_actual[num_filosofo] = PENSANDO;
    printf("Filosofo %d termina de comer\n", num_filosofo);

    num_comida++;

    printf("#Recuento de comidas = %d\n\n", num_comida);
    //si el numero total de comica el igual al numero total que han comido ... termina el programa
    if (num_comida == comida_maxima) {
        salida();
    }
    //se comprobara el estado de los tenedores 
    comprueba_tenedores((num_filosofo + numero_filosofos - 1) % numero_filosofos);
    comprueba_tenedores((num_filosofo + 1) % numero_filosofos);
    pthread_mutex_unlock(&filosofos.bloquear);
}

//Si el filosofo tiene hambre 
//Comprobamos si hay tenedores libres, si no lo están entonces espera la señal
void coger_tenedores(int num_filosofo) {
    pthread_mutex_lock(&filosofos.bloquear);
    filosofos.estado_actual[num_filosofo] = HAMBRIENTO;
    printf("Filosofo %d esta hambriento\n", num_filosofo);

    //Guardamos el tiempo de inicio
    tiempo_espera_unico[num_filosofo] = (-1) * getHora();

    comprueba_tenedores(num_filosofo);
    if (filosofos.estado_actual[num_filosofo] != COMIENDO) {
        //Es necesario que el filosofo no esté comiendo, para que otros hilos puedan user el compartido
        pthread_cond_wait(&filosofos.condicion[num_filosofo], &filosofos.bloquear);
    }
    pthread_mutex_unlock(&filosofos.bloquear);
}

//Se ejecutara infinitamente para todos los filosofos en diferentes hilos
//Los filosofos piensan y comen
void *filosofo(void *param) {
    int num_filosofo = *(int *) param;
    while (1) {
        srand(time(NULL) + num_filosofo);
        int tiempo_pensar = (rand() % Tiempo_de_trabajo) + 1;
        int tiempo_comer = (rand() % Tiempo_de_trabajo) + 1;
        filosofos.estado_actual[num_filosofo] = PENSANDO;
        //Dormimos el proceso durante el tiempo de pensamiento antes hallado
        sleep(tiempo_pensar); //->>> nanda a dormir al filisofo
        coger_tenedores(num_filosofo);
        //Dormimos el proceso durante el tiempo de comida antes hallado
        sleep(tiempo_comer);
        return_tenedores(num_filosofo);
    }
}

int main() {
    //Iniciamos el codigo
    pthread_t tid[numero_filosofos];
    int id[numero_filosofos];
    pthread_mutex_init(&filosofos.bloquear, NULL);
    for (int j = 0; j < numero_filosofos; j++) {
        id[j] = j;
        // Todos los filosofos creados empezaran con un valor inicial de PENSANDO
        filosofos.estado_actual[j] = PENSANDO;
        filosofos.contador[j] = 0;
        filosofos.tiempo_espera_total[j] = 0;
        tiempo_espera_unico[j] = 0;
        pthread_create(&tid[j], NULL, &filosofo, &id[j]);
    }
    //El hilo principal debe seguir funcionando cuando los otros hilos se esten ejecutando
    for (int j = 0; j < numero_filosofos; j++) {
        pthread_join(tid[j], NULL);
    }
}