#include <gtk/gtk.h>
#include "structs.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Inicializar lista de usuários
    ListaUsuarios lista;
    lista.tamanho = 0;

    criar_tela_registro(&lista);

    gtk_main();
    return 0;
}
