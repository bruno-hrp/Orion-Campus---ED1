#ifndef OPERADOR_H
#define OPERADOR_H

#define MAX_OPERADORES 30

typedef enum { TURNO_MANHA, TURNO_TARDE, TURNO_NOITE } Turno;
typedef enum { OP_ATIVO, OP_AUSENTE, OP_EM_MISSAO }    SituacaoOperador;

typedef struct {
    char             mat[16];
    char             nome[64];
    char             certificacao[32];
    Turno            turno;
    SituacaoOperador situacao;
} Operador;

extern Operador operadores[MAX_OPERADORES];
extern int      totalOperadores;

const char *turnoStr(Turno t);
const char *situacaoOpStr(SituacaoOperador s);

#endif
