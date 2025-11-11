#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    int id;
    char endereco[100];
    char tipo[20]; // Casa, Apartamento, etc.
    float preco;
    int quartos;
    int disponivel; // 1 = sim, 0 = não
} Imovel;

typedef struct {
    int id;
    char nome[50];
    char email[50];
    char telefone[15];
} Usuario;

typedef struct {
    Usuario base;
    Imovel *imoveis[10]; // Array de ponteiros para imóveis
    int num_imoveis;
} Locador;

typedef struct {
    Usuario base;
    char preferencias[100]; // Ex: "Casa com 3 quartos"
    float orcamento;
} Comprador;

typedef struct {
    Usuario base;
    char preferencias[100];
    float orcamento_mensal;
} Alugador;

typedef struct {
    int id;
    Imovel *imovel;
    Usuario *comprador_alugador;
    Usuario *locador;
    char tipo[10]; // "Venda" ou "Aluguel"
    float valor;
    char data[11]; // YYYY-MM-DD
} Transacao;

typedef struct {
    Imovel *imoveis[100];
    int tamanho;
} ListaImoveis;

typedef struct {
    Usuario *usuarios[100];
    int tamanho;
} ListaUsuarios;

typedef struct {
    char tipo[20];
    float preco_min;
    float preco_max;
    int quartos_min;
} FiltroBusca;

typedef struct {
    ListaImoveis imoveis;
    ListaUsuarios usuarios;
    Transacao *transacoes[100];
    int num_transacoes;
} SistemaImobiliaria;

// Declarações de funções
void inicializar_sistema(SistemaImobiliaria *sistema);
void adicionar_imovel(SistemaImobiliaria *sistema, Imovel *imovel);
void buscar_imoveis(SistemaImobiliaria *sistema, FiltroBusca *filtro, Imovel **resultados, int *num_resultados);

#endif
