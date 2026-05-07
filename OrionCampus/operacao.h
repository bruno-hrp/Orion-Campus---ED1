#ifndef OPERACAO_H
#define OPERACAO_H

#include "missao.h"

int alocarRecursos(const char *codMissao, const char *idDrone, const char *matOp);
int alocarRecursosEvento(const char *codEvento, const char *idDrone, const char *matOp);
int iniciarMissao(const char *codMissao);
int iniciarEvento(const char *codEvento);
int atualizarStatusMissao(const char *codMissao, StatusMissao novoStatus);
int replanejarMissao(const char *codMissao, Missao nova);
int cancelarMissao(const char *codMissao, const char *motivo);
int encerrarEvento(const char *codEvento);
int marcarDroneManutencao(const char *idDrone);
int confirmarRetornoDrone(const char *idDrone);
int registrarAusenciaOperador(const char *mat);
int retornarOperador(const char *mat);

#endif
