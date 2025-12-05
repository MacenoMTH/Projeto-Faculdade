#ifndef HOME_H
#define HOME_H

#include <gtk/gtk.h>
#include "structs.h"  // UiImovel agora vem daqui

/* Protótipos públicos da UI Home */
void criar_tela_home(ListaUsuarios *lista);
GtkWidget* criar_secao_imoveis(const UiImovel **imoveis, int n_imoveis);
GtkWidget* criar_card_imovel(const UiImovel *imovel);
void mostrar_detalhes_imovel(GtkWidget *parent, const UiImovel *imovel);

#endif // HOME_H
