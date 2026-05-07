#include <stdio.h>
#include <string.h>
#include "relatorios.h"
#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"
#include "historico.h"
#include "consulta.h"

/* RF15 — Estado dos Drones */
void relEstadoDrones(void) {
    printf("========================================\n");
    printf(" ESTADO DOS DRONES\n");
    printf("========================================\n");
    if (totalDrones == 0) { printf(" (nenhum drone cadastrado)\n"); printf("========================================\n"); return; }
    printf(" %-8s| %-20s| %-14s| %-8s| %s\n",
           "ID","MODELO","STATUS","BATERIA","SETOR");
    printf("---------+---------------------+---------------+---------+-----------\n");
    for (int i = 0; i < totalDrones; i++) {
        printf(" %-8s| %-20s| %-14s| %-7d%%| %s\n",
               drones[i].id,
               drones[i].modelo,
               statusDroneStr(drones[i].status),
               drones[i].nivelDisponibilidade,
               drones[i].setorVinc);
    }
    printf("========================================\n");
}

/* RF16 — Agenda do Dia (vetor de missões por horário) */
void relAgendaDia(void) {
    printf("========================================\n");
    printf(" AGENDA DO DIA — MISSOES PLANEJADAS\n");
    printf("========================================\n");
    if (totalMissoes == 0) { printf(" (nenhuma missao cadastrada)\n"); printf("========================================\n"); return; }
    printf(" %-7s| %-8s| %-20s| %-15s| %-11s| %s\n",
           "HORARIO","CODIGO","AREA","TIPO","PRIORIDADE","STATUS");
    printf("--------+---------+---------------------+----------------+------------+-------------\n");

    /* ordenação simples por horário para exibição (cópia local dos índices) */
    int idx[MAX_MISSOES];
    for (int i = 0; i < totalMissoes; i++) idx[i] = i;
    for (int i = 1; i < totalMissoes; i++) {
        int key = idx[i], j = i - 1;
        while (j >= 0 && strcmp(missoes[idx[j]].horario, missoes[key].horario) > 0) {
            idx[j+1] = idx[j]; j--;
        }
        idx[j+1] = key;
    }
    for (int i = 0; i < totalMissoes; i++) {
        Missao *m = &missoes[idx[i]];
        printf(" %-7s| %-8s| %-20s| %-15s| %-11d| %s\n",
               m->horario, m->codigo, m->area,
               tipoMissaoStr(m->tipo), m->prioridade,
               statusMissaoStr(m->status));
    }
    printf("========================================\n");
}

/* RF18 / RF13 — Pendentes ordenados por prioridade */
void relPendentes(void) {
    exibirVisaoOrdenada();
}

/* RF14 — Ocorrências abertas */
void relOcorrenciasAbertas(void) {
    listarOcorrenciasAbertas();
}

/* RF17 — Histórico */
void relHistorico(void) {
    listarHistorico();
}

/* RF19 — Atividades por drone */
void relAtividadesPorDrone(const char *idDrone) {
    Drone *d = buscarDronePorId(idDrone);
    if (!d) return;

    printf("========================================\n");
    printf(" ATIVIDADES — DRONE %s (%s)\n", d->id, d->modelo);
    printf("========================================\n");

    /* missões ainda no vetor */
    printf(" [Missoes ativas/pendentes vinculadas]\n");
    printf(" %-8s| %-20s| %s\n","CODIGO","AREA","STATUS");
    printf("---------+---------------------+-------------\n");
    int found = 0;
    for (int i = 0; i < totalMissoes; i++) {
        if (strcmp(missoes[i].droneAlocado, idDrone) == 0) {
            printf(" %-8s| %-20s| %s\n",
                   missoes[i].codigo, missoes[i].area,
                   statusMissaoStr(missoes[i].status));
            found = 1;
        }
    }
    if (!found) printf(" (nenhuma)\n");

    /* histórico */
    printf("\n");
    filtrarPorDrone(idDrone);
}

/* RF20 — Atividades por operador */
void relAtividadesPorOp(const char *mat) {
    Operador *op = buscarOperadorPorMatricula(mat);
    if (!op) return;

    printf("========================================\n");
    printf(" ATIVIDADES — OPERADOR %s (%s)\n", op->mat, op->nome);
    printf("========================================\n");

    printf(" [Missoes ativas/pendentes vinculadas]\n");
    printf(" %-8s| %-20s| %s\n","CODIGO","AREA","STATUS");
    printf("---------+---------------------+-------------\n");
    int found = 0;
    for (int i = 0; i < totalMissoes; i++) {
        if (strcmp(missoes[i].operadorAlocado, mat) == 0) {
            printf(" %-8s| %-20s| %s\n",
                   missoes[i].codigo, missoes[i].area,
                   statusMissaoStr(missoes[i].status));
            found = 1;
        }
    }
    if (!found) printf(" (nenhuma)\n");

    printf("\n");
    filtrarPorOperador(mat);
}
