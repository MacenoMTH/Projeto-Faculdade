#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "structs.h"

SistemaImobiliaria sistema;

#define LOGIN_FILE "login_cache.txt"

/* --- Inicialização simples --- */
void inicializar_sistema(SistemaImobiliaria *sistema) {
    if (!sistema) return;
    sistema->imoveis.tamanho = 0;
    sistema->usuarios.tamanho = 0;
    sistema->num_transacoes = 0;
    sistema->usuario_logado = NULL;
}

void adicionar_imovel(SistemaImobiliaria *sistema_g, Imovel *imovel) {
    if (!sistema_g || !imovel) return;
    if (sistema_g->imoveis.tamanho < 100) {
        sistema_g->imoveis.imoveis[sistema_g->imoveis.tamanho++] = imovel;
    }
}

/* registrar_usuario: cria struct correta conforme role e adiciona na lista */
int registrar_usuario(ListaUsuarios *lista, char *nome, char *email, char *senha, char *tipo, char *pref, float orcamento) {
    if (!lista || !nome || !email || !senha) return 0;

    /* verifica duplicado por e-mail */
    for (int i = 0; i < lista->tamanho; ++i) {
        if (lista->usuarios[i] && strcmp(lista->usuarios[i]->email, email) == 0) return 0;
    }

    Usuario *base_ptr = NULL;

    if (tipo && strcmp(tipo, "Alugador") == 0) {
        Alugador *a = malloc(sizeof(Alugador));
        if (!a) return 0;
        memset(a, 0, sizeof(*a));
        a->base.id = lista->tamanho + 1;
        strncpy(a->base.nome, nome, sizeof(a->base.nome)-1);
        strncpy(a->base.email, email, sizeof(a->base.email)-1);
        strncpy(a->base.senha, senha, sizeof(a->base.senha)-1);
        a->base.telefone[0] = 0;
        strncpy(a->base.role, "Alugador", sizeof(a->base.role)-1);
        if (pref) strncpy(a->preferencias, pref, sizeof(a->preferencias)-1);
        a->orcamento_mensal = orcamento;
        base_ptr = (Usuario*)a;
    } else if (tipo && strcmp(tipo, "Locador") == 0) {
        Locador *l = malloc(sizeof(Locador));
        if (!l) return 0;
        memset(l, 0, sizeof(*l));
        l->base.id = lista->tamanho + 1;
        strncpy(l->base.nome, nome, sizeof(l->base.nome)-1);
        strncpy(l->base.email, email, sizeof(l->base.email)-1);
        strncpy(l->base.senha, senha, sizeof(l->base.senha)-1);
        l->base.telefone[0] = 0;
        strncpy(l->base.role, "Locador", sizeof(l->base.role)-1);
        l->num_imoveis = 0;
        base_ptr = (Usuario*)l;
    } else if (tipo && strcmp(tipo, "Comprador") == 0) {
        Comprador *c = malloc(sizeof(Comprador));
        if (!c) return 0;
        memset(c, 0, sizeof(*c));
        c->base.id = lista->tamanho + 1;
        strncpy(c->base.nome, nome, sizeof(c->base.nome)-1);
        strncpy(c->base.email, email, sizeof(c->base.email)-1);
        strncpy(c->base.senha, senha, sizeof(c->base.senha)-1);
        c->base.telefone[0] = 0;
        strncpy(c->base.role, "Comprador", sizeof(c->base.role)-1);
        if (pref) strncpy(c->preferencias, pref, sizeof(c->preferencias)-1);
        c->orcamento = orcamento;
        base_ptr = (Usuario*)c;
    } else {
        Usuario *u = malloc(sizeof(Usuario));
        if (!u) return 0;
        memset(u, 0, sizeof(*u));
        u->id = lista->tamanho + 1;
        strncpy(u->nome, nome, sizeof(u->nome)-1);
        strncpy(u->email, email, sizeof(u->email)-1);
        strncpy(u->senha, senha, sizeof(u->senha)-1);
        u->telefone[0] = 0;
        strncpy(u->role, "Usuário", sizeof(u->role)-1);
        base_ptr = u;
    }

    if (lista->tamanho < 100) {
        lista->usuarios[lista->tamanho++] = base_ptr;
        /* salvar lista completa em disco para persistência */
        salvar_usuarios(lista);
        return 1;
    }

    free(base_ptr);
    return 0;
}

/* --- Persistência de login (cache simples) --- */
int salvar_login(const char *email, const char *senha) {
    FILE *f = fopen(LOGIN_FILE, "w");
    if (!f) return 0;
    fprintf(f, "%s\n%s\n", email, senha);
    fclose(f);
    return 1;
}

int carregar_login_salvo(char *email_out, char *senha_out) {
    FILE *f = fopen(LOGIN_FILE, "r");
    if (!f) return 0;
    if (fscanf(f, "%49s\n%49s\n", email_out, senha_out) != 2) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

void limpar_login_salvo(void) {
    remove(LOGIN_FILE);
}

/* --- Persistência da lista de usuários em texto simples (tab-separated) --- */
/* Formato por linha:
   role \t id \t nome \t email \t senha \t telefone \t preferencias \t orcamento \t orcamento_mensal \t num_imoveis\n
   Campos não aplicáveis ficam vazios.
*/

int salvar_usuarios(ListaUsuarios *lista) {
    if (!lista) return 0;
    FILE *f = fopen(USERS_FILE, "w");
    if (!f) return 0;

    for (int i = 0; i < lista->tamanho; ++i) {
        Usuario *u = lista->usuarios[i];
        if (!u) continue;
        const char *role = u->role;
        /* valores padrão */
        const char *telefone = u->telefone;
        const char *nome = u->nome;
        const char *email = u->email;
        const char *senha = u->senha;
        char preferencias[256] = "";
        double orc = 0.0;
        double orc_mensal = 0.0;
        int num_imoveis = 0;

        if (strcmp(role, "Alugador") == 0) {
            Alugador *a = (Alugador*)u;
            strncpy(preferencias, a->preferencias, sizeof(preferencias)-1);
            orc_mensal = a->orcamento_mensal;
        } else if (strcmp(role, "Comprador") == 0) {
            Comprador *c = (Comprador*)u;
            strncpy(preferencias, c->preferencias, sizeof(preferencias)-1);
            orc = c->orcamento;
        } else if (strcmp(role, "Locador") == 0) {
            Locador *l = (Locador*)u;
            num_imoveis = l->num_imoveis;
        }

        /* escape tabs/newlines na prática simples: substituir '\t' por ' ' */
        for (char *p = preferencias; *p; ++p) if (*p == '\t' || *p == '\n') *p = ' ';

        fprintf(f, "%s\t%d\t%s\t%s\t%s\t%s\t%s\t%.2f\t%.2f\t%d\n",
                role, u->id, nome, email, senha, telefone ? telefone : "", preferencias, orc, orc_mensal, num_imoveis);
    }

    fclose(f);
    return 1;
}

int carregar_usuarios(ListaUsuarios *lista) {
    if (!lista) return 0;
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) return 0;

    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        /* remover newline final */
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';

        char *fields[10] = {0};
        char *p = line;
        for (int i = 0; i < 10; ++i) {
            char *tab = strchr(p, '\t');
            if (tab) {
                *tab = '\0';
                fields[i] = p;
                p = tab + 1;
            } else {
                fields[i] = p;
                break;
            }
        }

        const char *role = fields[0] ? fields[0] : "Usuário";
        int id = fields[1] ? atoi(fields[1]) : (lista->tamanho + 1);
        const char *nome = fields[2] ? fields[2] : "";
        const char *email = fields[3] ? fields[3] : "";
        const char *senha = fields[4] ? fields[4] : "";
        const char *telefone = fields[5] ? fields[5] : "";
        const char *preferencias = fields[6] ? fields[6] : "";
        double orc = fields[7] ? atof(fields[7]) : 0.0;
        double orc_mensal = fields[8] ? atof(fields[8]) : 0.0;
        int num_imoveis = fields[9] ? atoi(fields[9]) : 0;

        Usuario *base_ptr = NULL;

        if (strcmp(role, "Alugador") == 0) {
            Alugador *a = malloc(sizeof(Alugador));
            if (!a) continue;
            memset(a,0,sizeof(*a));
            a->base.id = id;
            strncpy(a->base.nome, nome, sizeof(a->base.nome)-1);
            strncpy(a->base.email, email, sizeof(a->base.email)-1);
            strncpy(a->base.senha, senha, sizeof(a->base.senha)-1);
            strncpy(a->base.telefone, telefone, sizeof(a->base.telefone)-1);
            strncpy(a->base.role, "Alugador", sizeof(a->base.role)-1);
            strncpy(a->preferencias, preferencias, sizeof(a->preferencias)-1);
            a->orcamento_mensal = (float)orc_mensal;
            base_ptr = (Usuario*)a;
        } else if (strcmp(role, "Locador") == 0) {
            Locador *l = malloc(sizeof(Locador));
            if (!l) continue;
            memset(l,0,sizeof(*l));
            l->base.id = id;
            strncpy(l->base.nome, nome, sizeof(l->base.nome)-1);
            strncpy(l->base.email, email, sizeof(l->base.email)-1);
            strncpy(l->base.senha, senha, sizeof(l->base.senha)-1);
            strncpy(l->base.telefone, telefone, sizeof(l->base.telefone)-1);
            strncpy(l->base.role, "Locador", sizeof(l->base.role)-1);
            l->num_imoveis = num_imoveis;
            for (int j=0;j<10;j++) l->imoveis[j]=NULL;
            base_ptr = (Usuario*)l;
        } else if (strcmp(role, "Comprador") == 0) {
            Comprador *c = malloc(sizeof(Comprador));
            if (!c) continue;
            memset(c,0,sizeof(*c));
            c->base.id = id;
            strncpy(c->base.nome, nome, sizeof(c->base.nome)-1);
            strncpy(c->base.email, email, sizeof(c->base.email)-1);
            strncpy(c->base.senha, senha, sizeof(c->base.senha)-1);
            strncpy(c->base.telefone, telefone, sizeof(c->base.telefone)-1);
            strncpy(c->base.role, "Comprador", sizeof(c->base.role)-1);
            strncpy(c->preferencias, preferencias, sizeof(c->preferencias)-1);
            c->orcamento = (float)orc;
            base_ptr = (Usuario*)c;
        } else {
            Usuario *u = malloc(sizeof(Usuario));
            if (!u) continue;
            memset(u,0,sizeof(*u));
            u->id = id;
            strncpy(u->nome, nome, sizeof(u->nome)-1);
            strncpy(u->email, email, sizeof(u->email)-1);
            strncpy(u->senha, senha, sizeof(u->senha)-1);
            strncpy(u->telefone, telefone, sizeof(u->telefone)-1);
            strncpy(u->role, "Usuário", sizeof(u->role)-1);
            base_ptr = u;
        }

        if (lista->tamanho < 100) {
            lista->usuarios[lista->tamanho++] = base_ptr;
        } else {
            free(base_ptr);
        }
    }

    fclose(f);
    return 1;
}
