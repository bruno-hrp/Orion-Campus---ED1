- drones {
    id;
    modelo;
    autonomia;
    nivelDisponibilidade;
    capacidade;
    status;
    setorVinc;
    atividadesVinculadas;
}

- operadores {
    mat;
    nome;
    certificacao;
    turno;
    situacaoOperacional;
}

- missao {
    codigo;
    horario;
    prioridade;
    area;
    duraçao;
    tipo;
    recursoPreferencial;
}

- eventosDinamicos {
    solicitacao;
    indicioInvasao;
    riscoEstrutural;
    alagamento;
    necessidadeInspecao;
}

- historicoOperacional {
    missoesConcluidas;
    missoesCanceladas;
    missoesInterrompidas;
    missoesReplanejadas;
}

- tiposOrdenacao {
    prioridade;
    horario;
    código;
    id;
}

- relatorioSupervisao {
    visaoGeral;
    agendaOperacional; //diária
    missoesPendentes;
    ocorrenciasAbertas;
}

/*
Sistema
- O que já foi planejado;
- o que foi dinâmico;
- dados consultados e reorganizados;
*/