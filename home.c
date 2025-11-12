#include <gtk/gtk.h>
#include "home.h"
#include "gui.h"

void criar_tela_home() {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *header;
    GtkWidget *search_entry;
    GtkWidget *search_button;
    GtkWidget *btn_cadastrar;
    GtkWidget *btn_ver_imoveis;
    GtkWidget *btn_perfil;

    gtk_init(NULL, NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Imobiliária — Home");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Cabeçalho
    header = gtk_label_new("<span size='xx-large' weight='bold'>🏠 Imobiliária PuroFoco</span>");
    gtk_label_set_use_markup(GTK_LABEL(header), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, FALSE, 10);

    // Barra de pesquisa
    GtkWidget *hbox_search = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Buscar imóveis, cidades...");
    search_button = gtk_button_new_with_label("🔍 Buscar");

    gtk_box_pack_start(GTK_BOX(hbox_search), search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_search), search_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_search, FALSE, FALSE, 10);

    // Botões principais
    btn_cadastrar = gtk_button_new_with_label("📦 Cadastrar Imóvel");
    btn_ver_imoveis = gtk_button_new_with_label("🏘️ Ver Imóveis Disponíveis");
    btn_perfil = gtk_button_new_with_label("👤 Meu Perfil");

    gtk_box_pack_start(GTK_BOX(vbox), btn_cadastrar, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_ver_imoveis, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), btn_perfil, TRUE, TRUE, 5);

    gtk_widget_show_all(window);
    gtk_main();
}
