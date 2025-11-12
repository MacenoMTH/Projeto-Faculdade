/* gui.h */
#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "structs.h" // Precisamos saber o que é ListaUsuarios

/* --- Protótipos das Funções da GUI --- */
// Este é o "índice" que o main.c vai ler

void carregar_css(void);
void criar_tela_inicial(ListaUsuarios *lista);
void criar_tela_login(ListaUsuarios *lista);
void criar_tela_registro(ListaUsuarios *lista);

#endif
