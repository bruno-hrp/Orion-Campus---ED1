#ifndef DRONE_H
#define DRONE_H

#define MAX_DRONES     50
#define MAX_ATIV_DRONE 20

typedef enum {
    DRONE_DISPONIVEL,
    DRONE_EM_MISSAO,
    DRONE_MANUTENCAO,
    DRONE_RETORNANDO
} StatusDrone;

typedef struct {
    char        id[16];
    char        modelo[64];
    int         autonomia;
    int         nivelDisponibilidade;
    float       capacidade;
    StatusDrone status;
    char        setorVinc[32];
    char        atividadesVinculadas[MAX_ATIV_DRONE][16];
    int         totalAtividades;
} Drone;

extern Drone drones[MAX_DRONES];
extern int   totalDrones;

const char *statusDroneStr(StatusDrone s);

#endif
