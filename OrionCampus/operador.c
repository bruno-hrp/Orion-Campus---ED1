#include "operador.h"

Operador operadores[MAX_OPERADORES];
int      totalOperadores = 0;

const char *turnoStr(Turno t) {
    switch (t) {
        case TURNO_MANHA: return "MANHA";
        case TURNO_TARDE: return "TARDE";
        case TURNO_NOITE: return "NOITE";
        default:          return "?";
    }
}

const char *situacaoOpStr(SituacaoOperador s) {
    switch (s) {
        case OP_ATIVO:     return "ATIVO";
        case OP_AUSENTE:   return "AUSENTE";
        case OP_EM_MISSAO: return "EM_MISSAO";
        default:           return "?";
    }
}
