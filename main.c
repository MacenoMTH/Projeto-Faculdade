#include "structs.h"
#include "gui.h"
#include <string.h>

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    carregar_css();

    /* carregar usuários salvos (persistência) */
    ListaUsuarios lista;
    lista.tamanho = 0;
    carregar_usuarios(&lista); 
    /* preenche lista. se arquivo não existir, lista fica vazia */

    /* Sempre abrir na TELA INICIAL (sem auto-login) */
    criar_tela_inicial(&lista);
    // Observação: o sistema ainda salva credenciais no registro/login,
    // mas a abertura automática foi desativada conforme pedido.

    gtk_main();
    return 0;
}