#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "drone.h"
#include "operador.h"
#include "missao.h"
#include "evento.h"
#include "historico.h"
#include "cadastro.h"
#include "operacao.h"
#include "consulta.h"
#include "relatorios.h"

#define PASS(msg) printf("[PASS] %s\n", msg)
#define FAIL(msg) printf("[FAIL] %s\n", msg)
#define CHECK(cond, msg) do { if (cond) PASS(msg); else FAIL(msg); } while(0)

int main(void) {
    inicializarListaEventos();

    printf("\n=== CENARIO 1: Cadastros basicos ===\n");
    Drone d1 = { "D001","Phantom 4",90,85,2.5f,DRONE_DISPONIVEL,"Bloco A",{{}},0 };
    Drone d2 = { "D002","Mavic Pro", 60,70,1.5f,DRONE_DISPONIVEL,"Patio",  {{}},0 };
    Drone d3 = { "D003","DJI Mini",  45,100,0.5f,DRONE_DISPONIVEL,"Sul",   {{}},0 };
    CHECK(cadastrarDrone(d1) == 0,  "Cadastrar D001");
    CHECK(cadastrarDrone(d2) == 0,  "Cadastrar D002");
    CHECK(cadastrarDrone(d3) == 0,  "Cadastrar D003");
    CHECK(cadastrarDrone(d1) == -1, "Duplicata D001 rejeitada");

    Operador op1 = {"OP001","Bruno Silva","Multirotor",TURNO_MANHA,OP_ATIVO};
    Operador op2 = {"OP002","Carla Mota", "Multirotor",TURNO_TARDE,OP_ATIVO};
    Operador op3 = {"OP003","Rafael Lima","Multirotor",TURNO_NOITE,OP_ATIVO};
    CHECK(cadastrarOperador(op1) == 0, "Cadastrar OP001 (manha)");
    CHECK(cadastrarOperador(op2) == 0, "Cadastrar OP002 (tarde)");
    CHECK(cadastrarOperador(op3) == 0, "Cadastrar OP003 (noite)");

    Missao m1 = {"M001","07:00",1,"Estacionamento Norte",60,TIPO_RONDA,"",MISSAO_PENDENTE,"",""};
    Missao m2 = {"M002","14:00",2,"Bloco B",45,TIPO_VISTORIA,"",MISSAO_PENDENTE,"",""};
    Missao m3 = {"M003","22:00",3,"Quadra Sul",30,TIPO_INSPECAO,"",MISSAO_PENDENTE,"",""};
    CHECK(cadastrarMissao(m1) == 0,  "Cadastrar M001");
    CHECK(cadastrarMissao(m2) == 0,  "Cadastrar M002");
    CHECK(cadastrarMissao(m3) == 0,  "Cadastrar M003");

    Missao mInvalida = {"","07:00",1,"Area",30,TIPO_RONDA,"",MISSAO_PENDENTE,"",""};
    CHECK(cadastrarMissao(mInvalida) == -1, "Codigo vazio rejeitado");
    Missao mPrio = {"M999","07:00",0,"Area",30,TIPO_RONDA,"",MISSAO_PENDENTE,"",""};
    CHECK(cadastrarMissao(mPrio) == -1, "Prioridade 0 rejeitada");

    printf("\n=== CENARIO 2: RN01 - Missao sem drone disponivel ===\n");
    marcarDroneManutencao("D001");
    CHECK(alocarRecursos("M001","D001","OP001") == -1, "RN03: Drone em manutencao bloqueado");
    confirmarRetornoDrone("D001");

    printf("\n=== CENARIO 3: RN02 - Turno incompativel ===\n");
    /* M001 = 07:00 (manha), OP002 = tarde */
    CHECK(alocarRecursos("M001","D001","OP002") == -1, "RN02: Turno tarde rejeitado para missao de manha");
    CHECK(alocarRecursos("M001","D001","OP001") == 0,  "RN02: Turno manha aceito para missao de manha");

    printf("\n=== CENARIO 4: Iniciar missao e verificar estados ===\n");
    CHECK(iniciarMissao("M001") == 0, "Missao M001 iniciada");
    Drone *dp = buscarDronePorId("D001");
    CHECK(dp && dp->status == DRONE_EM_MISSAO, "D001 esta EM_MISSAO");
    Operador *opp = buscarOperadorPorMatricula("OP001");
    CHECK(opp && opp->situacao == OP_EM_MISSAO, "OP001 esta EM_MISSAO");

    printf("\n=== CENARIO 5: RN09 - Dupla alocacao bloqueada ===\n");
    CHECK(alocarRecursos("M002","D001","OP002") == -1, "RN09: D001 ja alocado - bloqueado");
    CHECK(alocarRecursos("M002","D002","OP001") == -1, "RN09: OP001 ja alocado - bloqueado");

    printf("\n=== CENARIO 6: Concluir missao e verificar liberacao ===\n");
    CHECK(atualizarStatusMissao("M001", MISSAO_CONCLUIDA) == 0, "M001 concluida");
    dp = buscarDronePorId("D001");
    CHECK(dp && dp->status == DRONE_RETORNANDO, "D001 em RETORNANDO apos conclusao");
    opp = buscarOperadorPorMatricula("OP001");
    CHECK(opp && opp->situacao == OP_ATIVO, "OP001 voltou a ATIVO");

    printf("\n=== CENARIO 7: Evento dinamico com prioridade maxima ===\n");
    EventoDinamico ev1 = {"EV001",EV_RISCO_ESTRUTURAL,"09:15",0,EV_ABERTO,"",""};
    EventoDinamico ev2 = {"EV002",EV_ALAGAMENTO,      "09:15",0,EV_ABERTO,"",""};
    CHECK(registrarEvento(ev1) == 0, "EV001 registrado");
    CHECK(registrarEvento(ev2) == 0, "EV002 registrado (mesmo horario - desempate por cadastro)");

    confirmarRetornoDrone("D001");
    CHECK(alocarRecursosEvento("EV001","D001","OP001") == 0, "Recursos alocados para EV001");
    CHECK(iniciarEvento("EV001") == 0, "EV001 em atendimento");

    printf("\n=== CENARIO 8: Visao ordenada (eventos antes de missoes) ===\n");
    printf("--- Visao unificada de pendentes ---\n");
    exibirVisaoOrdenada();

    printf("\n=== CENARIO 9: Cancelar missao com motivo ===\n");
    CHECK(alocarRecursos("M002","D002","OP002") == 0, "M002 alocada");
    CHECK(cancelarMissao("M002","Drone com defeito mecanico") == 0, "M002 cancelada com motivo");
    CHECK(cancelarMissao("M002","tentativa dupla") == -1, "Cancelamento duplo rejeitado");
    dp = buscarDronePorId("D002");
    CHECK(dp && dp->status == DRONE_RETORNANDO, "D002 liberado apos cancelamento");

    printf("\n=== CENARIO 10: Replanejamento ===\n");
    CHECK(alocarRecursos("M003","D003","OP003") == 0, "M003 alocada");
    CHECK(iniciarMissao("M003") == 0, "M003 iniciada");
    Missao mNova = {"M003B","23:00",3,"Quadra Sul",30,TIPO_INSPECAO,"",MISSAO_PENDENTE,"",""};
    CHECK(replanejarMissao("M003", mNova) == 0, "M003 replanejada para M003B");
    CHECK(buscarMissaoPorCodigo("M003B") != NULL, "M003B existe no sistema");

    printf("\n=== CENARIO 11: Encerrar evento e verificar historico ===\n");
    CHECK(encerrarEvento("EV001") == 0, "EV001 encerrado");
    CHECK(buscarEventoPorCodigo("EV001") == NULL, "EV001 removido da lista");
    CHECK(historico.topo >= 0, "Historico nao esta vazio");

    printf("\n=== CENARIO 12: Lista vazia apos remocao ===\n");
    encerrarEvento("EV002");
    CHECK(cabecaEventos->prox == caudaEventos, "Lista de eventos vazia (sentinelas ligadas)");

    printf("\n=== RELATORIOS FINAIS ===\n");
    relEstadoDrones();
    relAgendaDia();
    relOcorrenciasAbertas();
    relHistorico();

    liberarListaEventos();
    printf("\n=== TODOS OS CENARIOS EXECUTADOS ===\n");
    return 0;
}
