#ifndef HOME_H
#define HOME_H

#include <gtk/gtk.h>
#include "structs.h"

/* Estrutura usada pela UI para representar um imóvel localmente
   (mantém compatibilidade com structs.h - é apenas para a camada visual) */
typedef struct {
    int id;
    char *titulo;
    char *preco;
    char *descricao;
    char *imagem_path;
    char *localizacao;
    char *tipo;        // ex: "Apartamento", "Casa", "Loft"
    int quartos;
    int banheiros;
    int area_m2;
} UiImovel;

/* Protótipos */
void criar_tela_home(ListaUsuarios *lista);
GtkWidget* criar_secao_imoveis(const UiImovel **imoveis, int n_imoveis);
GtkWidget* criar_card_imovel(const UiImovel *imovel);
void mostrar_detalhes_imovel(GtkWidget *parent, const UiImovel *imovel);

#endif // HOME_H
