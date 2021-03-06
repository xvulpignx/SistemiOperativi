/*
    name: generatore
    descrizione: Pgm che si occupa di generare una stringa casuale ogni 3 sec su una stringa in memoria condivisa di lunghezza N,
                 passato nel vett di stringhe **argv.
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include "procedure.h"



int main(int argn, char **argv){
    //chiavi IPC
    key_t chiave_buf, chiave_sem;
    //descrittori IPC
    int ds_shm, ds_sem;
    //Puntatori alla shm
    void* buf;
    char *s;
    int *n_lett;
    //N, da ricavare con strtol(STRING TO LONG), da char **argv
    char *p; //endptr di strtol
    int N;

    //converto argv[1] in intero se e' stato specificato
    if(argn < 2){
        perror("not enough parameters\n");
        exit(1);
    }
    N = (int)strtol(argv[1], &p, 10);
    
    //allocazione shm
    chiave_buf = ftok(FTOK_PATH, FTOK_CHAR);
    if(chiave_buf < 0){
        perror("ERR ftok shm generatore\n");
        exit(1);
    }
    ds_shm = shmget(chiave_buf, (N + 1) + sizeof(int), IPC_CREAT | 0664);
    if(ds_shm < 0){
        perror("ERR shmget generatore\n");
        exit(1);
    }
    buf = shmat(ds_shm, NULL, 0);
    if(buf == (void*)-1){
        perror("ERR shmat generatore\n");
        exit(1);
    }


    //allocazione semafori
    chiave_sem = ftok(FTOK_PATH, FTOK_CHAR);//posso usare i stessi parametri perche' due IPC diverse
    if(chiave_sem < 0){
        perror("ERR ftok sem generatore\n");
        exit(1);
    }
    ds_sem = semget(chiave_sem, 2, IPC_CREAT | 0664);
    if(ds_sem < 0){
        perror("ERR semget generatore\n");
        exit(1);
    }

    //Retrieve della stringa e di n_lett dal void* in shared memory
    s = (char*)buf;
    buf += N+1;
    n_lett = (int*)(buf);

    //CODICE GENERATORE
    int i =0;
    for(i = 0; i < ITERAZIONI; i++){
        generatore(ds_sem, s, N);
        sleep(3);
    }
    return 0;
}