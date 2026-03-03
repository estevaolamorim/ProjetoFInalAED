#include "lista.h"
#include "ingredientes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void exibirMenu() {
    printf("----------------Menu----------------:\n");
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
    printf("\n\n\n-----------------------------------------");
}

Lista* criarLista() {
    Lista* novaLista = (Lista*)malloc(sizeof(Lista));
    if (novaLista != NULL) {
        novaLista->inicio = NULL;
        novaLista->tamanho = 0;
    }
    return novaLista;
}

void cadastrarReceita(Lista* lista) {
    if (lista == NULL) return;

    No* novoNo = (No*)malloc(sizeof(No));
    if (novoNo == NULL) return;

    int escolha;

    printf("\n Digite o nome do prato: ");
    fgets(novoNo->Nome, 100, stdin); 
    novoNo->Nome[strcspn(novoNo->Nome, "\n")] = '\0'; // Sanitização para remover quebra de linha do buffer

    printf("\n Digite o preco do prato: \n");
    scanf("%f", &novoNo->preco);

    printf("\n Digite o numero de ingredientes: \n");
    scanf("%d", &escolha);

    /* Limpeza do buffer do teclado necessária para que o próximo fgets 
       não capture o '\n' deixado pelo scanf anterior. */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    novoNo->ingredientes = NULL;

    for (int i = 0; i < escolha; i++) {
        char buffer[100];
        printf("Digite o ingrediente %d: ", i + 1);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        Receita* novoIng = (Receita*)malloc(sizeof(Receita));
        if (novoIng == NULL) continue;
        novoIng->ingrediente = strdup(buffer); // Alocação dinâmica da string para evitar ponteiros pendentes
        novoIng->proxima = NULL;

        // Inserção na cauda da lista de ingredientes (preserva a ordem de inserção)
        if (novoNo->ingredientes == NULL) {
            novoNo->ingredientes = novoIng;
        } else {
            Receita* p = novoNo->ingredientes;
            while (p->proxima != NULL) {
                p = p->proxima;
            }
            p->proxima = novoIng;
        }
    }

    // Inserção no início da lista duplamente encadeada das receitas
    novoNo->proximo = lista->inicio;
    novoNo->anterior = NULL;

    if (lista->inicio != NULL) {
        lista->inicio->anterior = novoNo;
    }
    
    lista->inicio = novoNo;
    lista->tamanho++;
}

void cadastrarIngredienteReceita(Lista* lista, char* nomePrato){
    if(lista == NULL || lista->inicio == NULL) return;
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
                
                if(atual->ingredientes == NULL){
                    atual->ingredientes = novoIng;
                }else{
                    Receita* p = atual->ingredientes;
                    while(p->proxima != NULL){
                        p = p->proxima;
                    }
                    p->proxima = novoIng;
                }
            }
            break;
        }
        atual = atual->proximo;
    }
}

void listarIngredientesReceita(Lista* lista, char* nomePrato){
    if(lista == NULL || lista->inicio == NULL) return;
    No* atual = lista->inicio;
    while(atual != NULL){
        if(strcmp(atual->Nome, nomePrato) == 0){
            Receita* ingAtual = atual->ingredientes;
            while(ingAtual != NULL){
                printf("\n - %s\n", ingAtual->ingrediente);
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
    No* menorPrato = atual;
    float menor = atual->preco;
    while(atual != NULL){
        if(atual->preco < menor){
            menor = atual->preco;
            menorPrato = atual;
        }
        atual = atual->proximo;
    }
    printf("\n Nome: %s\nPreco: %.2f\n", menorPrato->Nome, menorPrato->preco);
}

// Estrutura auxiliar para mapear a contagem de nomes únicos
typedef struct Freq{
    char nome[100];
    int contagem;
    struct Freq* proximo;
} Freq;

void registrarIngrediente(Freq** listaFreq, char* nomeIngrediente){
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
        novo->proximo = *listaFreq; // Inserção no início da lista de frequência por performance
        *listaFreq = novo;
    }
}

void liberarFreq(Freq* listaFreq){
    Freq* atual = listaFreq;
    while(atual != NULL){
        Freq* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
}

void IngredienteMaisUsado(Lista* lista){
    if(lista == NULL || lista->inicio == NULL) return;
    Freq* contagem = NULL;
    No* pratoAtual = lista->inicio;
    
    // Varredura completa: N receitas * M ingredientes
    while(pratoAtual != NULL){
        Receita* ingAtual = pratoAtual->ingredientes;
        while(ingAtual != NULL){
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
    
    // Tratamento para empate: imprime todos os ingredientes com a frequência máxima encontrada
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
    Receita* atual = head;
    while(atual != NULL){
        Receita* temp = atual;
        atual = atual->proxima;
        free(temp->ingrediente); // Importante: liberar string alocada por strdup antes da struct
        free(temp);
    }
}

void removerReceita(Lista* lista, char* nome){
    if(lista == NULL || lista->inicio == NULL) return;
    No* atual = lista->inicio;
    while(atual != NULL){
        if(strcmp(atual->Nome, nome) == 0){
            // Ajuste de encadeamento para exclusão em lista dupla
            if(atual->anterior != NULL)
                atual->anterior->proximo = atual->proximo;
            else
                lista->inicio = atual->proximo;

            if(atual->proximo != NULL)
                atual->proximo->anterior = atual->anterior;

            if(atual->ingredientes != NULL)
                liberarIngredientes(atual->ingredientes);

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
        Receita* ing = atual->ingredientes;
        while(ing != NULL){
            if(strcmp(ing->ingrediente, ingrediente) == 0){
                printf("%s (%.2f)\n", atual->Nome, atual->preco);
                found = 1;
                break; // Otimização: interrompe a busca na sublista de ingredientes se já encontrou o alvo
            }
            ing = ing->proxima;
        }
        atual = atual->proximo;
    }
    if(!found){
        printf("Nenhuma receita encontrada com o ingrediente '%s'.\n", ingrediente);
    }
}
