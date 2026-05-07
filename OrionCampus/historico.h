#ifndef HISTORICO_H
#define HISTORICO_H

#define MAX_HISTORICO 200

typedef enum { ORIGEM_PLANEJADA, ORIGEM_DINAMICA } TipoOrigem;

typedef enum {
    HIST_CONCLUIDA,
    HIST_CANCELADA,
    HIST_INTERROMPIDA,
    HIST_REPLANEJADA
} StatusFinalHist;

typedef struct {
    char            referenciaOrigem[16];
    TipoOrigem      tipoOrigem;
    StatusFinalHist statusFinal;
    char            horarioInicioReal[6];
    char            horarioEncerramento[6];
    char            droneUtilizado[16];
    char            operadorResponsavel[16];
    char            observacoes[128];
} HistoricoEntry;

typedef struct {
    HistoricoEntry dados[MAX_HISTORICO];
    int            topo;
} PilhaHistorico;

extern PilhaHistorico historico;

int             empilhar(HistoricoEntry entry);
HistoricoEntry *consultarTopo(void);
void            listarHistorico(void);
void            filtrarPorDrone(const char *idDrone);
void            filtrarPorOperador(const char *mat);

const char *origemStr(TipoOrigem o);
const char *statusFinalStr(StatusFinalHist s);

#endif
