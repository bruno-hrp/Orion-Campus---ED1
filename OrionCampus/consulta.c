#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "consulta.h"
#include "historico.h"

#define LIMIAR_MERGESORT 50

/* ── ItemVisao ──────────────────────────────────────────────────────────── */
typedef enum { ITEM_MISSAO, ITEM_EVENTO } TipoItem;

typedef struct {
    void    *ptr;
    TipoItem tipo;
    int      prioridade;
    char     horario[6];
} ItemVisao;

/* ── comparador ────────────────────────────────────────────────────────── */
static int cmpItem(const ItemVisao *a, const ItemVisao *b) {
    if (a->prioridade != b->prioridade)
        return a->prioridade - b->prioridade;
    return strcmp(a->horario, b->horario);
}

/* ── insertion sort ─────────────────────────────────────────────────────── */
static void insertionSort(ItemVisao *v, int n) {
    for (int i = 1; i < n; i++) {
        ItemVisao key = v[i];
        int j = i - 1;
        while (j >= 0 && cmpItem(&v[j], &key) > 0) {
            v[j+1] = v[j];
            j--;
        }
        v[j+1] = key;
    }
}

/* ── merge sort ─────────────────────────────────────────────────────────── */
static void merge(ItemVisao *v, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    ItemVisao *L = (ItemVisao *)malloc(n1 * sizeof(ItemVisao));
    ItemVisao *R = (ItemVisao *)malloc(n2 * sizeof(ItemVisao));
    for (int i = 0; i < n1; i++) L[i] = v[l+i];
    for (int j = 0; j < n2; j++) R[j] = v[m+1+j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        v[k++] = cmpItem(&L[i], &R[j]) <= 0 ? L[i++] : R[j++];
    while (i < n1) v[k++] = L[i++];
    while (j < n2) v[k++] = R[j++];
    free(L); free(R);
}

static void mergeSort(ItemVisao *v, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(v, l, m);
        mergeSort(v, m+1, r);
        merge(v, l, m, r);
    }
}

static void ordenarVisao(ItemVisao *v, int n) {
    if (n < LIMIAR_MERGESORT)
        insertionSort(v, n);
    else
        mergeSort(v, 0, n - 1);
}

/* ── construir visão unificada ─────────────────────────────────────────── */
#define MAX_VISAO (MAX_MISSOES + 200)

static int construirVisao(ItemVisao *visao) {
    int n = 0;
    for (int i = 0; i < totalMissoes; i++) {
        if (missoes[i].status == MISSAO_PENDENTE) {
            visao[n].ptr        = &missoes[i];
            visao[n].tipo       = ITEM_MISSAO;
            visao[n].prioridade = missoes[i].prioridade;
            strncpy(visao[n].horario, missoes[i].horario, 5);
            visao[n].horario[5] = '\0';
            n++;
        }
    }
    NoEvento *cur = cabecaEventos->prox;
    while (cur != caudaEventos) {
        if (cur->evento.status == EV_ABERTO || cur->evento.status == EV_EM_ATENDIMENTO) {
            visao[n].ptr        = &cur->evento;
            visao[n].tipo       = ITEM_EVENTO;
            visao[n].prioridade = cur->evento.prioridade;
            strncpy(visao[n].horario, cur->evento.horarioAbertura, 5);
            visao[n].horario[5] = '\0';
            n++;
        }
        cur = cur->prox;
    }
    return n;
}

/* ── busca ──────────────────────────────────────────────────────────────── */
Drone *buscarDronePorId(const char *id) {
    for (int i = 0; i < totalDrones; i++)
        if (strcmp(drones[i].id, id) == 0) return &drones[i];
    printf("[INFO] Drone '%s' nao encontrado.\n", id);
    return NULL;
}

Operador *buscarOperadorPorMatricula(const char *mat) {
    for (int i = 0; i < totalOperadores; i++)
        if (strcmp(operadores[i].mat, mat) == 0) return &operadores[i];
    printf("[INFO] Operador '%s' nao encontrado.\n", mat);
    return NULL;
}

Missao *buscarMissaoPorCodigo(const char *codigo) {
    for (int i = 0; i < totalMissoes; i++)
        if (strcmp(missoes[i].codigo, codigo) == 0) return &missoes[i];
    printf("[INFO] Missao '%s' nao encontrada.\n", codigo);
    return NULL;
}

NoEvento *buscarEventoPorCodigo(const char *codigo) {
    NoEvento *cur = cabecaEventos->prox;
    while (cur != caudaEventos) {
        if (strcmp(cur->evento.codigo, codigo) == 0) return cur;
        cur = cur->prox;
    }
    printf("[INFO] Evento '%s' nao encontrado.\n", codigo);
    return NULL;
}

/* ── visão ordenada ─────────────────────────────────────────────────────── */
void exibirVisaoOrdenada(void) {
    ItemVisao visao[MAX_VISAO];
    int n = construirVisao(visao);
    ordenarVisao(visao, n);

    printf("========================================\n");
    printf(" PENDENTES — ORDENADOS POR PRIORIDADE\n");
    printf(" [%s]\n", n < LIMIAR_MERGESORT ? "insertion sort" : "merge sort");
    printf("========================================\n");
    if (n == 0) { printf(" (nenhum pendente)\n"); printf("========================================\n"); return; }
    printf(" %-4s| %-8s| %-10s| %-12s| %-7s| %s\n",
           "POS","CODIGO","ORIGEM","PRIORIDADE","HORARIO","AREA");
    printf("-----+---------+-----------+------------+---------+-------------------\n");

    for (int i = 0; i < n; i++) {
        char prioStr[16];
        if (visao[i].tipo == ITEM_EVENTO)
            snprintf(prioStr, sizeof(prioStr), "0 (URGENTE)");
        else
            snprintf(prioStr, sizeof(prioStr), "%d", visao[i].prioridade);

        if (visao[i].tipo == ITEM_MISSAO) {
            Missao *m = (Missao *)visao[i].ptr;
            printf("  %-3d| %-8s| %-10s| %-12s| %-7s| %s\n",
                   i+1, m->codigo, "PLANEJADA", prioStr, m->horario, m->area);
        } else {
            EventoDinamico *ev = (EventoDinamico *)visao[i].ptr;
            printf("  %-3d| %-8s| %-10s| %-12s| %-7s| %s\n",
                   i+1, ev->codigo, "DINAMICA", prioStr,
                   ev->horarioAbertura, tipoEventoStr(ev->tipo));
        }
    }
    printf("========================================\n");
}

/* ── ocorrências abertas ────────────────────────────────────────────────── */
void listarOcorrenciasAbertas(void) {
    printf("========================================\n");
    printf(" OCORRENCIAS ABERTAS\n");
    printf("========================================\n");
    printf(" %-8s| %-22s| %-8s| %-14s| %-7s| %s\n",
           "CODIGO","TIPO","ABERTURA","STATUS","DRONE","OPERADOR");
    printf("---------+-----------------------+----------+----------------+--------+---------\n");
    int found = 0;
    NoEvento *cur = cabecaEventos->prox;
    while (cur != caudaEventos) {
        if (cur->evento.status != EV_ENCERRADO) {
            printf(" %-8s| %-22s| %-8s| %-14s| %-7s| %s\n",
                   cur->evento.codigo,
                   tipoEventoStr(cur->evento.tipo),
                   cur->evento.horarioAbertura,
                   statusEventoStr(cur->evento.status),
                   cur->evento.droneAlocado[0] ? cur->evento.droneAlocado : "-",
                   cur->evento.operadorAlocado[0] ? cur->evento.operadorAlocado : "-");
            found = 1;
        }
        cur = cur->prox;
    }
    if (!found) printf(" (nenhuma ocorrencia aberta)\n");
    printf("========================================\n");
}
