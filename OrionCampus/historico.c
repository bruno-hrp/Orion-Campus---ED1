#include <stdio.h>
#include <string.h>
#include "historico.h"

PilhaHistorico historico = { .topo = -1 };

int empilhar(HistoricoEntry entry) {
    if (historico.topo >= MAX_HISTORICO - 1) {
        printf("[ERRO] Historico cheio (MAX=%d).\n", MAX_HISTORICO);
        return -1;
    }
    historico.dados[++historico.topo] = entry;
    return 0;
}

HistoricoEntry *consultarTopo(void) {
    if (historico.topo < 0) return NULL;
    return &historico.dados[historico.topo];
}

static void imprimirEntrada(const HistoricoEntry *e) {
    printf(" %-8s| %-10s| %-14s| %-7s| %-7s| %-7s| %s\n",
           e->referenciaOrigem,
           origemStr(e->tipoOrigem),
           statusFinalStr(e->statusFinal),
           e->horarioInicioReal,
           e->horarioEncerramento,
           e->droneUtilizado,
           e->operadorResponsavel);
}

void listarHistorico(void) {
    printf("========================================\n");
    printf(" HISTORICO OPERACIONAL (mais recente ao mais antigo)\n");
    printf("========================================\n");
    if (historico.topo < 0) { printf(" (vazio)\n"); printf("========================================\n"); return; }
    printf(" %-8s| %-10s| %-14s| %-7s| %-7s| %-7s| %s\n",
           "CODIGO","ORIGEM","STATUS FINAL","INICIO","FIM","DRONE","OPERADOR");
    printf("---------+-----------+---------------+--------+-------+-------+---------\n");
    for (int i = historico.topo; i >= 0; i--)
        imprimirEntrada(&historico.dados[i]);
    printf("========================================\n");
}

void filtrarPorDrone(const char *idDrone) {
    printf("========================================\n");
    printf(" ATIVIDADES NO HISTORICO — DRONE %s\n", idDrone);
    printf("========================================\n");
    printf(" %-8s| %-10s| %-14s| %-7s| %s\n","CODIGO","ORIGEM","STATUS FINAL","INICIO","FIM");
    printf("---------+-----------+---------------+--------+-------\n");
    int found = 0;
    for (int i = historico.topo; i >= 0; i--) {
        if (strcmp(historico.dados[i].droneUtilizado, idDrone) == 0) {
            printf(" %-8s| %-10s| %-14s| %-7s| %s\n",
                   historico.dados[i].referenciaOrigem,
                   origemStr(historico.dados[i].tipoOrigem),
                   statusFinalStr(historico.dados[i].statusFinal),
                   historico.dados[i].horarioInicioReal,
                   historico.dados[i].horarioEncerramento);
            found = 1;
        }
    }
    if (!found) printf(" (nenhum registro)\n");
    printf("========================================\n");
}

void filtrarPorOperador(const char *mat) {
    printf("========================================\n");
    printf(" ATIVIDADES NO HISTORICO — OPERADOR %s\n", mat);
    printf("========================================\n");
    printf(" %-8s| %-10s| %-14s| %-7s| %s\n","CODIGO","ORIGEM","STATUS FINAL","INICIO","FIM");
    printf("---------+-----------+---------------+--------+-------\n");
    int found = 0;
    for (int i = historico.topo; i >= 0; i--) {
        if (strcmp(historico.dados[i].operadorResponsavel, mat) == 0) {
            printf(" %-8s| %-10s| %-14s| %-7s| %s\n",
                   historico.dados[i].referenciaOrigem,
                   origemStr(historico.dados[i].tipoOrigem),
                   statusFinalStr(historico.dados[i].statusFinal),
                   historico.dados[i].horarioInicioReal,
                   historico.dados[i].horarioEncerramento);
            found = 1;
        }
    }
    if (!found) printf(" (nenhum registro)\n");
    printf("========================================\n");
}

const char *origemStr(TipoOrigem o) {
    return o == ORIGEM_PLANEJADA ? "PLANEJADA" : "DINAMICA";
}

const char *statusFinalStr(StatusFinalHist s) {
    switch (s) {
        case HIST_CONCLUIDA:    return "CONCLUIDA";
        case HIST_CANCELADA:    return "CANCELADA";
        case HIST_INTERROMPIDA: return "INTERROMPIDA";
        case HIST_REPLANEJADA:  return "REPLANEJADA";
        default:                return "?";
    }
}
