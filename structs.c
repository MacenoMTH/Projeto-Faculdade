#include <stdlib.h>
#include <string.h>
#include "structs.h"

// Definir variável global
SistemaImobiliaria sistema;

void inicializar_sistema(SistemaImobiliaria *sistema) {
    sistema->imoveis.tamanho = 0;
    sistema->usuarios.tamanho = 0;
    sistema->num_transacoes = 0;
}

void adicionar_imovel(SistemaImobiliaria *sistema, Imovel *imovel) {
    if (sistema->imoveis.tamanho < 100) {
        sistema->imoveis.imoveis[sistema->imoveis.tamanho] = imovel;
        sistema->imoveis.tamanho++;
    }
}

int registrar_usuario(ListaUsuarios *lista, char *nome, char *email, char *senha, char *tipo) {
    // Verificar se e-mail já existe
    for (int i = 0; i < lista->tamanho; i++) {
        if (strcmp(lista->usuarios[i]->email, email) == 0) {
            return 0; // Já existe
        }
    }

    // Criar novo usuário
    Usuario *novo = malloc(sizeof(Usuario));
    novo->id = lista->tamanho + 1;
    strcpy(novo->nome, nome);
    strcpy(novo->email, email);
    strcpy(novo->senha, senha); // Em produção, use hash!
    strcpy(novo->telefone, "");

    // Adicionar à lista
    if (lista->tamanho < 100) {
        lista->usuarios[lista->tamanho] = novo;
        lista->tamanho++;
        return 1; // Sucesso
    }
    
    free(novo);
    return 0; // Lista cheia
}
