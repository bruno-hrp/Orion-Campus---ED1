#ifndef MISSAO_H
#define MISSAO_H

#define MAX_MISSOES 100

typedef enum {
    MISSAO_PENDENTE,
    MISSAO_EM_EXECUCAO,
    MISSAO_CONCLUIDA,
    MISSAO_CANCELADA,
    MISSAO_REPLANEJADA,
    MISSAO_INTERROMPIDA
} StatusMissao;

typedef enum {
    TIPO_RONDA,
    TIPO_VISTORIA,
    TIPO_INSPECAO,
    TIPO_ACOMPANHAMENTO,
    TIPO_OUTRO
} TipoMissao;

typedef struct {
    char         codigo[16];
    char         horario[6];
    int          prioridade;
    char         area[64];
    int          duracao;
    TipoMissao   tipo;
    char         recursoPreferencial[16];
    StatusMissao status;
    char         droneAlocado[16];
    char         operadorAlocado[16];
} Missao;

extern Missao missoes[MAX_MISSOES];
extern int    totalMissoes;

const char *statusMissaoStr(StatusMissao s);
const char *tipoMissaoStr(TipoMissao t);

#endif
