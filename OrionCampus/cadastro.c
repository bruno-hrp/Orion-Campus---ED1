#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cadastro.h"

/* ── validação de formato HH:MM ─────────────────────────────────────────── */
static int horarioValido(const char *h) {
    if (strlen(h) != 5) return 0;
    if (h[2] != ':') return 0;
    for (int i = 0; i < 5; i++)
        if (i != 2 && (h[i] < '0' || h[i] > '9')) return 0;
    int hh = (h[0]-'0')*10 + (h[1]-'0');
    int mm = (h[3]-'0')*10 + (h[4]-'0');
    return hh < 24 && mm < 60;
}

/* ── Drone ──────────────────────────────────────────────────────────────── */
int cadastrarDrone(Drone d) {
    if (d.id[0] == '\0') { printf("[ERRO] ID do drone nao pode ser vazio.\n"); return -1; }
    if (totalDrones >= MAX_DRONES) { printf("[ERRO] Limite de drones atingido (%d).\n", MAX_DRONES); return -1; }
    for (int i = 0; i < totalDrones; i++) {
        if (strcmp(drones[i].id, d.id) == 0) { printf("[ERRO] Drone '%s' ja cadastrado.\n", d.id); return -1; }
    }
    if (d.autonomia <= 0)                      { printf("[ERRO] Autonomia deve ser > 0.\n"); return -1; }
    if (d.nivelDisponibilidade < 0 || d.nivelDisponibilidade > 100) { printf("[ERRO] Nivel de disponibilidade deve ser 0-100.\n"); return -1; }
    if (d.capacidade <= 0.0f)                  { printf("[ERRO] Capacidade deve ser > 0.\n"); return -1; }
    d.totalAtividades = 0;
    drones[totalDrones++] = d;
    printf("[OK] Drone '%s' cadastrado.\n", d.id);
    return 0;
}

/* ── Operador ───────────────────────────────────────────────────────────── */
int cadastrarOperador(Operador op) {
    if (op.mat[0] == '\0') { printf("[ERRO] Matricula nao pode ser vazia.\n"); return -1; }
    if (totalOperadores >= MAX_OPERADORES) { printf("[ERRO] Limite de operadores atingido (%d).\n", MAX_OPERADORES); return -1; }
    for (int i = 0; i < totalOperadores; i++) {
        if (strcmp(operadores[i].mat, op.mat) == 0) { printf("[ERRO] Operador '%s' ja cadastrado.\n", op.mat); return -1; }
    }
    operadores[totalOperadores++] = op;
    printf("[OK] Operador '%s' cadastrado.\n", op.mat);
    return 0;
}

/* ── Missão ─────────────────────────────────────────────────────────────── */
int cadastrarMissao(Missao m) {
    if (m.codigo[0] == '\0') { printf("[ERRO] Codigo da missao nao pode ser vazio.\n"); return -1; }
    if (totalMissoes >= MAX_MISSOES) { printf("[ERRO] Limite de missoes atingido (%d).\n", MAX_MISSOES); return -1; }
    for (int i = 0; i < totalMissoes; i++) {
        if (strcmp(missoes[i].codigo, m.codigo) == 0) { printf("[ERRO] Missao '%s' ja cadastrada.\n", m.codigo); return -1; }
    }
    if (!horarioValido(m.horario)) { printf("[ERRO] Horario invalido (use HH:MM).\n"); return -1; }
    if (m.prioridade < 1)          { printf("[ERRO] Prioridade deve ser >= 1.\n"); return -1; }
    if (m.duracao <= 0)            { printf("[ERRO] Duracao deve ser > 0.\n"); return -1; }
    m.status = MISSAO_PENDENTE;
    m.droneAlocado[0] = '\0';
    m.operadorAlocado[0] = '\0';
    missoes[totalMissoes++] = m;
    printf("[OK] Missao '%s' cadastrada.\n", m.codigo);
    return 0;
}

/* ── Evento Dinâmico ────────────────────────────────────────────────────── */
int registrarEvento(EventoDinamico ev) {
    if (!horarioValido(ev.horarioAbertura)) { printf("[ERRO] Horario invalido (use HH:MM).\n"); return -1; }
    ev.prioridade = 0;
    ev.status = EV_ABERTO;
    ev.droneAlocado[0] = '\0';
    ev.operadorAlocado[0] = '\0';

    NoEvento *no = (NoEvento *)malloc(sizeof(NoEvento));
    if (!no) { printf("[ERRO] Falha de alocacao de memoria.\n"); return -1; }
    no->evento = ev;
    /* inserir após sentinela cabeça em O(1) */
    no->prox = cabecaEventos->prox;
    cabecaEventos->prox = no;
    printf("[OK] Evento '%s' registrado.\n", ev.codigo);
    return 0;
}
