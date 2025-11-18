#include <gtk/gtk.h>
#include "home.h"
#include "gui.h"

GtkWidget* criar_card_imovel(const char *imagem_path, const char *titulo, const char *preco) {
    
    // O card em si é uma caixa vertical
    GtkWidget *card_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *imagem = gtk_image_new_from_file(imagem_path);
    
    // Define um tamanho fixo para a imagem
    gtk_widget_set_size_request(imagem, 200, 150); 
    
    // 2. O Título
    GtkWidget *label_titulo = gtk_label_new(titulo);
    gtk_widget_set_halign(label_titulo, GTK_ALIGN_START); // Alinha à esquerda
    
    // 3. O Preço
    GtkWidget *label_preco = gtk_label_new(preco);
    gtk_widget_set_halign(label_preco, GTK_ALIGN_START); // Alinha à esquerda

    // Adiciona tudo ao card
    gtk_box_pack_start(GTK_BOX(card_box), imagem, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card_box), label_titulo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card_box), label_preco, FALSE, FALSE, 0);
    
    return card_box;
}

GtkWidget* criar_secao_imoveis() {
    
    // 1. A Janela de Rolagem
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    // Configura para rolar SÓ na horizontal, e a barra de rolagem só aparece se precisar
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                   GTK_POLICY_AUTOMATIC, 
                                   GTK_POLICY_NEVER); // Nunca mostrar barra vertical

    // 2. A Caixa Horizontal DENTRO da janela de rolagem
    GtkWidget *hbox_imoveis = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10); // 10px de espaçamento
    gtk_container_add(GTK_CONTAINER(scrolled_window), hbox_imoveis);

    // 3. Adicione vários cards (aqui você faria um loop com seus dados reais)
    GtkWidget *card1 = criar_card_imovel("C:/Teste/img/img.png", "Apartamento - Praia Grande", "R$ 570");
    GtkWidget *card2 = criar_card_imovel("C:/Teste/img/img1.png", "Condomínio - Maracanã", "R$ 668");
    GtkWidget *card3 = criar_card_imovel("C:/Teste/img/img2.png", "Apartamento - Vilamar", "R$ 424");
    GtkWidget *card4 = criar_card_imovel("C:/Teste/img/img3.png", "Loft - Rio de Janeiro", "R$ 379");
    
    gtk_box_pack_start(GTK_BOX(hbox_imoveis), card1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_imoveis), card2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_imoveis), card3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_imoveis), card4, FALSE, FALSE, 0);
    
    return scrolled_window;
}

void criar_tela_home(ListaUsuarios *lista) { // Mudei para receber a lista
    GtkWidget *window;
    GtkWidget *vbox; // Sua caixa vertical principal
    GtkWidget *header;
    GtkWidget *hbox_search;
    GtkWidget *search_entry;
    GtkWidget *search_button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // ... (seu código de window_set_title, set_default_size, etc.) ...
    
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // --- Seu Cabeçalho e Barra de Pesquisa (Mantém) ---
    header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), "<span size='xx-large' weight='bold'>🏡 Imobiliária</span>");
    gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, FALSE, 10);

    hbox_search = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    search_entry = gtk_entry_new();
    // ... (seu código de search_entry, search_button, etc.) ...
    gtk_box_pack_start(GTK_BOX(vbox), hbox_search, FALSE, FALSE, 10);


    /* --- AQUI COMEÇA A MUDANÇA --- */
    
    // Título da Seção 1
    GtkWidget *label_secao1 = gtk_label_new("Disponível em Praia Grande");
    gtk_widget_set_halign(label_secao1, GTK_ALIGN_START); // Alinha à esquerda
    gtk_box_pack_start(GTK_BOX(vbox), label_secao1, FALSE, FALSE, 0);

    // Lista de Rolagem 1
    GtkWidget *secao1 = criar_secao_imoveis(); // Chama a função que fizemos
    gtk_box_pack_start(GTK_BOX(vbox), secao1, TRUE, TRUE, 0);

    // Título da Seção 2
    GtkWidget *label_secao2 = gtk_label_new("Fique em Guarujá");
    gtk_widget_set_halign(label_secao2, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), label_secao2, FALSE, FALSE, 0);
    
    // Lista de Rolagem 2 (chamaria a mesma função, mas com dados diferentes)
    GtkWidget *secao2 = criar_secao_imoveis(); 
    gtk_box_pack_start(GTK_BOX(vbox), secao2, TRUE, TRUE, 0);

    // Você ainda pode ter botões, talvez menores ou em um menu
    // GtkWidget *btn_cadastrar_imovel = gtk_button_new_with_label("Cadastrar Imóvel");
    // gtk_box_pack_start(GTK_BOX(vbox), btn_cadastrar_imovel, FALSE, FALSE, 5);

    
    gtk_widget_show_all(window);
}
