#include <stdlib.h>
#include "evento.h"

NoEvento *cabecaEventos = NULL;
NoEvento *caudaEventos  = NULL;

void inicializarListaEventos(void) {
    cabecaEventos = (NoEvento *)malloc(sizeof(NoEvento));
    caudaEventos  = (NoEvento *)malloc(sizeof(NoEvento));
    cabecaEventos->prox = caudaEventos;
    caudaEventos->prox  = NULL;
}

void liberarListaEventos(void) {
    NoEvento *cur = cabecaEventos->prox;
    while (cur != caudaEventos) {
        NoEvento *prox = cur->prox;
        free(cur);
        cur = prox;
    }
    free(cabecaEventos);
    free(caudaEventos);
    cabecaEventos = NULL;
    caudaEventos  = NULL;
}

const char *statusEventoStr(StatusEvento s) {
    switch (s) {
        case EV_ABERTO:         return "ABERTO";
        case EV_EM_ATENDIMENTO: return "EM_ATENDIMENTO";
        case EV_ENCERRADO:      return "ENCERRADO";
        default:                return "?";
    }
}

const char *tipoEventoStr(TipoEvento t) {
    switch (t) {
        case EV_INVASAO:              return "INVASAO";
        case EV_ALAGAMENTO:           return "ALAGAMENTO";
        case EV_RISCO_ESTRUTURAL:     return "RISCO_ESTRUTURAL";
        case EV_PANE_ELETRICA:        return "PANE_ELETRICA";
        case EV_INSPECAO_EMERGENCIAL: return "INSPECAO_EMERGENCIAL";
        case EV_OUTRO:                return "OUTRO";
        default:                      return "?";
    }
}
