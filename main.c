/* main.c */
#include "structs.h"
#include "gui.h"     // Inclui nosso "índice" de funções da GUI

int main(int argc, char *argv[]) {
    
    // Inicializa o GTK
    gtk_init(&argc, &argv);

    // Carrega o CSS uma vez para todo o app
    carregar_css();

    // Inicializa a lista (como na sua imagem)
    ListaUsuarios lista;
    lista.tamanho = 0; 
    // (ou qualquer outra inicialização que 'structs.h' precise)

    // Chama a NOVA tela inicial, passando a lista
    criar_tela_inicial(&lista);

    // Inicia o loop principal do GTK
    gtk_main();
    
    return 0;
}
