#include <gtk/gtk.h>
#include <string.h>
// Assumo que "structs.h" contenha a definição de ListaUsuarios
// e a função registrar_usuario()
#include "structs.h" 

// Estrutura para passar os dados para o callback
// Isso é mais limpo do que usar g_object_set_data
typedef struct {
    GtkWidget *entry_nome;
    GtkWidget *entry_email;
    GtkWidget *entry_senha;
    GtkWidget *combo_tipo;
    GtkWidget *window; // Para ser usado como pai dos diálogos
    ListaUsuarios *lista;
} CallbackData;

// Função de callback (quase igual, mas agora usa a struct)
void on_registrar_clicked(GtkWidget *widget, gpointer data) {
    CallbackData *cb_data = (CallbackData *)data;

    // Obter valores dos campos
    const char *nome = gtk_entry_get_text(GTK_ENTRY(cb_data->entry_nome));
    const char *email = gtk_entry_get_text(GTK_ENTRY(cb_data->entry_email));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(cb_data->entry_senha));
    const char *tipo = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cb_data->combo_tipo));

    // Validar campos
    if (strlen(nome) == 0 || strlen(email) == 0 || strlen(senha) == 0 || tipo == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(cb_data->window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
                                                   "Preencha todos os campos!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Registrar usuário
    if (registrar_usuario(cb_data->lista, (char *)nome, (char *)email, (char *)senha, (char *)tipo)) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(cb_data->window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_INFO,
                                                   GTK_BUTTONS_OK,
                                                   "Usuário registrado com sucesso!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // Limpar campos
        gtk_entry_set_text(GTK_ENTRY(cb_data->entry_nome), "");
        gtk_entry_set_text(GTK_ENTRY(cb_data->entry_email), "");
        gtk_entry_set_text(GTK_ENTRY(cb_data->entry_senha), "");
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(cb_data->window),
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_OK,
"E-mail já cadastrado!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

// Função para carregar o CSS
void carregar_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Carrega o arquivo CSS. Salve o CSS (da próxima seção) 
    // em um arquivo chamado "style.css" no mesmo diretório.
    GError *error = NULL;
    gtk_css_provider_load_from_path(provider, "style.css", &error);
    if (error) {
        g_warning("Erro ao carregar CSS: %s", error->message);
        g_clear_error(&error);
    }
    g_object_unref(provider);
}

void criar_tela_registro(ListaUsuarios *lista) {
    // Inicializa o carregador de CSS
    carregar_css();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 450);
    // Adiciona uma classe CSS à janela para estilização
    gtk_widget_set_name(window, "auth-window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 1. Cabeçalho (HeaderBar)
    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Registro de Usuário");
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    // 2. Caixa de Centralização
    // Esta caixa (auth_box) vai conter o formulário e centralizá-lo.
    GtkWidget *auth_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_halign(auth_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(auth_box, GTK_ALIGN_CENTER);
    // Adiciona uma classe CSS para o "card" de login
    gtk_widget_set_name(auth_box, "auth-box");
    gtk_container_add(GTK_CONTAINER(window), auth_box);

    // 3. O Grid do Formulário
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(auth_box), grid);

    // Alocar a struct de dados do callback
    CallbackData *cb_data = g_new(CallbackData, 1);
    cb_data->lista = lista;
    cb_data->window = window;

    // --- Campo Nome ---
    GtkWidget *label_nome = gtk_label_new("Nome:");
    gtk_widget_set_halign(label_nome, GTK_ALIGN_END);
    cb_data->entry_nome = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label_nome, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->entry_nome, 1, 0, 1, 1);

    // --- Campo E-mail ---
    GtkWidget *label_email = gtk_label_new("E-mail:");
    gtk_widget_set_halign(label_email, GTK_ALIGN_END);
    cb_data->entry_email = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label_email, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->entry_email, 1, 1, 1, 1);

    // --- Campo Senha ---
    GtkWidget *label_senha = gtk_label_new("Senha:");
    gtk_widget_set_halign(label_senha, GTK_ALIGN_END);
    cb_data->entry_senha = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(cb_data->entry_senha), FALSE);
    gtk_grid_attach(GTK_GRID(grid), label_senha, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->entry_senha, 1, 2, 1, 1);

    // --- Combo Tipo ---
    GtkWidget *label_tipo = gtk_label_new("Tipo:");
    gtk_widget_set_halign(label_tipo, GTK_ALIGN_END);
    cb_data->combo_tipo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb_data->combo_tipo), "Locador");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb_data->combo_tipo), "Cliente");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cb_data->combo_tipo), 0);
    gtk_grid_attach(GTK_GRID(grid), label_tipo, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->combo_tipo, 1, 3, 1, 1);

    // 4. Botão Registrar
    GtkWidget *button = gtk_button_new_with_label("Registrar");
    // Adiciona uma classe CSS para o botão de ação principal
    gtk_widget_set_name(button, "suggested-action-button");
    // Conecta o sinal, passando a struct cb_data como dado
    g_signal_connect(button, "clicked", G_CALLBACK(on_registrar_clicked), cb_data);
    
    // Adiciona o botão *depois* do grid, dentro da auth_box
    gtk_box_pack_start(GTK_BOX(auth_box), button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

// Função main (exemplo de como iniciar)
/*
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Inicializar sua lista de usuários (exemplo)
    ListaUsuarios *minha_lista = g_new(ListaUsuarios, 1);
    minha_lista->inicio = NULL; 
    minha_lista->total = 0;

    criar_tela_registro(minha_lista);
    gtk_main();

    // (Aqui você deveria liberar a memória da lista, etc.)
    g_free(minha_lista);
    return 0;
}
*/
