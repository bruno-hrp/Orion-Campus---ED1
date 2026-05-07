#ifndef CADASTRO_H
#define CADASTRO_H

#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"

int cadastrarDrone(Drone d);
int cadastrarOperador(Operador op);
int cadastrarMissao(Missao m);
int registrarEvento(EventoDinamico ev);

#endif
