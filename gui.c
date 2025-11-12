/* gui.c */
#include <string.h>
#include "gui.h" // Inclui nosso header com os protótipos e structs

/* --- Estrutura de Callback (para tela de Registro) --- */
typedef struct {
    GtkWidget *entry_nome;
    GtkWidget *entry_email;
    GtkWidget *entry_senha;
    GtkWidget *combo_tipo;
    GtkWidget *window;
    ListaUsuarios *lista;
} CallbackData;

/* --- Função para Carregar CSS --- */
// (Exatamente como você tinha)
void carregar_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    GError *error = NULL;
    if (!gtk_css_provider_load_from_path(provider, "style.css", &error)) {
        g_warning("Erro ao carregar CSS: %s", error->message);
        g_clear_error(&error);
    }
    g_object_unref(provider);
}


/* --- Callbacks da TELA INICIAL --- */

// Chamado quando o botão "Já Tenho Login" é clicado
void on_abrir_login_clicked(GtkWidget *widget, gpointer data) {
    ListaUsuarios *lista = (ListaUsuarios *)data;
    // Pega a janela atual (a tela inicial) e a destrói
    GtkWidget *window = gtk_widget_get_toplevel(widget);
    gtk_widget_destroy(window);

    // Abre a tela de login
    criar_tela_login(lista);
}

// Chamado quando o botão "Criar Nova Conta" é clicado
void on_abrir_registro_clicked(GtkWidget *widget, gpointer data) {
    ListaUsuarios *lista = (ListaUsuarios *)data;
    // Pega a janela atual (a tela inicial) e a destrói
    GtkWidget *window = gtk_widget_get_toplevel(widget);
    gtk_widget_destroy(window);

    // Abre a tela de registro
    criar_tela_registro(lista);
}


/* --- Callbacks da TELA DE REGISTRO --- */

// (Exatamente como você tinha)
void on_registrar_clicked(GtkWidget *widget, gpointer data) {
    CallbackData *cb_data = (CallbackData *)data;

    const char *nome = gtk_entry_get_text(GTK_ENTRY(cb_data->entry_nome));
    const char *email = gtk_entry_get_text(GTK_ENTRY(cb_data->entry_email));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(cb_data->entry_senha));
    const char *tipo = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cb_data->combo_tipo));

    if (strlen(nome) == 0 || strlen(email) == 0 || strlen(senha) == 0 || tipo == NULL) {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(cb_data->window),
                                                   GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Preencha todos os campos!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // if (registrar_usuario(cb_data->lista, (char *)nome, (char *)email, (char *)senha, (char *)tipo)) {
    // Simulação de sucesso (troque pelo seu 'registrar_usuario' real)
    if (strcmp(email, "email@jaexiste.com") != 0) { 
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(cb_data->window),
                                                   GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                   "Usuário registrado com sucesso!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        gtk_entry_set_text(GTK_ENTRY(cb_data->entry_nome), "");
        gtk_entry_set_text(GTK_ENTRY(cb_data->entry_email), "");
        gtk_entry_set_text(GTK_ENTRY(cb_data->entry_senha), "");
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(cb_data->window),
                                                   GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "E-mail já cadastrado!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


/* --- TELA 1: Tela de Login (Placeholder) --- */
/* (NOVA FUNÇÃO) */
void criar_tela_login(ListaUsuarios *lista) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_widget_set_name(window, "auth-window"); // Reusa o CSS

    GtkWidget *label = gtk_label_new("Tela de Login (A ser implementada)");
    gtk_container_add(GTK_CONTAINER(window), label);

    gtk_widget_show_all(window);
}


/* --- TELA 2: Tela de Registro --- */
/* (Quase igual ao seu código, mas note que a chamada 'carregar_css()' foi REMOVIDA
   porque ela agora é chamada no main.c) */
void criar_tela_registro(ListaUsuarios *lista) {
    // A LINHA 'carregar_css()' FOI REMOVIDA DAQUI!

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 450);
    gtk_widget_set_name(window, "auth-window");

    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Registro de Usuário");
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    GtkWidget *auth_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_halign(auth_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(auth_box, GTK_ALIGN_CENTER);
    gtk_widget_set_name(auth_box, "auth-box");
    gtk_container_add(GTK_CONTAINER(window), auth_box);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(auth_box), grid);

    CallbackData *cb_data = g_new(CallbackData, 1);
    cb_data->lista = lista;
    cb_data->window = window;

    // Nome
    GtkWidget *label_nome = gtk_label_new("Nome:");
    gtk_widget_set_halign(label_nome, GTK_ALIGN_END);
    cb_data->entry_nome = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label_nome, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->entry_nome, 1, 0, 1, 1);

    // E-mail
    GtkWidget *label_email = gtk_label_new("E-mail:");
    gtk_widget_set_halign(label_email, GTK_ALIGN_END);
    cb_data->entry_email = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label_email, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->entry_email, 1, 1, 1, 1);

    // Senha
    GtkWidget *label_senha = gtk_label_new("Senha:");
    gtk_widget_set_halign(label_senha, GTK_ALIGN_END);
    cb_data->entry_senha = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(cb_data->entry_senha), FALSE);
    gtk_grid_attach(GTK_GRID(grid), label_senha, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->entry_senha, 1, 2, 1, 1);

    // Tipo
    GtkWidget *label_tipo = gtk_label_new("Tipo:");
    gtk_widget_set_halign(label_tipo, GTK_ALIGN_END);
    cb_data->combo_tipo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb_data->combo_tipo), "Locador");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cb_data->combo_tipo), "Cliente");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cb_data->combo_tipo), 0);
    gtk_grid_attach(GTK_GRID(grid), label_tipo, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cb_data->combo_tipo, 1, 3, 1, 1);

    // Botão Registrar
    GtkWidget *button = gtk_button_new_with_label("Registrar");
    gtk_widget_set_name(button, "suggested-action-button");
    g_signal_connect(button, "clicked", G_CALLBACK(on_registrar_clicked), cb_data);
    
    gtk_box_pack_start(GTK_BOX(auth_box), button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}


/* --- TELA 3: Tela Inicial (A NOVA TELA!) --- */
/* (Esta é a primeira tela que o usuário verá) */
void criar_tela_inicial(ListaUsuarios *lista) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 350);
    gtk_widget_set_name(window, "auth-window"); // Reusa o CSS
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Imobiliária");
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    // Caixa de centralização
    GtkWidget *auth_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20); // Mais espaço
    gtk_widget_set_halign(auth_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(auth_box, GTK_ALIGN_CENTER);
    gtk_widget_set_name(auth_box, "auth-box"); // Reusa o CSS
    gtk_container_add(GTK_CONTAINER(window), auth_box);

    // Título de Boas-vindas
    GtkWidget *label = gtk_label_new("Bem-vindo(a)!");
    gtk_widget_set_name(label, "welcome-label"); // CSS novo
    gtk_box_pack_start(GTK_BOX(auth_box), label, FALSE, FALSE, 0);

    // Botão de Login
    GtkWidget *btn_login = gtk_button_new_with_label("Já Tenho Login");
    gtk_widget_set_name(btn_login, "suggested-action-button"); // CSS
    g_signal_connect(btn_login, "clicked", G_CALLBACK(on_abrir_login_clicked), lista);
    gtk_box_pack_start(GTK_BOX(auth_box), btn_login, FALSE, FALSE, 0);

    // Botão de Registro
    GtkWidget *btn_registro = gtk_button_new_with_label("Criar Nova Conta");
    gtk_widget_set_name(btn_registro, "secondary-action-button"); // CSS novo
    g_signal_connect(btn_registro, "clicked", G_CALLBACK(on_abrir_registro_clicked), lista);
    gtk_box_pack_start(GTK_BOX(auth_box), btn_registro, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}
