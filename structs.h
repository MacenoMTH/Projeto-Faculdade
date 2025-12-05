#ifndef STRUCTS_H
#define STRUCTS_H

#include <stddef.h>

#define USERS_FILE "users.db"

typedef struct {
    int id;
    char endereco[100];
    char tipo[20];
    float preco;
    int quartos;
    int disponivel; // 1 = sim, 0 = não
} Imovel;

/* UI-oriented Imovel (para exibição) */
typedef struct {
    int id;
    char titulo[150];
    char preco_str[50];
    char descricao[500];
    char imagem_path[100];
    char localizacao[100];
    char tipo[30];
    int quartos;
    int banheiros;
    int area_m2;
} UiImovel;

/* Campos universais que todo usuário tem */
typedef struct {
    int id;
    char nome[50];
    char email[50];
    char senha[50]; /* em demo, plaintext */
    char telefone[20];
    char role[16]; /* "Alugador","Locador","Comprador","Usuário" */
} Usuario;

/* Locador: usuário que anuncia imóveis */
typedef struct {
    Usuario base;
    Imovel *imoveis[10];
    int num_imoveis;
} Locador;

/* Comprador: interessado em comprar */
typedef struct {
    Usuario base;
    char preferencias[200];
    float orcamento;
} Comprador;

/* Alugador: interessado em alugar */
typedef struct {
    Usuario base;
    char preferencias[200];
    float orcamento_mensal;
} Alugador;

typedef struct {
    int id;
    Imovel *imovel;
    Usuario *comprador_alugador;
    Usuario *locador;
    char tipo[16];
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
    Usuario *usuario_logado;
} SistemaImobiliaria;

/* Inicialização / manipulação */
void inicializar_sistema(SistemaImobiliaria *sistema);
void adicionar_imovel(SistemaImobiliaria *sistema, Imovel *imovel);

/* Registrar usuário (tipo: "Alugador"/"Locador"/"Comprador"/"Usuário") */
int registrar_usuario(ListaUsuarios *lista, char *nome, char *email, char *senha,
     char *tipo, char *pref, float orcamento);

/* Persistência de login (cache de credenciais) */
int salvar_login(const char *email, const char *senha);
int carregar_login_salvo(char *email_out, char *senha_out);
void limpar_login_salvo(void);

/* Persistência da lista de usuários (salvar/ler accounts) */
int salvar_usuarios(ListaUsuarios *lista);
int carregar_usuarios(ListaUsuarios *lista);

extern SistemaImobiliaria sistema;

#endif
