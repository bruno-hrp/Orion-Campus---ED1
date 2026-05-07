#ifndef EVENTO_H
#define EVENTO_H

typedef enum { EV_ABERTO, EV_EM_ATENDIMENTO, EV_ENCERRADO } StatusEvento;

typedef enum {
    EV_INVASAO,
    EV_ALAGAMENTO,
    EV_RISCO_ESTRUTURAL,
    EV_PANE_ELETRICA,
    EV_INSPECAO_EMERGENCIAL,
    EV_OUTRO
} TipoEvento;

typedef struct {
    char         codigo[16];
    TipoEvento   tipo;
    char         horarioAbertura[6];
    int          prioridade;          /* sempre 0 */
    StatusEvento status;
    char         droneAlocado[16];
    char         operadorAlocado[16];
} EventoDinamico;

typedef struct NoEvento {
    EventoDinamico  evento;
    struct NoEvento *prox;
} NoEvento;

extern NoEvento *cabecaEventos;
extern NoEvento *caudaEventos;

void inicializarListaEventos(void);
void liberarListaEventos(void);

const char *statusEventoStr(StatusEvento s);
const char *tipoEventoStr(TipoEvento t);

#endif
