#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "operacao.h"
#include "drone.h"
#include "operador.h"
#include "evento.h"
#include "historico.h"
#include "consulta.h"

/* ── utilitário: verifica se turno é compatível com horário ─────────────── */
static int turnoCompativel(Turno turno, const char *horario) {
    int hh = (horario[0]-'0')*10 + (horario[1]-'0');
    /* MANHA: 06-13, TARDE: 13-21, NOITE: 21-06 */
    if (turno == TURNO_MANHA)  return hh >= 6  && hh < 13;
    if (turno == TURNO_TARDE)  return hh >= 13 && hh < 21;
    if (turno == TURNO_NOITE)  return hh >= 21 || hh < 6;
    return 0;
}

/* ── utilitário: libera drone e operador de uma missão ──────────────────── */
static void liberarRecursosMissao(Missao *m) {
    Drone *d = buscarDronePorId(m->droneAlocado);
    if (d && d->status == DRONE_EM_MISSAO) d->status = DRONE_RETORNANDO;

    Operador *op = buscarOperadorPorMatricula(m->operadorAlocado);
    if (op && op->situacao == OP_EM_MISSAO) op->situacao = OP_ATIVO;

    m->droneAlocado[0]    = '\0';
    m->operadorAlocado[0] = '\0';
}

/* ── utilitário: libera drone e operador de um evento ───────────────────── */
static void liberarRecursosEvento(EventoDinamico *ev) {
    Drone *d = buscarDronePorId(ev->droneAlocado);
    if (d && d->status == DRONE_EM_MISSAO) d->status = DRONE_RETORNANDO;

    Operador *op = buscarOperadorPorMatricula(ev->operadorAlocado);
    if (op && op->situacao == OP_EM_MISSAO) op->situacao = OP_ATIVO;

    ev->droneAlocado[0]    = '\0';
    ev->operadorAlocado[0] = '\0';
}

/* ── verificar dupla alocação de drone ──────────────────────────────────── */
static int droneJaAlocado(const char *idDrone) {
    for (int i = 0; i < totalMissoes; i++) {
        if ((missoes[i].status == MISSAO_PENDENTE || missoes[i].status == MISSAO_EM_EXECUCAO)
            && strcmp(missoes[i].droneAlocado, idDrone) == 0)
            return 1;
    }
    NoEvento *cur = cabecaEventos->prox;
    while (cur != caudaEventos) {
        if (cur->evento.status != EV_ENCERRADO
            && strcmp(cur->evento.droneAlocado, idDrone) == 0)
            return 1;
        cur = cur->prox;
    }
    return 0;
}

/* ── verificar dupla alocação de operador ───────────────────────────────── */
static int operadorJaAlocado(const char *mat) {
    for (int i = 0; i < totalMissoes; i++) {
        if ((missoes[i].status == MISSAO_PENDENTE || missoes[i].status == MISSAO_EM_EXECUCAO)
            && strcmp(missoes[i].operadorAlocado, mat) == 0)
            return 1;
    }
    NoEvento *cur = cabecaEventos->prox;
    while (cur != caudaEventos) {
        if (cur->evento.status != EV_ENCERRADO
            && strcmp(cur->evento.operadorAlocado, mat) == 0)
            return 1;
        cur = cur->prox;
    }
    return 0;
}

/* ── alocarRecursos (missão planejada) ──────────────────────────────────── */
int alocarRecursos(const char *codMissao, const char *idDrone, const char *matOp) {
    Missao *m = buscarMissaoPorCodigo(codMissao);
    if (!m) return -1;
    if (m->status != MISSAO_PENDENTE) {
        printf("[ERRO] Missao '%s' nao esta PENDENTE (status: %s).\n",
               codMissao, statusMissaoStr(m->status)); return -1;
    }
    Drone *d = buscarDronePorId(idDrone);
    if (!d) return -1;
    if (d->status != DRONE_DISPONIVEL) {
        printf("[ERRO] Drone '%s' nao esta DISPONIVEL (status: %s).\n",
               idDrone, statusDroneStr(d->status)); return -1;
    }
    if (droneJaAlocado(idDrone)) {
        printf("[ERRO] Drone '%s' ja esta alocado em outra missao ativa.\n", idDrone); return -1;
    }
    Operador *op = buscarOperadorPorMatricula(matOp);
    if (!op) return -1;
    if (op->situacao != OP_ATIVO) {
        printf("[ERRO] Operador '%s' nao esta ATIVO (situacao: %s).\n",
               matOp, situacaoOpStr(op->situacao)); return -1;
    }
    if (!turnoCompativel(op->turno, m->horario)) {
        printf("[ERRO] Turno do operador '%s' (%s) incompativel com horario da missao (%s).\n",
               matOp, turnoStr(op->turno), m->horario); return -1;
    }
    if (operadorJaAlocado(matOp)) {
        printf("[ERRO] Operador '%s' ja esta alocado em outra missao ativa.\n", matOp); return -1;
    }
    strncpy(m->droneAlocado,    idDrone, 15); m->droneAlocado[15]    = '\0';
    strncpy(m->operadorAlocado, matOp,   15); m->operadorAlocado[15] = '\0';
    d->status    = DRONE_EM_MISSAO;
    op->situacao = OP_EM_MISSAO;
    printf("[OK] Recursos alocados para missao '%s': drone '%s', operador '%s'.\n",
           codMissao, idDrone, matOp);
    return 0;
}

/* ── alocarRecursosEvento ───────────────────────────────────────────────── */
int alocarRecursosEvento(const char *codEvento, const char *idDrone, const char *matOp) {
    NoEvento *no = buscarEventoPorCodigo(codEvento);
    if (!no) return -1;
    EventoDinamico *ev = &no->evento;
    if (ev->status != EV_ABERTO) {
        printf("[ERRO] Evento '%s' nao esta ABERTO.\n", codEvento); return -1;
    }
    Drone *d = buscarDronePorId(idDrone);
    if (!d) return -1;
    if (d->status != DRONE_DISPONIVEL) {
        printf("[ERRO] Drone '%s' nao esta DISPONIVEL (status: %s).\n",
               idDrone, statusDroneStr(d->status)); return -1;
    }
    if (droneJaAlocado(idDrone)) {
        printf("[ERRO] Drone '%s' ja esta alocado.\n", idDrone); return -1;
    }
    Operador *op = buscarOperadorPorMatricula(matOp);
    if (!op) return -1;
    if (op->situacao != OP_ATIVO) {
        printf("[ERRO] Operador '%s' nao esta ATIVO.\n", matOp); return -1;
    }
    if (operadorJaAlocado(matOp)) {
        printf("[ERRO] Operador '%s' ja esta alocado.\n", matOp); return -1;
    }
    strncpy(ev->droneAlocado,    idDrone, 15); ev->droneAlocado[15]    = '\0';
    strncpy(ev->operadorAlocado, matOp,   15); ev->operadorAlocado[15] = '\0';
    d->status    = DRONE_EM_MISSAO;
    op->situacao = OP_EM_MISSAO;
    printf("[OK] Recursos alocados para evento '%s': drone '%s', operador '%s'.\n",
           codEvento, idDrone, matOp);
    return 0;
}

/* ── iniciarMissao ──────────────────────────────────────────────────────── */
int iniciarMissao(const char *codMissao) {
    Missao *m = buscarMissaoPorCodigo(codMissao);
    if (!m) return -1;
    if (m->status != MISSAO_PENDENTE) {
        printf("[ERRO] Missao '%s' nao esta PENDENTE.\n", codMissao); return -1;
    }
    if (m->droneAlocado[0] == '\0' || m->operadorAlocado[0] == '\0') {
        printf("[ERRO] Missao '%s' sem recursos alocados. Aloque drone e operador primeiro.\n",
               codMissao); return -1;
    }
    m->status = MISSAO_EM_EXECUCAO;
    printf("[OK] Missao '%s' iniciada.\n", codMissao);
    return 0;
}

/* ── iniciarEvento ──────────────────────────────────────────────────────── */
int iniciarEvento(const char *codEvento) {
    NoEvento *no = buscarEventoPorCodigo(codEvento);
    if (!no) return -1;
    EventoDinamico *ev = &no->evento;
    if (ev->status != EV_ABERTO) {
        printf("[ERRO] Evento '%s' nao esta ABERTO.\n", codEvento); return -1;
    }
    if (ev->droneAlocado[0] == '\0' || ev->operadorAlocado[0] == '\0') {
        printf("[ERRO] Evento '%s' sem recursos alocados.\n", codEvento); return -1;
    }
    ev->status = EV_EM_ATENDIMENTO;
    printf("[OK] Atendimento do evento '%s' iniciado.\n", codEvento);
    return 0;
}

/* ── atualizarStatusMissao ──────────────────────────────────────────────── */
int atualizarStatusMissao(const char *codMissao, StatusMissao novoStatus) {
    Missao *m = buscarMissaoPorCodigo(codMissao);
    if (!m) return -1;
    /* transições válidas */
    if (m->status == MISSAO_PENDENTE && novoStatus == MISSAO_EM_EXECUCAO) {
        return iniciarMissao(codMissao);
    }
    if (m->status == MISSAO_EM_EXECUCAO &&
        (novoStatus == MISSAO_CONCLUIDA || novoStatus == MISSAO_INTERROMPIDA)) {
        liberarRecursosMissao(m);
        m->status = novoStatus;
        printf("[OK] Status da missao '%s' atualizado para %s.\n",
               codMissao, statusMissaoStr(novoStatus));
        return 0;
    }
    printf("[ERRO] Transicao invalida: %s -> %s.\n",
           statusMissaoStr(m->status), statusMissaoStr(novoStatus));
    return -1;
}

/* ── cancelarMissao ─────────────────────────────────────────────────────── */
int cancelarMissao(const char *codMissao, const char *motivo) {
    Missao *m = buscarMissaoPorCodigo(codMissao);
    if (!m) return -1;
    if (m->status == MISSAO_CONCLUIDA || m->status == MISSAO_CANCELADA) {
        printf("[ERRO] Missao '%s' ja esta %s.\n", codMissao, statusMissaoStr(m->status)); return -1;
    }
    if (!motivo || motivo[0] == '\0') {
        printf("[ERRO] Motivo de cancelamento nao pode ser vazio.\n"); return -1;
    }
    liberarRecursosMissao(m);
    m->status = MISSAO_CANCELADA;

    HistoricoEntry entry;
    strncpy(entry.referenciaOrigem, m->codigo, 15);  entry.referenciaOrigem[15] = '\0';
    entry.tipoOrigem   = ORIGEM_PLANEJADA;
    entry.statusFinal  = HIST_CANCELADA;
    entry.horarioInicioReal[0]   = '\0';
    entry.horarioEncerramento[0] = '\0';
    entry.droneUtilizado[0]      = '\0';
    entry.operadorResponsavel[0] = '\0';
    strncpy(entry.observacoes, motivo, 127); entry.observacoes[127] = '\0';
    empilhar(entry);
    printf("[OK] Missao '%s' cancelada.\n", codMissao);
    return 0;
}

/* ── replanejarMissao ───────────────────────────────────────────────────── */
int replanejarMissao(const char *codMissao, Missao nova) {
    Missao *m = buscarMissaoPorCodigo(codMissao);
    if (!m) return -1;
    if (m->status != MISSAO_PENDENTE && m->status != MISSAO_EM_EXECUCAO) {
        printf("[ERRO] Replanejamento exige status PENDENTE ou EM_EXECUCAO.\n"); return -1;
    }
    liberarRecursosMissao(m);
    m->status = MISSAO_REPLANEJADA;

    HistoricoEntry entry;
    strncpy(entry.referenciaOrigem, m->codigo, 15); entry.referenciaOrigem[15] = '\0';
    entry.tipoOrigem   = ORIGEM_PLANEJADA;
    entry.statusFinal  = HIST_REPLANEJADA;
    entry.horarioInicioReal[0]   = '\0';
    entry.horarioEncerramento[0] = '\0';
    entry.droneUtilizado[0]      = '\0';
    entry.operadorResponsavel[0] = '\0';
    snprintf(entry.observacoes, 128, "Replanejado para: %s", nova.codigo);
    empilhar(entry);

    /* cadastrar nova missão */
    nova.status = MISSAO_PENDENTE;
    nova.droneAlocado[0]    = '\0';
    nova.operadorAlocado[0] = '\0';
    if (totalMissoes >= MAX_MISSOES) {
        printf("[ERRO] Limite de missoes atingido, nao foi possivel criar nova missao.\n"); return -1;
    }
    missoes[totalMissoes++] = nova;
    printf("[OK] Missao '%s' replanejada. Nova missao '%s' criada.\n", codMissao, nova.codigo);
    return 0;
}

/* ── encerrarEvento ─────────────────────────────────────────────────────── */
int encerrarEvento(const char *codEvento) {
    /* busca o nó anterior para remoção da lista encadeada */
    NoEvento *prev = cabecaEventos;
    NoEvento *cur  = cabecaEventos->prox;
    while (cur != caudaEventos) {
        if (strcmp(cur->evento.codigo, codEvento) == 0) break;
        prev = cur;
        cur  = cur->prox;
    }
    if (cur == caudaEventos) {
        printf("[INFO] Evento '%s' nao encontrado.\n", codEvento); return -1;
    }
    EventoDinamico *ev = &cur->evento;
    if (ev->status != EV_EM_ATENDIMENTO && ev->status != EV_ABERTO) {
        printf("[ERRO] Evento '%s' nao pode ser encerrado (status: %s).\n",
               codEvento, statusEventoStr(ev->status)); return -1;
    }
    HistoricoEntry entry;
    strncpy(entry.referenciaOrigem,    ev->codigo,          15); entry.referenciaOrigem[15]    = '\0';
    strncpy(entry.droneUtilizado,      ev->droneAlocado,    15); entry.droneUtilizado[15]      = '\0';
    strncpy(entry.operadorResponsavel, ev->operadorAlocado, 15); entry.operadorResponsavel[15] = '\0';
    entry.tipoOrigem   = ORIGEM_DINAMICA;
    entry.statusFinal  = HIST_CONCLUIDA;
    entry.horarioInicioReal[0]   = '\0';
    entry.horarioEncerramento[0] = '\0';
    strncpy(entry.observacoes, tipoEventoStr(ev->tipo), 127); entry.observacoes[127] = '\0';

    liberarRecursosEvento(ev);

    /* remover nó da lista */
    prev->prox = cur->prox;
    free(cur);
    empilhar(entry);
    printf("[OK] Evento '%s' encerrado e registrado no historico.\n", codEvento);
    return 0;
}

/* ── estado de drone e operador ─────────────────────────────────────────── */
int marcarDroneManutencao(const char *idDrone) {
    Drone *d = buscarDronePorId(idDrone);
    if (!d) return -1;
    d->status = DRONE_MANUTENCAO;
    printf("[OK] Drone '%s' marcado como EM MANUTENCAO.\n", idDrone);
    return 0;
}

int confirmarRetornoDrone(const char *idDrone) {
    Drone *d = buscarDronePorId(idDrone);
    if (!d) return -1;
    if (d->status != DRONE_RETORNANDO && d->status != DRONE_MANUTENCAO) {
        printf("[ERRO] Drone '%s' nao esta RETORNANDO ou EM MANUTENCAO.\n", idDrone); return -1;
    }
    d->status = DRONE_DISPONIVEL;
    printf("[OK] Drone '%s' disponivel.\n", idDrone);
    return 0;
}

int registrarAusenciaOperador(const char *mat) {
    Operador *op = buscarOperadorPorMatricula(mat);
    if (!op) return -1;
    op->situacao = OP_AUSENTE;
    printf("[OK] Operador '%s' marcado como AUSENTE.\n", mat);
    return 0;
}

int retornarOperador(const char *mat) {
    Operador *op = buscarOperadorPorMatricula(mat);
    if (!op) return -1;
    op->situacao = OP_ATIVO;
    printf("[OK] Operador '%s' retornou ao servico.\n", mat);
    return 0;
}
