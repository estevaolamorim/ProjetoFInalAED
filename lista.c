#include "lista.h"
#include "ingredientes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void exibirMenu(){
    printf("\n\n");
    printf("----------------MENU----------------\n");
    printf("\n\n");
    printf("1. Cadastrar Receita\n");
    printf("2. Remover Receita\n");
    printf("3. Listar Receitas\n");
    printf("4. Cadastrar Ingrediente em uma Receita\n");
    printf("5. Listar Ingredientes de uma Receita\n");
    printf("6. Listar Receitas por Ingrediente\n");
    printf("7. Encontrar Prato com Menor Preço\n");
    printf("8. Encontrar Ingrediente Mais Usado\n");
    printf("9. Encontrar Ingredientes em Comum entre Receitas\n");
    printf("10. Sair\n");
    printf("\n\n-----------------------------------------");
}

Lista* criarLista() {
    // aloca um bloco de memória do tamanho da struct lista e guarda o endereço no ponteiro novalista
    Lista* novaLista = (Lista*)malloc(sizeof(Lista));
    if (novaLista != NULL) {
        // inicializa o ponteiro de início como null porque a lista começa vazia
        novaLista->inicio = NULL;
        novaLista->tamanho = 0;
    }
    // retorna o endereço de memória onde a lista foi criada
    return novaLista;
}

void cadastrarReceita(Lista* lista) {
    if (lista == NULL) return;

    // aloca memória para um novo nó (receita) e guarda a referência no ponteiro novono
    No* novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) return;

    int escolha;

    printf("\n Digite o nome do prato: ");
    fgets(novoNo->Nome, 100, stdin); // fgets para nao ter stack overflow
    novoNo->Nome[strcspn(novoNo->Nome, "\n")] = '\0'; // remove '\n' se houver

    printf("\n Digite o preco do prato: \n");
    scanf("%f", &novoNo->preco);

    printf("\n Digite o numero de ingredientes: \n");
    scanf("%d", &escolha);
    // consumir o '\n' que ficou no buffer após scanf
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // o ponteiro ingredientes dentro do nó aponta para null (lista de ingredientes vazia)
    novoNo->ingredientes = NULL;

    for (int i = 0; i < escolha; i++) {
        char buffer[100];
        printf("Digite o ingrediente %d: ", i + 1);
        fgets(buffer, sizeof(buffer), stdin);
        // remove a nova linha que o fgets deixa
        buffer[strcspn(buffer, "\n")] = '\0';

        // aloca memória para o nó do ingrediente
        Receita* novoIng = (Receita*)malloc(sizeof(Receita));
        if (novoIng == NULL) continue; // sem memória, pula
        
        // strdup aloca memória para a string e retorna o ponteiro para essa área
        novoIng->ingrediente = strdup(buffer);
        // o ponteiro proxima do ingrediente aponta para null (fim da sublista)
        novoIng->proxima = NULL;

        // anexa na cauda da lista de ingredientes
        if (novoNo->ingredientes == NULL) {
            // se for o primeiro, o ponteiro da receita aponta direto para esse novo ingrediente
            novoNo->ingredientes = novoIng;
        } else {
            // ponteiro auxiliar p para percorrer a sublista sem perder o início
            Receita* p = novoNo->ingredientes;
            while (p->proxima != NULL) {
                // p pula para o endereço guardado no ponteiro proxima
                p = p->proxima;
            }
            // o último elemento agora aponta para o novo ingrediente em vez de null
            p->proxima = novoIng;
        }
    }

    // faz o novo nó apontar para onde o início da lista apontava (insere no começo)
    novoNo->proximo = lista->inicio;
    // como é o primeiro, o ponteiro anterior deve ser null
    novoNo->anterior = NULL;

    if (lista->inicio != NULL) {
        // se já existia alguém, o ponteiro anterior do antigo primeiro agora aponta para o novo nó
        lista->inicio->anterior = novoNo;
    }
    
    // o ponteiro de início da lista agora guarda o endereço do novo nó
    lista->inicio = novoNo;
    lista->tamanho++;
}

void cadastrarIngredienteReceita(Lista* lista, char* nomePrato){
    if(lista == NULL || lista->inicio == NULL) return;
    // ponteiro atual recebe o endereço do primeiro elemento da lista
    No* atual = lista->inicio;
    while(atual != NULL){
        if(strcmp(atual->Nome, nomePrato) == 0){
            char buffer[100];
            printf("\n Digite o novo ingrediente: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            
            Receita* novoIng = (Receita*)malloc(sizeof(Receita));
            if(novoIng != NULL){
                novoIng->ingrediente = strdup(buffer);
                novoIng->proxima = NULL;
                
                // verifica o ponteiro da sublista de ingredientes dentro do nó atual
                if(atual->ingredientes == NULL){
                    atual->ingredientes = novoIng;
                }else{
                    // ponteiro auxiliar para não perder o início da sublista de ingredientes
                    Receita* p = atual->ingredientes;
                    while(p->proxima != NULL){
                        p = p->proxima;
                    }
                    p->proxima = novoIng;
                }
            }
            break;
        }
        // atual recebe o endereço do próximo nó da lista principal
        atual = atual->proximo;
    }
}

void listarIngredientesReceita(Lista* lista, char* nomePrato){
    if(lista == NULL || lista->inicio == NULL) return;
    No* atual = lista->inicio;
    while(atual != NULL){
        if(strcmp(atual->Nome, nomePrato) == 0){
            // ponteiro ingatual recebe o endereço do primeiro ingrediente dessa receita
            Receita* ingAtual = atual->ingredientes;
            while(ingAtual != NULL){
                printf("\n - %s\n", ingAtual->ingrediente);
                // pula para o endereço do próximo ingrediente na sublista
                ingAtual = ingAtual->proxima;
            }
            return;
        }
        atual = atual->proximo;
    }
}

void MenorPreco(Lista* lista){
    if(lista == NULL || lista->inicio == NULL) return;
    No* atual = lista->inicio;
    // guarda o endereço do nó que tem o menor preço
    No* menorPrato = atual;
    float menor = atual->preco;
    while(atual != NULL){
        if(atual->preco < menor){
            menor = atual->preco;
            // atualiza o ponteiro menorprato para o endereço do nó atual
            menorPrato = atual;
        }
        atual = atual->proximo;
    }
    printf("\n Nome: %s\nPreco: %.2f\n", menorPrato->Nome, menorPrato->preco);
}

typedef struct Freq{
    char nome[100];
    int contagem;
    struct Freq* proximo;
} Freq;

// recebe um ponteiro para ponteiro (freq**) porque precisa alterar o endereço original da lista de frequência
void registrarIngrediente(Freq** listaFreq, char* nomeIngrediente){
    // desreferencia uma vez para pegar o endereço do primeiro nó da lista de freq
    Freq* atual = *listaFreq;
    while(atual != NULL){
        if(strcmp(atual->nome, nomeIngrediente) == 0){
            atual->contagem++;
            return;
        }
        atual = atual->proximo;
    }
    Freq* novo = (Freq*)malloc(sizeof(Freq));
    if(novo != NULL){
        strcpy(novo->nome, nomeIngrediente);
        novo->contagem = 1;
        // o novo nó aponta para onde o ponteiro original apontava
        novo->proximo = *listaFreq;
        // o ponteiro original agora aponta para o endereço do novo nó
        *listaFreq = novo;
    }
}

void liberarFreq(Freq* listaFreq){
    Freq* atual = listaFreq;
    while(atual != NULL){
        // usa um ponteiro temporário para não perder a referência do próximo antes de dar free no atual
        Freq* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
}

void IngredienteMaisUsado(Lista* lista){
    if(lista == NULL || lista->inicio == NULL) return;
    // inicializa um ponteiro nulo para a lista de frequências
    Freq* contagem = NULL;
    No* pratoAtual = lista->inicio;
    while(pratoAtual != NULL){
        Receita* ingAtual = pratoAtual->ingredientes;
        while(ingAtual != NULL){
            // passa o endereço do ponteiro contagem (ponteiro para ponteiro)
            registrarIngrediente(&contagem, ingAtual->ingrediente);
            ingAtual = ingAtual->proxima;
        }
        pratoAtual = pratoAtual->proximo;
    }
    int maiorFreq = 0;
    Freq* fAtual = contagem;
    while(fAtual != NULL){
        if(fAtual->contagem > maiorFreq){
            maiorFreq = fAtual->contagem;
        }
        fAtual = fAtual->proximo;
    }
    fAtual = contagem;
    while(fAtual != NULL){
        if(fAtual->contagem == maiorFreq){
            printf("\n - %s\n", fAtual->nome);
        }
        fAtual = fAtual->proximo;
    }
    liberarFreq(contagem);
}

void IngredientesEmComum(Lista* lista){
    if(lista == NULL || lista->inicio == NULL) return;
    Freq* contagem = NULL;
    No* pratoAtual = lista->inicio;
    while(pratoAtual != NULL){
        Receita* ingAtual = pratoAtual->ingredientes;
        while(ingAtual != NULL){
            registrarIngrediente(&contagem, ingAtual->ingrediente);
            ingAtual = ingAtual->proxima;
        }
        pratoAtual = pratoAtual->proximo;
    }
    Freq* fAtual = contagem;
    while(fAtual != NULL){
        if(fAtual->contagem > 1){
            printf("\n - %s\n", fAtual->nome);
        }
        fAtual = fAtual->proximo;
    }
    liberarFreq(contagem);
}

void liberarIngredientes(Receita* head){
    // recebe o endereço do primeiro nó da sublista de ingredientes
    Receita* atual = head;
    while(atual != NULL){
        Receita* temp = atual;
        atual = atual->proxima;
        // libera a memória da string que foi alocada pelo strdup (ponteiro para char)
        free(temp->ingrediente);
        // libera a memória do nó da sublista
        free(temp);
    }
}

void removerReceita(Lista* lista, char* nome){
    if(lista == NULL || lista->inicio == NULL) return;
    No* atual = lista->inicio;
    while(atual != NULL){
        if(strcmp(atual->Nome, nome) == 0){
            // unlink: reorganiza os ponteiros dos nós vizinhos para tirar o nó atual da corrente
            if(atual->anterior != NULL)
                // o próximo do anterior passa a apontar para o próximo do atual
                atual->anterior->proximo = atual->proximo;
            else
                // se for o primeiro, o início da lista pula para o próximo
                lista->inicio = atual->proximo;

            if(atual->proximo != NULL)
                // o anterior do próximo passa a apontar para o anterior do atual
                atual->proximo->anterior = atual->anterior;

            // libera a memória da sublista (seguindo o ponteiro ingredientes)
            if(atual->ingredientes != NULL)
                liberarIngredientes(atual->ingredientes);

            // libera a memória do nó da receita em si
            free(atual);
            lista->tamanho--;
            printf("\n Receita '%s' removida.\n", nome);
            return;
        }
        atual = atual->proximo;
    }
    printf("\n Receita '%s' nao encontrada.\n", nome);
}

void listarReceitas(Lista* lista){
    if(lista == NULL || lista->inicio == NULL){
        printf("\n Nenhuma receita cadastrada.\n");
        return;
    }
    No* atual = lista->inicio;
    while(atual != NULL){
        printf("\n Nome: %s\n", atual->Nome);
        printf("Preco: %.2f\n", atual->preco);
        printf("Ingredientes:\n");
        // ponteiro auxiliar para varrer a sublista sem perder a referência do nó atual da lista principal
        Receita* ing = atual->ingredientes;
        while(ing != NULL){
            printf(" - %s\n", ing->ingrediente);
            ing = ing->proxima;
        }
        printf("---\n");
        atual = atual->proximo;
    }
}

void listarReceitasPorIngrediente(Lista* lista, char* ingrediente){
    if(lista == NULL || lista->inicio == NULL) return;
    No* atual = lista->inicio;
    int found = 0;
    while(atual != NULL){
        // entra na sublista de ingredientes usando o ponteiro ingredientes do nó atual
        Receita* ing = atual->ingredientes;
        while(ing != NULL){
            if(strcmp(ing->ingrediente, ingrediente) == 0){
                printf("%s (%.2f)\n", atual->Nome, atual->preco);
                found = 1;
                break;
            }
            ing = ing->proxima;
        }
        atual = atual->proximo;
    }
    if(!found){
        printf("Nenhuma receita encontrada com o ingrediente '%s'.\n", ingrediente);
    }
}
