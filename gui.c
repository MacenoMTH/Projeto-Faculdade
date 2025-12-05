/* gui.c */
#include <string.h>
#include "gui.h"
#include <gdk-pixbuf/gdk-pixbuf.h> // carregar logo
#include <gtk/gtk.h>

/* --- Helper local: carrega imagem preservando proporção --- */
static GdkPixbuf* carregar_pixbuf_preservar(const char *path, int max_w, int max_h) {
    if (!path) return NULL;
    GError *err = NULL;
    GdkPixbuf *pix = gdk_pixbuf_new_from_file(path, &err);
    if (!pix) {
        g_clear_error(&err);
        return NULL;
    }
    int w = gdk_pixbuf_get_width(pix);
    int h = gdk_pixbuf_get_height(pix);
    if (w <= 0 || h <= 0) { g_object_unref(pix); return NULL; }

    double scale_w = (double)max_w / (double)w;
    double scale_h = (double)max_h / (double)h;
    double scale = (scale_w < scale_h) ? scale_w : scale_h;
    if (scale > 1.0) scale = 1.0;

    int nw = (int)(w * scale);
    int nh = (int)(h * scale);

    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(pix, nw, nh, GDK_INTERP_BILINEAR);
    g_object_unref(pix);
    return scaled;
}

/* --- Função para Carregar CSS --- */
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
    GtkWidget *window = gtk_widget_get_toplevel(widget);
    gtk_widget_destroy(window);
    criar_tela_login(lista);
}

// Chamado quando o botão "Criar Nova Conta" é clicado
void on_abrir_registro_clicked(GtkWidget *widget, gpointer data) {
    ListaUsuarios *lista = (ListaUsuarios *)data;
    GtkWidget *window = gtk_widget_get_toplevel(widget);
    gtk_widget_destroy(window);
    criar_tela_registro(lista);
}

/* Voltar para tela inicial (usado em login/registro) */
void on_voltar_para_inicial_clicked(GtkWidget *widget, gpointer data) {
    ListaUsuarios *lista = (ListaUsuarios *)data;
    GtkWidget *window = gtk_widget_get_toplevel(widget);
    gtk_widget_destroy(window);
    criar_tela_inicial(lista);
}

/* --- Helpers para registro dinâmico --- */
static void on_role_changed(GtkComboBox *combo, gpointer user_data) {
    GtkWidget *alugador_box = GTK_WIDGET(user_data);
    gchar *active = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
    if (active && strcmp(active, "Alugador") == 0) {
        gtk_widget_show(alugador_box);
    } else {
        gtk_widget_hide(alugador_box);
    }
    if (active) g_free(active);
}

/* --- Novos handlers: submit login / submit registro --- */
static void on_login_submit_clicked(GtkWidget *widget, gpointer data) {
    ListaUsuarios *lista = (ListaUsuarios *)data;
    GtkWidget *window = gtk_widget_get_toplevel(widget);

    GtkWidget *entry_email = g_object_get_data(G_OBJECT(window), "entry_email");
    GtkWidget *entry_senha = g_object_get_data(G_OBJECT(window), "entry_senha");
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));

    for (int i = 0; i < lista->tamanho; ++i) {
        Usuario *u = lista->usuarios[i];
        if (u && strcmp(u->email, email) == 0 && strcmp(u->senha, senha) == 0) {
            gtk_widget_destroy(window);
            criar_tela_home(lista);
            return;
        }
    }

    GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(window),
                                            GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_WARNING,
                                            GTK_BUTTONS_OK,
                                            "Credenciais inválidas.");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

static void on_registro_submit_clicked(GtkWidget *widget, gpointer data) {
    ListaUsuarios *lista = (ListaUsuarios *)data;
    GtkWidget *window = gtk_widget_get_toplevel(widget);

    GtkWidget *entry_nome = g_object_get_data(G_OBJECT(window), "entry_nome");
    GtkWidget *entry_email = g_object_get_data(G_OBJECT(window), "entry_email");
    GtkWidget *entry_senha = g_object_get_data(G_OBJECT(window), "entry_senha");
    GtkWidget *combo_role = g_object_get_data(G_OBJECT(window), "combo_role");
    GtkWidget *entry_pref = g_object_get_data(G_OBJECT(window), "entry_pref");
    GtkWidget *entry_orc = g_object_get_data(G_OBJECT(window), "entry_orc");

    const char *nome = gtk_entry_get_text(GTK_ENTRY(entry_nome));
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));
    const char *role = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_role));
    const char *pref = entry_pref ? gtk_entry_get_text(GTK_ENTRY(entry_pref)) : NULL;
    const char *orc_txt = entry_orc ? gtk_entry_get_text(GTK_ENTRY(entry_orc)) : NULL;
    float orc = 0.0f;
    if (orc_txt && strlen(orc_txt) > 0) orc = (float) atof(orc_txt);

    if (strlen(nome) == 0 || strlen(email) == 0 || strlen(senha) == 0) {
        GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(window),
                                                GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_WARNING,
                                                GTK_BUTTONS_OK,
                                                "Preencha todos os campos obrigatórios.");
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        if (role) g_free((gchar*)role);
        return;
    }

    int ok = registrar_usuario(lista, (char*)nome, (char*)email, (char*)senha, (char*)(role?role:""), (char*)(pref?pref:""), orc);
    if (role) g_free((gchar*)role);

    if (!ok) {
        GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(window),
                                                GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_ERROR,
                                                GTK_BUTTONS_OK,
                                                "Falha ao registrar (e-mail já existe ou lista cheia).");
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        return;
    }

    /* salvar login para persistência */
    salvar_login(email, senha);

    GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(window),
                                            GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_INFO,
                                            GTK_BUTTONS_OK,
                                            "Conta criada com sucesso!");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);

    gtk_widget_destroy(window);
    criar_tela_home(lista);
}

/* --- TELA DE REGISTRO (agora com formulário e escolha de role) --- */
void criar_tela_registro(ListaUsuarios *lista) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Registro");
    gtk_window_set_default_size(GTK_WINDOW(window), 460, 460);
    gtk_widget_set_name(window, "auth-window");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GdkPixbuf *pix = NULL;
    pix = carregar_pixbuf_preservar("imagens/logo.jpg", 320, 120);
    if (!pix) pix = carregar_pixbuf_preservar("imagens/imovel1.jpg", 320, 120);
    if (pix) {
        GtkWidget *logo = gtk_image_new_from_pixbuf(pix);
        gtk_widget_set_name(logo, "auth-logo");
        g_object_unref(pix);
        gtk_box_pack_start(GTK_BOX(vbox), logo, FALSE, FALSE, 4);
    }

    GtkWidget *label = gtk_label_new("Criar Conta");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 4);

    GtkWidget *entry_nome = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_nome), "Nome completo");
    gtk_box_pack_start(GTK_BOX(vbox), entry_nome, FALSE, FALSE, 4);

    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "E-mail");
    gtk_box_pack_start(GTK_BOX(vbox), entry_email, FALSE, FALSE, 4);

    GtkWidget *entry_senha = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_senha), "Senha");
    gtk_entry_set_visibility(GTK_ENTRY(entry_senha), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), entry_senha, FALSE, FALSE, 4);

    /* Role selector (removi opção "Usuário" — não faz sentido) */
    GtkWidget *combo_role = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_role), "Alugador");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_role), "Locador");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_role), "Comprador");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_role), 0);
    gtk_box_pack_start(GTK_BOX(vbox), combo_role, FALSE, FALSE, 4);

    /* Campos extras para Alugador (inicialmente escondidos) */
    GtkWidget *alugador_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_halign(alugador_box, GTK_ALIGN_START);

    GtkWidget *entry_pref = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_pref), "Preferências (ex: bairro, tipo)");
    gtk_box_pack_start(GTK_BOX(alugador_box), entry_pref, FALSE, FALSE, 0);

    GtkWidget *entry_orc = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_orc), "Orçamento mensal (ex: 2500)");
    gtk_box_pack_start(GTK_BOX(alugador_box), entry_orc, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), alugador_box, FALSE, FALSE, 4);
    gtk_widget_hide(alugador_box);

    /* armazenar widgets para o callback */
    g_object_set_data(G_OBJECT(window), "entry_nome", entry_nome);
    g_object_set_data(G_OBJECT(window), "entry_email", entry_email);
    g_object_set_data(G_OBJECT(window), "entry_senha", entry_senha);
    g_object_set_data(G_OBJECT(window), "combo_role", combo_role);
    g_object_set_data(G_OBJECT(window), "entry_pref", entry_pref);
    g_object_set_data(G_OBJECT(window), "entry_orc", entry_orc);

    /* conectar mudança de role para mostrar/ocultar campos */
    g_signal_connect(combo_role, "changed", G_CALLBACK(on_role_changed), alugador_box);

    GtkWidget *btn_criar = gtk_button_new_with_label("Criar Conta");
    gtk_widget_set_name(btn_criar, "primary-action");
    g_signal_connect(btn_criar, "clicked", G_CALLBACK(on_registro_submit_clicked), lista);
    gtk_box_pack_start(GTK_BOX(vbox), btn_criar, FALSE, FALSE, 6);

    GtkWidget *btn_voltar = gtk_button_new_with_label("← Voltar");
    gtk_widget_set_name(btn_voltar, "secondary-action");
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(on_voltar_para_inicial_clicked), lista);
    gtk_box_pack_start(GTK_BOX(vbox), btn_voltar, FALSE, FALSE, 6);

    gtk_widget_show_all(window);
    gtk_widget_hide(alugador_box); /* garantir escondido após show_all */
}

/* TELA 1: Tela de Login (agora com campos e submit) */
void criar_tela_login(ListaUsuarios *lista)
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 360, 360);
    gtk_widget_set_name(window, "auth-window"); // Reusa o CSS

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(vbox, 12);
    gtk_widget_set_margin_bottom(vbox, 12);
    gtk_widget_set_margin_start(vbox, 18);
    gtk_widget_set_margin_end(vbox, 18);
    gtk_widget_set_name(vbox, "auth-box");
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Logo pequeno no topo (preservando proporção) */
    GdkPixbuf *pix = carregar_pixbuf_preservar("imagens/logo.jpg", 320, 120);
    if (!pix) pix = carregar_pixbuf_preservar("imagens/imovel1.jpg", 320, 120);
    if (pix) {
        GtkWidget *logo = gtk_image_new_from_pixbuf(pix);
        gtk_widget_set_name(logo, "auth-logo");
        g_object_unref(pix);
        gtk_box_pack_start(GTK_BOX(vbox), logo, FALSE, FALSE, 4);
    }

    GtkWidget *label = gtk_label_new("Tela de Login");
    gtk_widget_set_name(label, "welcome-label");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 4);

    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "E-mail");
    gtk_box_pack_start(GTK_BOX(vbox), entry_email, FALSE, FALSE, 4);

    GtkWidget *entry_senha = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_senha), "Senha");
    gtk_entry_set_visibility(GTK_ENTRY(entry_senha), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), entry_senha, FALSE, FALSE, 4);

    /* Guardar entradas na janela */
    g_object_set_data(G_OBJECT(window), "entry_email", entry_email);
    g_object_set_data(G_OBJECT(window), "entry_senha", entry_senha);

    /* Botões: Entrar / Criar Conta / Voltar */
    GtkWidget *btn_entrar = gtk_button_new_with_label("Entrar");
    gtk_widget_set_name(btn_entrar, "primary-action");
    gtk_widget_set_size_request(btn_entrar, 240, 44);
    g_signal_connect(btn_entrar, "clicked", G_CALLBACK(on_login_submit_clicked), lista);
    gtk_box_pack_start(GTK_BOX(vbox), btn_entrar, FALSE, FALSE, 4);

    GtkWidget *btn_criar = gtk_button_new_with_label("Criar Nova Conta");
    gtk_widget_set_name(btn_criar, "secondary-action");
    gtk_widget_set_size_request(btn_criar, 240, 44);
    g_signal_connect(btn_criar, "clicked", G_CALLBACK(on_abrir_registro_clicked), lista);
    gtk_box_pack_start(GTK_BOX(vbox), btn_criar, FALSE, FALSE, 4);

    GtkWidget *btn_voltar = gtk_button_new_with_label("← Voltar");
    gtk_widget_set_name(btn_voltar, "secondary-action");
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(on_voltar_para_inicial_clicked), lista);
    gtk_box_pack_start(GTK_BOX(vbox), btn_voltar, FALSE, FALSE, 4);

    gtk_widget_show_all(window);
}

/* --- TELA INICIAL --- */
void criar_tela_inicial(ListaUsuarios *lista) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 520, 420);
    gtk_widget_set_name(window, "auth-window"); // Reusa o CSS

    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Imobiliária");
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    GtkWidget *auth_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    gtk_widget_set_halign(auth_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(auth_box, GTK_ALIGN_CENTER);
    gtk_widget_set_name(auth_box, "auth-box");
    gtk_container_add(GTK_CONTAINER(window), auth_box);

    GError *err = NULL;
    GdkPixbuf *p = carregar_pixbuf_preservar("imagens/logo.jpg", 360, 140);
    if (!p) p = carregar_pixbuf_preservar("imagens/imovel1.jpg", 360, 140);
    if (p) {
        GtkWidget *logo = gtk_image_new_from_pixbuf(p);
        gtk_widget_set_name(logo, "auth-logo");
        g_object_unref(p);
        gtk_box_pack_start(GTK_BOX(auth_box), logo, FALSE, FALSE, 0);
    }

    GtkWidget *label_ini = gtk_label_new("Bem-vindo(a)!");
    gtk_widget_set_name(label_ini, "welcome-label");
    gtk_box_pack_start(GTK_BOX(auth_box), label_ini, FALSE, FALSE, 0);

    GtkWidget *btn_login = gtk_button_new_with_label("Já Tenho Login");
    gtk_widget_set_name(btn_login, "primary-action");
    g_signal_connect(btn_login, "clicked", G_CALLBACK(on_abrir_login_clicked), lista);
    gtk_widget_set_size_request(btn_login, 240, 44);
    gtk_box_pack_start(GTK_BOX(auth_box), btn_login, FALSE, FALSE, 0);

    GtkWidget *btn_registro = gtk_button_new_with_label("Criar Nova Conta");
    gtk_widget_set_name(btn_registro, "secondary-action");
    g_signal_connect(btn_registro, "clicked", G_CALLBACK(on_abrir_registro_clicked), lista);
    gtk_widget_set_size_request(btn_registro, 240, 44);
    gtk_box_pack_start(GTK_BOX(auth_box), btn_registro, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}
