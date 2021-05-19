#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <iostream>
#include "header.hpp"
#include <list>
#include <deque>

/*
Bibliotecas que poderão ser úteis
#include <stack>
#include <queue>
*/
using namespace std;
/*
struct Atrib: descreve atributos a serem considerados para escalonamento da tarefa. O exemplo abaixo é uma configuração mínima, podendo ser adicionados outros atributos.O campo p representa a prioridade da tarefa e o campo c o seu custo computacional. Note: o custo computacional deverá ser indicado pelo programador.
*/
struct Atrib {
  int p;
  int c;
}typedef Atrib;

struct Trabalho {
   int tid;
   void* (*f)(void*); 
   void* numFib; // Parâmetro de entrada para f, no caso o número que é  fibonacci
   void* res; // Retorno de f
}typedef Trabalho;

//struct Trabalho *ListaTrabalhosProntos, *ListaTrabalhosTerminados;
//Lista TrabalhoProntos e trabalhos terminados
struct deque<struct Trabalho *> listaTrabalhosProntos, listaTrabalhosTerminados;
deque<struct Trabalho *>::iterator iteratorList;


//Lista de PVs:
static pthread_t *pvs;
static int qtdPVs = 5;
// Fim
static bool Fim;
pthread_mutex_t lock =  PTHREAD_MUTEX_INITIALIZER;
void* exemploTarefa( void* p ){
  int n1 = (int) p;
  //pthread_mutex_lock(&lock)
  cout << "Teste" ;
  //cout << n1; 
  //pthread_mutex_unlock(&lock);
  return (int*) n1;
}

int main()
{
    int t1;
    struct Atrib a1;
    a1.c = 0;
    a1.p = 0;
    int n1 = 10;
    // struct Trabalho * teste;
    // teste = (Trabalho *) malloc(sizeof(Trabalho));
    // teste->res = NULL;
    // teste->tid = 1;
    // teste->numFib = (void*) 10;
    // teste->f = exemploTarefa;
    /*
    cout << "Quantidade de processadores virtuais: " << endl;
    cin >> qtdPVs;
    cin.ignore();
    */
    t1 = spawn( &a1, exemploTarefa, (void*) n1 );
    t1 = spawn( &a1, exemploTarefa, (void*) n1 );
    t1 = spawn( &a1, exemploTarefa, (void*) n1 );
    t1 = spawn( &a1, exemploTarefa, (void*) n1 );
    if(!start(qtdPVs)){
      cout << "Criação de Processadores virtuais falhou" << endl;
      exit(EXIT_FAILURE);
    }

  cout << "teste";
    finish();
}

void* MeuPV(void* dta) {
  /*
 void* res;
 res = NULL;
 int n;
 Trabalho *t;
 pthread_t tread_aux;
 //Coisas que esse while faz: 
 //1° - Ele "consome" uma chamada de função da lista de Trabalhos prontos para serem processados
 //2° - Ele "sincroniza" o resultado de uma chamada de função ???
 //3° - Ele armazena o resultado adquirido na sincronização na lista de trabalhos terminados 
  int i = 0;
// for(iteratorList = listaTrabalhosProntos.begin(); iteratorList != listaTrabalhosProntos.end(); iteratorList++){
   
// }
cout << listaTrabalhosProntos.size();
 
 while(Fim == false && !(listaTrabalhosProntos.empty())) {
  t = listaTrabalhosProntos.front(); //--->> Aqui o PV tem comportamento de consumidor
   pthread_mutex_lock(&lock);
  res = t->f(t->numFib);
  n = (int) res;
  cout << "resultado" << n;
  t->res = res;
  listaTrabalhosTerminados.push_front(t);
  listaTrabalhosProntos.pop_front();
  pthread_mutex_unlock(&lock);

  
    // res = t->f( Trabalho->dta );   --->> vai para AAA
    // ArmazenaResultados(t,res); --->> Coloca na Lista de Terminados
  
 }
 
  return NULL;
  */
}

// eu crio os pv executando a função MeuPV, como o fim é false ele vai ficar rodando até ter algum trabalho na lista, qndo tiver um trabalho, ele deveria executar o trabalho. Como eu faria pra ele executar o fibonacci?
int start( int qtdpvs ) {
 int thread_status;
 pvs =(pthread_t *) malloc(qtdpvs*sizeof(pthread_t));
 Fim = false;
 for( int i = 0 ; i < qtdpvs ; i++ ){
   thread_status = pthread_create(&(pvs[i]),NULL, MeuPV , NULL);
   if(thread_status != 0)
    {
      // Seguindo documentação do trabalho, se ocorrer erro na criação retorna 0;
      return 0;
    }
 } 
 return 1; // Criação bem-sucedida
}

void finish() {
  int thread_status;
  Fim = true;
  for( int i = 0 ; i < qtdPVs;i++ ){
   thread_status = pthread_join(pvs[i], NULL);
   if(thread_status != 0)
    {
      // Caso ocorra erro no join da threads;
      printf ("Erro ao finalizar Processadores Virtuais.\n");
      exit(EXIT_FAILURE);
    }
  }
  free(pvs);
}

// struct TrabalhoPronto {
//    int tid;
//    void* (*f)(void*);
//    void* numFib; // Parâmetro de entrada para f, no caso o número que é  fibonacci
//    void* res; // Retorno de f
// };
// Essa função é responsável por colocar a função na lista de trabalhos prontos. ??? 
// eu tentei pegar os atributos da função Spawn e colocar numa variavel do tipo Trabalho, depois colocar essa variavel na listaTrabalhosProntos.
// Como funcionaria a sincronização??? 
int spawn( struct Atrib* atrib, void *(*t) (void *), void* dta ){
  struct Trabalho *trab;
  trab = (Trabalho *) malloc(sizeof(Trabalho));
  trab->tid = 2;        //Como definir o id???
  trab->f = t;
  trab->numFib = dta;
  trab->res = NULL;
  listaTrabalhosProntos.push_front(trab);
}

//// struct Trabalho {
//    int tid;
//    void* (*f)(void*);
//    void* numFib; // Parâmetro de entrada para f, no caso o número que é  fibonacci
//    void* res; // Retorno de f
// };
// Essa Função verifica se o resultado da função já foi calculado
// Se foi, Ele pega um Trabalho da função listaTrabalhosTerminados pelo ID e devolve o resultado
// Como pegar um elemento da lista pra comparar com o tId passado no sync
int sync( int tId, void** res ){
    void *resp;
    Trabalho *aux;
    for(iteratorList = listaTrabalhosTerminados.begin(); iteratorList != listaTrabalhosTerminados.end(); iteratorList++){
        aux = listaTrabalhosTerminados.front();
        if(aux->res != NULL){
          if(aux->tid == tId){
            resp = aux->res; // Em teoria sucesso 
            
            listaTrabalhosTerminados.push_front(aux);
            return 1;
          }
        }
    }
    return 0;
}


/*
possivel ordem de execução do programa
-main -
      - createPtreads(3)
      -               - MeuPV - id 1(fica rodando esperando algo aparecer)
      -               - spawn(fibo 2)
      -               -               - Coloca na lista de trabalhos prontos (fibo 4)
      -               - MeuPV-1
      -               -               - MeuPV-1 executa fibo 2
      -               -               -                       - MeuPV-1 chama Spawn (fibo 1)
      -               -               -                       -                      - TrabalhosProntos(fibo1)
      -               -               -                       -                      - MeuPV-2 Executa Fibo 1
      -               -               -                       -                      - TrabalhoTerminado(fibo1)
      -               -               -                       - MeuPV-1 chama Spawn (fibo 0)
      -               -               -                       -                      - TrabalhoProntos(fibo0)
      -               -               -                       -                      - MeuPV-3 Executa Fibo 0
      -               -               -                       -                      - TrabalhoTerminado(fibo0)
      -               -               -                       - MeuPV-1 sync fibo 1(res= 1)
      -               -               -                       - MeuPV-1 sync fibo 0(res= 1 - deveria pegar 0)
      -               - Como definir o fim???
-Retorna Resultado
-Exibe Resultado
-finish()
      -               -
Fim poderia ser definido com a propriedade profundidade?
Fim poderia ser definido com tempo ocioso da função MEUPV?
O programa rodaria a função finish?
  - 
função fibonacci do exemplo ta errada> fibo 0 é 0 e não 1
COMO MONTAR OS IDS
  sempre q ser feito uma chamada para um spawn adicionar +1 no id
OQ ACONTECE SE O NÚMERO DE PVS FOR MENOR QUE O NÚMERO DE CHAMADAS DE SPAWNS? (VAI SER NA MAIORIA DOS CASOS, CAUSANDO DEADLOCK)

*/

