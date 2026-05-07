#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"
#include "historico.h"
#include "cadastro.h"
#include "operacao.h"
#include "consulta.h"
#include "relatorios.h"

/* ── utilitários de leitura ─────────────────────────────────────────────── */
static void lerLinha(char *buf, int tamanho) {
    if (fgets(buf, tamanho, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}

static int lerInt(const char *prompt) {
    char buf[32];
    printf("%s", prompt);
    lerLinha(buf, sizeof(buf));
    return atoi(buf);
}

static float lerFloat(const char *prompt) {
    char buf[32];
    printf("%s", prompt);
    lerLinha(buf, sizeof(buf));
    return (float)atof(buf);
}

static void lerStr(const char *prompt, char *dest, int tam) {
    printf("%s", prompt);
    lerLinha(dest, tam);
}

/* ── menus de seleção de enum ───────────────────────────────────────────── */
static StatusDrone selecionarStatusDrone(void) {
    printf("  Status: 0=DISPONIVEL  1=EM_MISSAO  2=MANUTENCAO  3=RETORNANDO\n");
    int s = lerInt("  Opcao: ");
    if (s < 0 || s > 3) s = 0;
    return (StatusDrone)s;
}

static Turno selecionarTurno(void) {
    printf("  Turno: 0=MANHA  1=TARDE  2=NOITE\n");
    int t = lerInt("  Opcao: ");
    if (t < 0 || t > 2) t = 0;
    return (Turno)t;
}

static TipoMissao selecionarTipoMissao(void) {
    printf("  Tipo: 0=RONDA  1=VISTORIA  2=INSPECAO  3=ACOMPANHAMENTO  4=OUTRO\n");
    int t = lerInt("  Opcao: ");
    if (t < 0 || t > 4) t = 0;
    return (TipoMissao)t;
}

static TipoEvento selecionarTipoEvento(void) {
    printf("  Tipo: 0=INVASAO  1=ALAGAMENTO  2=RISCO_ESTRUTURAL\n");
    printf("        3=PANE_ELETRICA  4=INSPECAO_EMERGENCIAL  5=OUTRO\n");
    int t = lerInt("  Opcao: ");
    if (t < 0 || t > 5) t = 0;
    return (TipoEvento)t;
}

/* ── fluxos de cadastro ─────────────────────────────────────────────────── */
static void fluxoCadastrarDrone(void) {
    Drone d;
    memset(&d, 0, sizeof(d));
    lerStr("  ID do drone: ",        d.id,       sizeof(d.id));
    lerStr("  Modelo: ",             d.modelo,   sizeof(d.modelo));
    d.autonomia              = lerInt("  Autonomia (min): ");
    d.nivelDisponibilidade   = lerInt("  Nivel disponibilidade (0-100): ");
    d.capacidade             = lerFloat("  Capacidade (kg): ");
    lerStr("  Setor vinculado: ",    d.setorVinc, sizeof(d.setorVinc));
    d.status                 = selecionarStatusDrone();
    cadastrarDrone(d);
}

static void fluxoCadastrarOperador(void) {
    Operador op;
    memset(&op, 0, sizeof(op));
    lerStr("  Matricula: ",      op.mat,          sizeof(op.mat));
    lerStr("  Nome: ",           op.nome,         sizeof(op.nome));
    lerStr("  Certificacao: ",   op.certificacao, sizeof(op.certificacao));
    op.turno    = selecionarTurno();
    op.situacao = OP_ATIVO;
    cadastrarOperador(op);
}

static void fluxoCadastrarMissao(void) {
    Missao m;
    memset(&m, 0, sizeof(m));
    lerStr("  Codigo: ",   m.codigo,  sizeof(m.codigo));
    lerStr("  Horario (HH:MM): ", m.horario, sizeof(m.horario));
    m.prioridade = lerInt("  Prioridade (>= 1, menor = mais urgente): ");
    lerStr("  Area: ",     m.area,    sizeof(m.area));
    m.duracao    = lerInt("  Duracao (min): ");
    m.tipo       = selecionarTipoMissao();
    lerStr("  Recurso preferencial (opcional, Enter p/ pular): ",
           m.recursoPreferencial, sizeof(m.recursoPreferencial));
    cadastrarMissao(m);
}

static void fluxoRegistrarEvento(void) {
    EventoDinamico ev;
    memset(&ev, 0, sizeof(ev));
    lerStr("  Codigo do evento: ",         ev.codigo,          sizeof(ev.codigo));
    lerStr("  Horario de abertura (HH:MM): ", ev.horarioAbertura, sizeof(ev.horarioAbertura));
    ev.tipo = selecionarTipoEvento();
    registrarEvento(ev);
}

/* ── fluxos de operação ─────────────────────────────────────────────────── */
static void fluxoAlocar(void) {
    char cod[16], drone[16], op[16];
    printf("  Tipo: 1=Missao planejada  2=Evento dinamico\n");
    int tipo = lerInt("  Opcao: ");
    lerStr("  Codigo da missao/evento: ", cod,   sizeof(cod));
    lerStr("  ID do drone: ",             drone, sizeof(drone));
    lerStr("  Matricula do operador: ",   op,    sizeof(op));
    if (tipo == 2)
        alocarRecursosEvento(cod, drone, op);
    else
        alocarRecursos(cod, drone, op);
}

static void fluxoIniciar(void) {
    printf("  Tipo: 1=Missao planejada  2=Evento dinamico\n");
    int tipo = lerInt("  Opcao: ");
    char cod[16];
    lerStr("  Codigo: ", cod, sizeof(cod));
    if (tipo == 2)
        iniciarEvento(cod);
    else
        iniciarMissao(cod);
}

static void fluxoAtualizar(void) {
    char cod[16];
    lerStr("  Codigo da missao: ", cod, sizeof(cod));
    printf("  Novo status: 1=CONCLUIDA  2=INTERROMPIDA\n");
    int s = lerInt("  Opcao: ");
    StatusMissao ns = (s == 2) ? MISSAO_INTERROMPIDA : MISSAO_CONCLUIDA;
    atualizarStatusMissao(cod, ns);
}

static void fluxoCancelar(void) {
    char cod[16], motivo[128];
    lerStr("  Codigo da missao: ", cod, sizeof(cod));
    lerStr("  Motivo: ", motivo, sizeof(motivo));
    cancelarMissao(cod, motivo);
}

static void fluxoReplanejar(void) {
    char cod[16];
    lerStr("  Codigo da missao original: ", cod, sizeof(cod));
    Missao nova;
    memset(&nova, 0, sizeof(nova));
    printf("  --- Dados da nova missao ---\n");
    lerStr("  Codigo: ",           nova.codigo,  sizeof(nova.codigo));
    lerStr("  Horario (HH:MM): ",  nova.horario, sizeof(nova.horario));
    nova.prioridade = lerInt("  Prioridade: ");
    lerStr("  Area: ",             nova.area,    sizeof(nova.area));
    nova.duracao    = lerInt("  Duracao (min): ");
    nova.tipo       = selecionarTipoMissao();
    replanejarMissao(cod, nova);
}

static void fluxoEncerrarEvento(void) {
    char cod[16];
    lerStr("  Codigo do evento: ", cod, sizeof(cod));
    encerrarEvento(cod);
}

/* ── fluxos de estado de recursos ──────────────────────────────────────── */
static void fluxoEstadoDrone(void) {
    printf("  1=Manutencao  2=Confirmar retorno\n");
    int op = lerInt("  Opcao: ");
    char id[16];
    lerStr("  ID do drone: ", id, sizeof(id));
    if (op == 1) marcarDroneManutencao(id);
    else         confirmarRetornoDrone(id);
}

static void fluxoEstadoOp(void) {
    printf("  1=Registrar ausencia  2=Retornar ao servico\n");
    int op = lerInt("  Opcao: ");
    char mat[16];
    lerStr("  Matricula: ", mat, sizeof(mat));
    if (op == 1) registrarAusenciaOperador(mat);
    else         retornarOperador(mat);
}

/* ── fluxos de consulta ─────────────────────────────────────────────────── */
static void fluxoBuscarDrone(void) {
    char id[16];
    lerStr("  ID do drone: ", id, sizeof(id));
    Drone *d = buscarDronePorId(id);
    if (d) {
        printf("  Drone encontrado:\n");
        printf("  ID: %s | Modelo: %s | Status: %s | Bateria: %d%% | Setor: %s\n",
               d->id, d->modelo, statusDroneStr(d->status),
               d->nivelDisponibilidade, d->setorVinc);
    }
}

static void fluxoBuscarOperador(void) {
    char mat[16];
    lerStr("  Matricula: ", mat, sizeof(mat));
    Operador *op = buscarOperadorPorMatricula(mat);
    if (op) {
        printf("  Operador encontrado:\n");
        printf("  Mat: %s | Nome: %s | Turno: %s | Situacao: %s\n",
               op->mat, op->nome, turnoStr(op->turno), situacaoOpStr(op->situacao));
    }
}

static void fluxoBuscarMissao(void) {
    char cod[16];
    lerStr("  Codigo da missao: ", cod, sizeof(cod));
    Missao *m = buscarMissaoPorCodigo(cod);
    if (m) {
        printf("  Missao encontrada:\n");
        printf("  Cod: %s | Horario: %s | Area: %s | Prio: %d | Status: %s\n",
               m->codigo, m->horario, m->area, m->prioridade,
               statusMissaoStr(m->status));
        if (m->droneAlocado[0])
            printf("  Drone: %s | Operador: %s\n", m->droneAlocado, m->operadorAlocado);
    }
}

/* ── menus ──────────────────────────────────────────────────────────────── */
static void menuCadastro(void) {
    int op;
    do {
        printf("\n=== CADASTRO ===\n");
        printf("1. Cadastrar drone\n");
        printf("2. Cadastrar operador\n");
        printf("3. Cadastrar missao planejada\n");
        printf("4. Registrar evento dinamico\n");
        printf("0. Voltar\n");
        op = lerInt("Opcao: ");
        switch (op) {
            case 1: fluxoCadastrarDrone();    break;
            case 2: fluxoCadastrarOperador(); break;
            case 3: fluxoCadastrarMissao();   break;
            case 4: fluxoRegistrarEvento();   break;
        }
    } while (op != 0);
}

static void menuOperacao(void) {
    int op;
    do {
        printf("\n=== OPERACAO ===\n");
        printf("1. Alocar drone e operador\n");
        printf("2. Iniciar missao/evento\n");
        printf("3. Atualizar status de missao (concluir/interromper)\n");
        printf("4. Cancelar missao\n");
        printf("5. Replanejar missao\n");
        printf("6. Encerrar evento dinamico\n");
        printf("7. Alterar estado de drone (manutencao/retorno)\n");
        printf("8. Alterar situacao de operador (ausencia/retorno)\n");
        printf("0. Voltar\n");
        op = lerInt("Opcao: ");
        switch (op) {
            case 1: fluxoAlocar();       break;
            case 2: fluxoIniciar();      break;
            case 3: fluxoAtualizar();    break;
            case 4: fluxoCancelar();     break;
            case 5: fluxoReplanejar();   break;
            case 6: fluxoEncerrarEvento(); break;
            case 7: fluxoEstadoDrone();  break;
            case 8: fluxoEstadoOp();     break;
        }
    } while (op != 0);
}

static void menuConsulta(void) {
    int op;
    do {
        printf("\n=== CONSULTA ===\n");
        printf("1. Buscar drone por ID\n");
        printf("2. Buscar operador por matricula\n");
        printf("3. Buscar missao por codigo\n");
        printf("4. Listar pendentes por prioridade\n");
        printf("5. Listar ocorrencias abertas\n");
        printf("0. Voltar\n");
        op = lerInt("Opcao: ");
        switch (op) {
            case 1: fluxoBuscarDrone();       break;
            case 2: fluxoBuscarOperador();    break;
            case 3: fluxoBuscarMissao();      break;
            case 4: exibirVisaoOrdenada();    break;
            case 5: listarOcorrenciasAbertas(); break;
        }
    } while (op != 0);
}

static void menuRelatorios(void) {
    int op;
    do {
        printf("\n=== RELATORIOS ===\n");
        printf("1.  Estado dos drones\n");
        printf("2.  Agenda do dia\n");
        printf("3.  Missoes pendentes (por prioridade)\n");
        printf("4.  Ocorrencias abertas\n");
        printf("5.  Historico operacional\n");
        printf("6.  Atividades por drone\n");
        printf("7.  Atividades por operador\n");
        printf("0.  Voltar\n");
        op = lerInt("Opcao: ");
        char buf[16];
        switch (op) {
            case 1: relEstadoDrones();         break;
            case 2: relAgendaDia();            break;
            case 3: relPendentes();            break;
            case 4: relOcorrenciasAbertas();   break;
            case 5: relHistorico();            break;
            case 6:
                lerStr("  ID do drone: ", buf, sizeof(buf));
                relAtividadesPorDrone(buf);
                break;
            case 7:
                lerStr("  Matricula do operador: ", buf, sizeof(buf));
                relAtividadesPorOp(buf);
                break;
        }
    } while (op != 0);
}

/* ── main ───────────────────────────────────────────────────────────────── */
int main(void) {
    inicializarListaEventos();

    int op;
    do {
        printf("\n");
        printf("╔══════════════════════════════════════╗\n");
        printf("║        ORION CAMPUS — CONTROLE       ║\n");
        printf("╚══════════════════════════════════════╝\n");
        printf("  1. Cadastro\n");
        printf("  2. Operacao\n");
        printf("  3. Consulta\n");
        printf("  4. Relatorios\n");
        printf("  0. Sair\n");
        op = lerInt("Opcao: ");
        switch (op) {
            case 1: menuCadastro();   break;
            case 2: menuOperacao();   break;
            case 3: menuConsulta();   break;
            case 4: menuRelatorios(); break;
            case 0: printf("Encerrando Orion Campus.\n"); break;
            default: printf("[ERRO] Opcao invalida.\n"); break;
        }
    } while (op != 0);

    liberarListaEventos();
    return 0;
}
