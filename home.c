#include <gtk/gtk.h>
#include "home.h"
#include "gui.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* --- Protótipos privados --- */
static GdkPixbuf* carregar_imagem_scaled(const char *path, int w, int h);
static void on_card_clicked(GtkButton *button, gpointer user_data);
static void on_falar_clicked(GtkButton *button, gpointer user_data);
static GtkWidget* criar_sidebar(ListaUsuarios *lista);
static GtkWidget* criar_header(void);
static void on_maximize_toggle_clicked(GtkWidget *button, gpointer user_data);
static void on_search_changed(GtkEntry *entry, gpointer user_data);
static void show_sale_page(GtkWindow *main_win, const UiImovel *imovel);
static void on_confirm_purchase_clicked(GtkWidget *btn, gpointer user_data);
static void on_back_to_list_clicked(GtkWidget *btn, gpointer user_data);

/* --- Dados estáticos: 12 imóveis --- */
static const UiImovel IMOVEIS_STATIC[12] = {
    { 1, "Apartamento 2 quartos vista mar - Guilhermina", "R$ 785.000", "Apartamento com 2 quartos, varanda com vista para o mar, sala integrada, 1 vaga e ótima iluminação natural. Condomínio com salão de festas e portaria 24h.", "imagens/imovel1.jpg", "Guilhermina, Praia Grande", "Apartamento", 2, 1, 78 },
    { 2, "Casa 3 suítes com piscina - Pernambuco", "R$ 1.980.000", "Casa de alto padrão com 3 suítes (1 suíte master), piscina, área gourmet e garagem para 4 carros. Terreno amplo e acabamento premium.", "imagens/imovel2.jpg", "Pernambuco, Guarujá", "Casa", 3, 4, 420 },
    { 3, "Studio mobiliado - Gonzaga", "R$ 2.800/mês", "Studio moderno e mobiliado no coração do Gonzaga. Ideal para profissionais, próximo a comércio e transporte, pronto para morar.", "imagens/imovel3.jpg", "Gonzaga, Santos", "Studio", 0, 1, 38 },
    { 4, "Cobertura duplex com terraço - Boqueirão", "R$ 1.450.000", "Cobertura com terraço privativo, suíte master com closet, cozinha americana e vista panorâmica. Condomínio com piscina e sauna.", "imagens/imovel4.jpg", "Boqueirão, Santos", "Cobertura", 3, 3, 210 },
    { 5, "Casa de praia reformada - Vicente de Carvalho", "R$ 1.250.000", "Casa de praia próxima ao mar, reformada recentemente, 4 dormitórios, 2 piscinas, área gourmet e quintal grande.", "imagens/imovel5.jpg", "Vicente de Carvalho, Guarujá", "Casa", 4, 3, 300 },
    { 6, "Apartamento 3 quartos - Comércio", "R$ 980.000", "Apartamento espaçoso com 3 quartos, próximo a comércio e transporte. Sala ampla, cozinha com área de serviço e varanda.", "imagens/imovel6.jpg", "Centro, São Paulo", "Apartamento", 3, 2, 120 },
    { 7, "Casa com terreno - Litoral Norte", "R$ 620.000", "Ótima casa em terreno amplo, ideal para quem busca espaço; 2 quartos, quintal grande, possibilidade de ampliação.", "imagens/imovel7.jpg", "Litoral Norte, São Paulo", "Casa", 2, 1, 150 },
    { 8, "Apartamento compacto - Perdizes", "R$ 420.000", "Apartamento compacto e funcional, excelente para investimento ou locação, bem localizado e com boa infraestrutura.", "imagens/imovel8.jpg", "Perdizes, São Paulo", "Apartamento", 1, 1, 45 },
    { 9, "Casa na praia com vista - Pitangueiras", "R$ 2.150.000", "Casa de alto padrão com vista para o mar, área externa com piscina e jardim, acabamento de primeira linha.", "imagens/imovel9.jpg", "Pitangueiras, Guarujá", "Casa", 4, 5, 360 },
    {10, "Apartamento pronto para morar - Vila Olímpia", "R$ 790.000", "Apartamento moderno, 2 quartos, varanda, ótima planta e infraestrutura completa no condomínio.", "imagens/imovel10.jpg", "Vila Olímpia, São Paulo", "Apartamento", 2, 2, 88 },
    {11, "Casa/apartamento comercial - Gonzaga", "R$ 1.100.000", "Versátil: pode ser usado como residência ou espaço comercial. Localização estratégica perto de comércio.", "imagens/imovel11.jpg", "Gonzaga, Santos", "Comercial/Residencial", 3, 2, 160 },
    {12, "Prédio/apartamento moderno - Centro", "R$ 3.200.000", "Imóvel em prédio moderno com acabamento premium, ideal para investidores e alto padrão residencial.", "imagens/imovel12.jpg", "Centro, São Paulo", "Prédio/Apartamento", 6, 6, 980 }
};

/* --- Função auxiliar: carrega e escala imagem preservando proporção --- */
static GdkPixbuf* carregar_imagem_scaled(const char *path, int w, int h) {
    if (!path || strlen(path) == 0) return NULL;
    GError *err = NULL;
    GdkPixbuf *pix = gdk_pixbuf_new_from_file(path, &err);
    if (!pix) {
        g_clear_error(&err);
        return NULL;
    }

    int orig_w = gdk_pixbuf_get_width(pix);
    int orig_h = gdk_pixbuf_get_height(pix);
    if (orig_w <= 0 || orig_h <= 0) {
        g_object_unref(pix);
        return NULL;
    }

    double scale_w = (double)w / (double)orig_w;
    double scale_h = (double)h / (double)orig_h;
    double scale = (scale_w < scale_h) ? scale_w : scale_h; // fit inside
    if (scale > 1.0) scale = 1.0; // don't upscale beyond original

    int new_w = (int)(orig_w * scale);
    int new_h = (int)(orig_h * scale);

    GdkPixbuf *scaled = gdk_pixbuf_scale_simple(pix, new_w, new_h, GDK_INTERP_BILINEAR);
    g_object_unref(pix);
    return scaled;
}

/* --- Callback que abre a janela de detalhes (ou página de venda na mesma janela) --- */
static void on_card_clicked(GtkButton *button, gpointer user_data) {
    const UiImovel *imovel = (const UiImovel*)user_data;
    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(toplevel)) return;
    GtkWindow *win = GTK_WINDOW(toplevel);

    /* Mostrar página de venda na mesma janela (principal) */
    show_sale_page(win, imovel);
}

/* --- Callback do botão "Falar com anunciante" (placeholder) --- */
static void on_falar_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *parent = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(parent),
                                            GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_INFO,
                                            GTK_BUTTONS_OK,
                                            "Contato com o anunciante solicitado. (placeholder)");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

/* --- Cria seção de imóveis usando GtkFlowBox para grid responsivo --- */
GtkWidget* criar_secao_imoveis(const UiImovel **imoveis, int n_imoveis) {
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkWidget *flow = gtk_flow_box_new();
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flow), 4);
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flow), GTK_SELECTION_NONE);
    gtk_flow_box_set_row_spacing(GTK_FLOW_BOX(flow), 16);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(flow), 16);
    gtk_widget_set_margin_start(flow, 12);
    gtk_widget_set_margin_end(flow, 12);
    gtk_widget_set_margin_top(flow, 10);
    gtk_widget_set_margin_bottom(flow, 20);

    for (int i = 0; i < n_imoveis; ++i) {
        GtkWidget *card = criar_card_imovel(imoveis[i]);
        /* ligar o objeto UiImovel ao widget para busca/filtragem */
        g_object_set_data(G_OBJECT(card), "ui_imovel", (gpointer)imoveis[i]);
        gtk_flow_box_insert(GTK_FLOW_BOX(flow), card, -1);
    }

    /* tornar o flow acessível através do scrolled */
    g_object_set_data(G_OBJECT(scrolled), "flowbox", flow);

    gtk_container_add(GTK_CONTAINER(scrolled), flow);
    return scrolled;
}

/* --- Cria um card moderno (GtkButton sem relevo) --- */
GtkWidget* criar_card_imovel(const UiImovel *imovel) {
    GtkWidget *btn = gtk_button_new();
    gtk_button_set_relief(GTK_BUTTON(btn), GTK_RELIEF_NONE);
    gtk_widget_set_name(btn, "imovel-card-btn");
    gtk_widget_set_size_request(btn, 280, 380);

    GtkWidget *card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(card_box, "imovel-card");
    gtk_container_add(GTK_CONTAINER(btn), card_box);

    GtkWidget *img_align = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(img_align, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(img_align, GTK_ALIGN_START);
    gtk_widget_set_hexpand(img_align, TRUE);
    gtk_box_pack_start(GTK_BOX(card_box), img_align, FALSE, FALSE, 0);

    GdkPixbuf *pix = carregar_imagem_scaled(imovel->imagem_path, 560, 240);
    GtkWidget *img_widget;
    if (pix) {
        img_widget = gtk_image_new_from_pixbuf(pix);
        g_object_unref(pix);
    } else {
        img_widget = gtk_image_new_from_icon_name("image-missing", GTK_ICON_SIZE_DIALOG);
    }
    gtk_widget_set_name(img_widget, "imovel-image");
    gtk_box_pack_start(GTK_BOX(img_align), img_widget, FALSE, FALSE, 0);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(content), 12);

    GtkWidget *label_titulo = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span weight='bold' size='medium'>%s</span>", imovel->titulo);
    gtk_label_set_markup(GTK_LABEL(label_titulo), markup);
    g_free(markup);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(label_titulo), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label_titulo), 25);
    gtk_box_pack_start(GTK_BOX(content), label_titulo, FALSE, FALSE, 0);

    GtkWidget *label_preco = gtk_label_new(NULL);
    char *markup_preco = g_markup_printf_escaped("<span foreground='#1976d2' weight='bold' size='large'>%s</span>", imovel->preco_str);
    gtk_label_set_markup(GTK_LABEL(label_preco), markup_preco);
    g_free(markup_preco);
    gtk_label_set_xalign(GTK_LABEL(label_preco), 0.0);
    gtk_box_pack_start(GTK_BOX(content), label_preco, FALSE, FALSE, 0);

    char info_buf[128];
    snprintf(info_buf, sizeof(info_buf), "📍 %s · 🛏 %d · 🛁 %d · %dm²", imovel->localizacao ? imovel->localizacao : "-", imovel->quartos, imovel->banheiros, imovel->area_m2);
    GtkWidget *label_info = gtk_label_new(info_buf);
    gtk_label_set_xalign(GTK_LABEL(label_info), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(label_info), TRUE);
    gtk_box_pack_start(GTK_BOX(content), label_info, FALSE, FALSE, 0);

    GtkWidget *hbox_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *btn_ver = gtk_button_new_with_label("Ver detalhes");
    gtk_widget_set_name(btn_ver, "card-action-button");
    gtk_widget_set_size_request(btn_ver, 120, 36);
    gtk_box_pack_end(GTK_BOX(hbox_actions), btn_ver, FALSE, FALSE, 0);
    g_signal_connect(btn_ver, "clicked", G_CALLBACK(on_card_clicked), (gpointer)imovel);

    gtk_box_pack_end(GTK_BOX(content), hbox_actions, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card_box), content, TRUE, TRUE, 0);

    /* ligar também no clique do botão inteiro */
    g_signal_connect(btn, "clicked", G_CALLBACK(on_card_clicked), (gpointer)imovel);

    return btn;
}

/* --- Janela de detalhes do imóvel substituída por página de venda na mesma janela --- */
void mostrar_detalhes_imovel(GtkWidget *parent, const UiImovel *imovel) {
    /* Mantive apenas fallback que abre janela modal (não usado pela navegação principal) */
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), imovel->titulo ? imovel->titulo : "Detalhes");
    gtk_window_set_default_size(GTK_WINDOW(win), 980, 700);
    if (GTK_IS_WINDOW(parent)) {
        gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(parent));
        gtk_window_set_modal(GTK_WINDOW(win), TRUE);
    }
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 18);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 18);
    gtk_container_add(GTK_CONTAINER(win), hbox);

    GdkPixbuf *pix = carregar_imagem_scaled(imovel->imagem_path, 720, 480);
    GtkWidget *img;
    if (pix) {
        img = gtk_image_new_from_pixbuf(pix);
        g_object_unref(pix);
    } else {
        img = gtk_image_new_from_icon_name("image-missing", GTK_ICON_SIZE_DIALOG);
    }
    gtk_widget_set_name(img, "detail-image");
    gtk_box_pack_start(GTK_BOX(hbox), img, FALSE, FALSE, 0);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_hexpand(vbox, TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

    GtkWidget *label_titulo = gtk_label_new(NULL);
    char *markup = g_markup_printf_escaped("<span weight='bold' size='xx-large'>%s</span>", imovel->titulo);
    gtk_label_set_markup(GTK_LABEL(label_titulo), markup);
    g_free(markup);
    gtk_label_set_xalign(GTK_LABEL(label_titulo), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), label_titulo, FALSE, FALSE, 0);

    GtkWidget *label_preco = gtk_label_new(NULL);
    char *markup_preco = g_markup_printf_escaped("<span foreground='#0d47a1' weight='bold' size='x-large'>%s</span>", imovel->preco_str);
    gtk_label_set_markup(GTK_LABEL(label_preco), markup_preco);
    g_free(markup_preco);
    gtk_label_set_xalign(GTK_LABEL(label_preco), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), label_preco, FALSE, FALSE, 0);

    char info_buf[256];
    snprintf(info_buf, sizeof(info_buf), "Tipo: %s\nLocalização: %s\nQuartos: %d · Banheiros: %d · Área: %dm²",
             imovel->tipo ? imovel->tipo : "-", imovel->localizacao ? imovel->localizacao : "-", imovel->quartos, imovel->banheiros, imovel->area_m2);
    GtkWidget *label_info = gtk_label_new(NULL);
    char *markup_info = g_markup_printf_escaped("<span size='small'>%s</span>", info_buf);
    gtk_label_set_markup(GTK_LABEL(label_info), markup_info);
    g_free(markup_info);
    gtk_label_set_xalign(GTK_LABEL(label_info), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(label_info), TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), label_info, FALSE, FALSE, 0);

    GtkWidget *label_desc = gtk_label_new(imovel->descricao ? imovel->descricao : "Sem descrição.");
    gtk_label_set_line_wrap(GTK_LABEL(label_desc), TRUE);
    gtk_label_set_justify(GTK_LABEL(label_desc), GTK_JUSTIFY_LEFT);
    gtk_widget_set_vexpand(label_desc, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), label_desc, TRUE, TRUE, 0);

    GtkWidget *hbox_actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(hbox_actions, GTK_ALIGN_END);

    GtkWidget *btn_falar = gtk_button_new_with_label("Falar com anunciante");
    gtk_widget_set_name(btn_falar, "primary-action");
    gtk_box_pack_start(GTK_BOX(hbox_actions), btn_falar, FALSE, FALSE, 0);

    GtkWidget *btn_voltar = gtk_button_new_with_label("Voltar");
    gtk_widget_set_name(btn_voltar, "secondary-action");
    gtk_box_pack_start(GTK_BOX(hbox_actions), btn_voltar, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), hbox_actions, FALSE, FALSE, 0);

    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(gtk_widget_destroy), win);
    g_signal_connect(btn_falar, "clicked", G_CALLBACK(on_falar_clicked), NULL);

    gtk_widget_show_all(win);
}

/* --- Barra lateral com logo e filtros --- */
static GtkWidget* criar_sidebar(ListaUsuarios *lista) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 280, -1);
    gtk_widget_set_valign(sidebar, GTK_ALIGN_START);
    gtk_container_set_border_width(GTK_CONTAINER(sidebar), 16);

    GtkWidget *img_logo = NULL;
    GdkPixbuf *pix = carregar_imagem_scaled("imagens/logo.jpg", 260, 120);
    if (!pix) pix = carregar_imagem_scaled("imagens/imovel1.jpg", 260, 120);
    if (pix) {
        img_logo = gtk_image_new_from_pixbuf(pix);
        g_object_unref(pix);
    } else {
        img_logo = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(img_logo), "<span weight='bold' size='large' foreground='white'>ImóvelApp</span>");
    }
    gtk_widget_set_name(img_logo, "sidebar-logo");
    gtk_widget_set_halign(img_logo, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(sidebar), img_logo, FALSE, FALSE, 6);

    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(sidebar), sep, FALSE, FALSE, 6);

    GtkWidget *frame1 = gtk_frame_new("Tipo de negociação");
    gtk_widget_set_name(frame1, "filter-frame");
    GtkWidget *vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(frame1), vbox1);
    GtkWidget *rb1 = gtk_radio_button_new_with_label(NULL, "Compra");
    GtkWidget *rb2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(rb1), "Aluguel");
    gtk_box_pack_start(GTK_BOX(vbox1), rb1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox1), rb2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), frame1, FALSE, FALSE, 8);

    GtkWidget *frame2 = gtk_frame_new("Tipo de imóvel");
    GtkWidget *vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(frame2), vbox2);
    GtkWidget *combo_tipo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_tipo), "Qualquer tipo");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_tipo), "Apartamento");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_tipo), "Casa");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_tipo), "Studio");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_tipo), 0);
    gtk_box_pack_start(GTK_BOX(vbox2), combo_tipo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), frame2, FALSE, FALSE, 8);

    GtkWidget *frame3 = gtk_frame_new("Faixa de preço");
    GtkWidget *vbox3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(frame3), vbox3);
    GtkWidget *combo_preco = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_preco), "Qualquer preço");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_preco), "Até R$ 500.000");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_preco), "R$ 500.000 - R$ 1.000.000");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_preco), "Acima de R$ 1.000.000");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_preco), 0);
    gtk_box_pack_start(GTK_BOX(vbox3), combo_preco, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), frame3, FALSE, FALSE, 8);

    GtkWidget *frame4 = gtk_frame_new("Cidade");
    GtkWidget *vbox4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(frame4), vbox4);
    GtkWidget *combo_cidade = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_cidade), "Qualquer cidade");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_cidade), "Praia Grande");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_cidade), "Guarujá");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_cidade), "Santos");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_cidade), "São Paulo");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_cidade), 0);
    gtk_box_pack_start(GTK_BOX(vbox4), combo_cidade, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), frame4, FALSE, FALSE, 8);

    GtkWidget *btn_aplicar = gtk_button_new_with_label("Aplicar filtros");
    gtk_widget_set_name(btn_aplicar, "primary-action");
    gtk_box_pack_end(GTK_BOX(sidebar), btn_aplicar, FALSE, FALSE, 6);

    return sidebar;
}

/* --- Header simples preservado (agora conecta busca) --- */
static GtkWidget* criar_header(void) {
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(header_box, "main-header");
    gtk_container_set_border_width(GTK_CONTAINER(header_box), 16);
    gtk_widget_set_halign(header_box, GTK_ALIGN_CENTER);

    GtkWidget *search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "🔍 Buscar imóveis...");
    gtk_widget_set_name(search_entry, "search-entry");
    gtk_widget_set_size_request(search_entry, 600, 50);
    gtk_widget_set_halign(search_entry, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(header_box), search_entry, FALSE, FALSE, 0);

    /* conectar busca (será ligado ao window no criar_tela_home) */
    g_signal_connect(search_entry, "changed", G_CALLBACK(on_search_changed), NULL);

    return header_box;
}

/* --- Tela Home principal (usa IMOVEIS_STATIC e sidebar) --- */
void criar_tela_home(ListaUsuarios *lista) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "ImóvelApp - Home");
    gtk_window_set_default_size(GTK_WINDOW(window), 1400, 900);
    gtk_widget_set_name(window, "home-window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* ícone da aplicação (barra de tarefas) */
    GError *err = NULL;
    if (!gtk_window_set_icon_from_file(GTK_WINDOW(window), "imagens/logo.jpg", &err)) {
        g_clear_error(&err);
    }

    /* Maximizar janela ao abrir */
    gtk_window_maximize(GTK_WINDOW(window));

    /* Header bar custom com minimizar, maximizar e fechar.
       Alterado: trocar posição de fechar e minimizar (close à esquerda, maximizar meio, minimizar direita) */
    GtkHeaderBar *hdr = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_set_show_close_button(hdr, FALSE);
    gtk_header_bar_set_title(hdr, "ImóvelApp");

    GtkWidget *btn_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_BUTTON);
    g_signal_connect_swapped(btn_close, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_header_bar_pack_end(hdr, btn_close);

    GtkWidget *btn_max = gtk_button_new_from_icon_name("window-maximize-symbolic", GTK_ICON_SIZE_BUTTON);
    g_object_set_data(G_OBJECT(btn_max), "window", window);
    g_signal_connect(btn_max, "clicked", G_CALLBACK(on_maximize_toggle_clicked), window);
    gtk_header_bar_pack_end(hdr, btn_max);

    GtkWidget *btn_min = gtk_button_new_from_icon_name("window-minimize-symbolic", GTK_ICON_SIZE_BUTTON);
    g_signal_connect_swapped(btn_min, "clicked", G_CALLBACK(gtk_window_iconify), GTK_WINDOW(window));
    gtk_header_bar_pack_end(hdr, btn_min);

    gtk_window_set_titlebar(GTK_WINDOW(window), GTK_WIDGET(hdr));

    GtkWidget *hbox_main = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), hbox_main);

    GtkWidget *sidebar = criar_sidebar(lista);
    gtk_box_pack_start(GTK_BOX(hbox_main), sidebar, FALSE, FALSE, 0);

    GtkWidget *content_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_hexpand(content_container, TRUE);
    gtk_widget_set_vexpand(content_container, TRUE);
    gtk_widget_set_margin_start(content_container, 16);
    gtk_widget_set_margin_end(content_container, 16);
    gtk_widget_set_margin_top(content_container, 12);
    gtk_widget_set_margin_bottom(content_container, 12);

    /* registrar content_container na janela para uso posterior (trocar página) */
    g_object_set_data(G_OBJECT(window), "content_container", content_container);

    GtkWidget *header = criar_header();
    /* atualizar o handler da busca para receber a janela (GTK3) */
    GList *header_children = gtk_container_get_children(GTK_CONTAINER(header));
    GtkWidget *search_entry = NULL;
    if (header_children) {
        search_entry = GTK_WIDGET(g_list_nth_data(header_children, 0));
        if (search_entry) {
            g_signal_handlers_disconnect_by_func(search_entry, G_CALLBACK(on_search_changed), NULL);
            g_signal_connect(search_entry, "changed", G_CALLBACK(on_search_changed), window);
        }
        g_list_free(header_children);
    }

    gtk_box_pack_start(GTK_BOX(content_container), header, FALSE, FALSE, 0);

    /* Título principal */
    GtkWidget *label_titulo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_titulo), "<span weight='bold' size='x-large'>✨ Imóveis em Destaque</span>");
    gtk_widget_set_halign(label_titulo, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label_titulo, 6);
    gtk_widget_set_margin_top(label_titulo, 8);
    gtk_box_pack_start(GTK_BOX(content_container), label_titulo, FALSE, FALSE, 0);

    /* Seção com todos os 12 imóveis em um único grid */
    const UiImovel *todos_imoveis[12];
    for (int i = 0; i < 12; ++i) todos_imoveis[i] = &IMOVEIS_STATIC[i];

    GtkWidget *imoveis_widget = criar_secao_imoveis(todos_imoveis, 12);
    gtk_box_pack_start(GTK_BOX(content_container), imoveis_widget, TRUE, TRUE, 0);

    /* armazenar referência aos imóveis (para restaurar depois) */
    g_object_set_data(G_OBJECT(window), "imoveis_widget", imoveis_widget);

    /* também armazenar scrolled/flowbox para busca */
    g_object_set_data(G_OBJECT(window), "imoveis_scrolled", imoveis_widget);
    /* dentro do scrolled o flowbox foi salvo como "flowbox" */
    GtkWidget *flow = g_object_get_data(G_OBJECT(imoveis_widget), "flowbox");
    if (flow) g_object_set_data(G_OBJECT(window), "flowbox", flow);

    gtk_box_pack_start(GTK_BOX(hbox_main), content_container, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
}

/* Callback para alternar maximizar/restaurar */
static void on_maximize_toggle_clicked(GtkWidget *button, gpointer user_data) {
    GtkWindow *win = GTK_WINDOW(user_data);
    GdkWindowState state = gdk_window_get_state(gtk_widget_get_window(GTK_WIDGET(win)));

    if (state & GDK_WINDOW_STATE_MAXIMIZED) {
        gtk_window_unmaximize(win);
    } else {
        gtk_window_maximize(win);
    }
}

/* --- Busca: filtra widgets do flowbox pelo texto --- */
static void on_search_changed(GtkEntry *entry, gpointer user_data) {
    const char *text = gtk_entry_get_text(entry);
    char q[256] = "";
    if (text) {
        strncpy(q, text, sizeof(q)-1);
        q[sizeof(q)-1] = 0;
    }

    GtkWindow *win = GTK_WINDOW(user_data ? user_data : gtk_widget_get_toplevel(GTK_WIDGET(entry)));
    if (!GTK_IS_WINDOW(win)) return;

    GtkWidget *flow = g_object_get_data(G_OBJECT(win), "flowbox");
    if (!flow) return;

    GList *children = gtk_container_get_children(GTK_CONTAINER(flow));
    for (GList *l = children; l; l = l->next) {
        GtkWidget *child = GTK_WIDGET(l->data);
        const UiImovel *im = (const UiImovel*) g_object_get_data(G_OBJECT(child), "ui_imovel");
        gboolean match = TRUE;
        if (im && q[0]) {
            char lower_q[256]; char field[512];
            for (int i=0; q[i] && i<255; ++i) lower_q[i] = tolower((unsigned char)q[i]);
            lower_q[strlen(q)] = 0;
            snprintf(field, sizeof(field), "%s %s %s", im->titulo ? im->titulo : "", im->localizacao ? im->localizacao : "", im->tipo ? im->tipo : "");
            for (int i=0; field[i]; ++i) field[i] = tolower((unsigned char)field[i]);
            match = (strstr(field, lower_q) != NULL);
        }
        if (match) gtk_widget_show(child); else gtk_widget_hide(child);
    }
    g_list_free(children);
}

/* --- Mostra a página de venda (substitui a seção de cards dentro da janela principal) --- */
static void show_sale_page(GtkWindow *main_win, const UiImovel *imovel) {
    if (!GTK_IS_WINDOW(main_win) || !imovel) return;

    GtkWidget *content = g_object_get_data(G_OBJECT(main_win), "content_container");
    GtkWidget *orig_section = g_object_get_data(G_OBJECT(main_win), "imoveis_widget");
    if (!content || !orig_section) {
        /* fallback: abrir modal */
        mostrar_detalhes_imovel(GTK_WIDGET(main_win), imovel);
        return;
    }

    /* criar container scrollável para a página de venda */
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_margin_top(scrolled, 6);
    gtk_widget_set_margin_bottom(scrolled, 6);

    GtkWidget *sale_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(sale_box), 10);
    gtk_widget_set_size_request(sale_box, 860, -1); /* largura controlada para melhor visualização */

    /* topo: botão Voltar pequeno e título (barra compacta) */
    GtkWidget *top_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(top_bar, GTK_ALIGN_START);

    GtkWidget *btn_back_top = gtk_button_new_with_label("← Voltar");
    gtk_widget_set_name(btn_back_top, "secondary-action");
    gtk_box_pack_start(GTK_BOX(top_bar), btn_back_top, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(sale_box), top_bar, FALSE, FALSE, 6);

    /* Cabeçalho com título e preço (compacto) */
    GtkWidget *title = gtk_label_new(NULL);
    char *m = g_markup_printf_escaped("<span weight='bold' size='large'>%s</span>\n<span foreground='#0d47a1' weight='bold' size='medium'>%s</span>",
                                     imovel->titulo, imovel->preco_str);
    gtk_label_set_markup(GTK_LABEL(title), m);
    g_free(m);
    gtk_label_set_xalign(GTK_LABEL(title), 0.0);
    gtk_box_pack_start(GTK_BOX(sale_box), title, FALSE, FALSE, 6);

    /* Imagem média */
    GdkPixbuf *pix = carregar_imagem_scaled(imovel->imagem_path, 640, 300);
    GtkWidget *img = pix ? gtk_image_new_from_pixbuf(pix) : gtk_image_new_from_icon_name("image-missing", GTK_ICON_SIZE_DIALOG);
    if (pix) g_object_unref(pix);
    gtk_widget_set_name(img, "detail-image");
    gtk_box_pack_start(GTK_BOX(sale_box), img, FALSE, FALSE, 6);

    /* Descrição e informações compactas */
    char info_buf[256];
    snprintf(info_buf, sizeof(info_buf), "Tipo: %s · Local: %s · Quartos: %d · Banheiros: %d · Área: %dm²",
             imovel->tipo ? imovel->tipo : "-", imovel->localizacao ? imovel->localizacao : "-", imovel->quartos, imovel->banheiros, imovel->area_m2);
    GtkWidget *label_info = gtk_label_new(info_buf);
    gtk_label_set_xalign(GTK_LABEL(label_info), 0.0);
    gtk_box_pack_start(GTK_BOX(sale_box), label_info, FALSE, FALSE, 4);

    GtkWidget *label_desc = gtk_label_new(imovel->descricao ? imovel->descricao : "Sem descrição.");
    gtk_label_set_line_wrap(GTK_LABEL(label_desc), TRUE);
    gtk_label_set_justify(GTK_LABEL(label_desc), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(sale_box), label_desc, FALSE, FALSE, 6);

    /* Formulário simples de compra (compacto) */
    GtkWidget *form = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *entry_nome = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_nome), "Seu nome");
    GtkWidget *entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Seu e-mail");
    GtkWidget *combo_pag = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_pag), "Pix");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_pag), "Transferência");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_pag), "Cartão de crédito");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_pag), 0);

    gtk_box_pack_start(GTK_BOX(form), entry_nome, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form), entry_email, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form), combo_pag, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sale_box), form, FALSE, FALSE, 6);

    /* Ações: Confirmar / Voltar (rodapé) */
    GtkWidget *h = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget *btn_confirm = gtk_button_new_with_label("Confirmar compra");
    GtkWidget *btn_back = gtk_button_new_with_label("Voltar aos imóveis");
    gtk_widget_set_name(btn_confirm, "primary-action");
    gtk_widget_set_name(btn_back, "secondary-action");
    gtk_box_pack_end(GTK_BOX(h), btn_confirm, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(h), btn_back, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sale_box), h, FALSE, FALSE, 6);

    /* conectar ações (passar main_win + imovel por user_data via struct simples) */
    struct {
        GtkWindow *win;
        const UiImovel *im;
    } *ud = malloc(sizeof(*ud));
    ud->win = main_win;
    ud->im = imovel;
    g_signal_connect(btn_confirm, "clicked", G_CALLBACK(on_confirm_purchase_clicked), ud);
    g_signal_connect(btn_back, "clicked", G_CALLBACK(on_back_to_list_clicked), ud);
    g_signal_connect(btn_back_top, "clicked", G_CALLBACK(on_back_to_list_clicked), ud);

    /* adicionar sale_box dentro do scrolled e substituir seção */
    gtk_container_add(GTK_CONTAINER(scrolled), sale_box);

    GList *kids = gtk_container_get_children(GTK_CONTAINER(content));
    GtkWidget *to_replace = NULL;
    if (g_list_length(kids) >= 3) {
        to_replace = GTK_WIDGET(g_list_nth_data(kids, 2));
    }
    if (to_replace) gtk_container_remove(GTK_CONTAINER(content), to_replace);

    gtk_box_pack_start(GTK_BOX(content), scrolled, TRUE, TRUE, 0);
    gtk_widget_show_all(GTK_WIDGET(main_win));
    g_list_free(kids);

    /* guardar referências para posterior remoção */
    g_object_set_data(G_OBJECT(main_win), "current_sale_page", scrolled);
}

/* --- Confirmar compra (placeholder) --- */
static void on_confirm_purchase_clicked(GtkWidget *btn, gpointer user_data) {
    if (!user_data) return;
    struct { GtkWindow *win; const UiImovel *im; } *ud = user_data;

    GtkWidget *dlg = gtk_message_dialog_new(GTK_WINDOW(ud->win),
                                            GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_INFO,
                                            GTK_BUTTONS_OK,
                                            "Compra confirmada!\nEntraremos em contato pelo e-mail informado.");
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);

    /* voltar para lista automaticamente (on_back_to_list_clicked libera ud) */
    on_back_to_list_clicked(NULL, user_data);
}

/* --- Voltar para lista de imóveis --- */
static void on_back_to_list_clicked(GtkWidget *btn, gpointer user_data) {
    if (!user_data) return;
    struct { GtkWindow *win; const UiImovel *im; } *ud = user_data;
    GtkWindow *main_win = ud->win;

    GtkWidget *content = g_object_get_data(G_OBJECT(main_win), "content_container");
    GtkWidget *orig_section = g_object_get_data(G_OBJECT(main_win), "imoveis_widget");
    GtkWidget *current = g_object_get_data(G_OBJECT(main_win), "current_sale_page");

    if (current) {
        gtk_container_remove(GTK_CONTAINER(content), current);
        g_object_set_data(G_OBJECT(main_win), "current_sale_page", NULL);
    }

    if (orig_section) {
        gtk_box_pack_start(GTK_BOX(content), orig_section, TRUE, TRUE, 0);
        gtk_widget_show_all(orig_section);
    }

    free(ud);
}