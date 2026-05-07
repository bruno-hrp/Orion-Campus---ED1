#ifndef CONSULTA_H
#define CONSULTA_H

#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"

Drone    *buscarDronePorId(const char *id);
Operador *buscarOperadorPorMatricula(const char *mat);
Missao   *buscarMissaoPorCodigo(const char *codigo);
NoEvento *buscarEventoPorCodigo(const char *codigo);

void exibirVisaoOrdenada(void);
void listarOcorrenciasAbertas(void);

#endif
