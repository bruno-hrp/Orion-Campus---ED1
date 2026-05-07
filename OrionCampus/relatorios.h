#ifndef RELATORIOS_H
#define RELATORIOS_H

void relEstadoDrones(void);
void relAgendaDia(void);
void relPendentes(void);
void relOcorrenciasAbertas(void);
void relHistorico(void);
void relAtividadesPorDrone(const char *idDrone);
void relAtividadesPorOp(const char *mat);

#endif
