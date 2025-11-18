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
    char senha[50]; // Adicionei campo senha
} Usuario;

typedef struct {
    Usuario base;
    Imovel *imoveis[10];
    int num_imoveis;
} Locador;

typedef struct {
    Usuario base;
    char preferencias[100];
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
    char tipo[10];
    float valor;
    char data[11];
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
int registrar_usuario(ListaUsuarios *lista, char *nome, char *email, char *senha, char *tipo);
void criar_tela_registro(ListaUsuarios *lista);

// Variável global
extern SistemaImobiliaria sistema;

#endif
