#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <iostream>
#include <unistd.h>
#include "header.hpp"
#include <list>
#include <deque>

using namespace std;

struct Atrib {
  int p;
  int c;
}typedef Atrib;

struct Trabalho {
   int tid;
   void* (*f)(void*); 
   int numFib; // Parâmetro de entrada para f, no caso o número que é  fibonacci
   int res; // Retorno de f
}typedef Trabalho;


struct deque<struct Trabalho *> listaTrabalhosProntos, listaTrabalhosTerminados;
deque<struct Trabalho *>::iterator iteratorList;


//Lista de PVs:
static pthread_t *pvs;
static int qtdPVs = 200;
static bool Fim = false;
pthread_mutex_t lock, lock2, lock3 =  PTHREAD_MUTEX_INITIALIZER;

void printIdLista(deque<struct Trabalho *> x){
    for (int i = 0; i < x.size(); i++){
      cout << x.at(i)->tid << " ";
    }
    cout << endl;
}

int contador = 0;
void* fibo( void* dta ) {
  int  *n = (int*)dta;
  cout << *n;
  int *n1, *n2,
      *r = (int *) malloc(sizeof(int)),
      *r1, *r2;
  struct Atrib a1, a2;

  if( *n <= 2 ) *r = 1;
  else {
    contador++;
    n1 = (int *) malloc(sizeof(int));
    *n1 = *n - 1;
    a1.p = 0; a1.c = *n1;
    spawn( &a1, fibo, (void*) n1 );
    n2 = (int *) malloc(sizeof(int));
    *n2 = *n - 2;
    a2.p = 0; a1.c = *n2;
    spawn( &a2, fibo, (void*) n2 );
    
    
    sync(contador, &r1);
    contador++;
    sync(contador, &r2);
    *r = *r1 + *r2;

    free(r1);    
    free(r2);
    free(n1);    
    free(n2);
  }
  return r;
}

int main()
{
    int t1, *r, resultadoFibonacci;
    struct Atrib a1;
    a1.c = 0;
    a1.p = 0;
    int numeroFibonacci = 9;

    t1 = spawn( &a1, fibo, &numeroFibonacci);
    
   
    if(!start(qtdPVs)){
      cout << "Criação de Processadores virtuais falhou" << endl;
      exit(EXIT_FAILURE);
    }
    resultadoFibonacci = sync (0, &r);
    
  
    finish();
    cout << "fibonacci("<< numeroFibonacci <<") = " << resultadoFibonacci;
   
}

void* MeuPV(void* dta) {
 int *recebeResultadoFuncao;
 Trabalho *t;

 pthread_mutex_lock(&lock);
 while (Fim == false && !listaTrabalhosProntos.empty()){
   t = listaTrabalhosProntos.front();
   cout << "MeuPv Executando tarefa " << t->tid << "..." <<endl;
   listaTrabalhosProntos.pop_front();

   recebeResultadoFuncao = (int*) t->f((void*) &(t->numFib));
   t->res = *recebeResultadoFuncao;

   listaTrabalhosTerminados.push_front(t);
   cout << "MeuPv terminou a execução da tarefa " << t->tid << "! Printando tarefas terminadas: ";
   printIdLista(listaTrabalhosTerminados);
 }
  pthread_mutex_unlock(&lock); 
  return NULL;
}

int start( int qtdpvs ) {
 int thread_status;
 pvs = (pthread_t *) malloc(qtdpvs*sizeof(pthread_t));
 Fim = false;
 for( int i = 0 ; i < qtdpvs ; i++ ){
   thread_status = pthread_create(&(pvs[i]),NULL, MeuPV , NULL);
   if(thread_status != 0)
      return 0;// Seguindo documentação do trabalho, se ocorrer erro na criação retorna 0;  ;
    
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

int idTarefa = -1;
int spawn( struct Atrib* atrib, void *(*t) (void *), void* dta ){
  int *x = (int*) dta;
  idTarefa++;
  cout << "Armazenando tarefa " << idTarefa << " na lista de Trabalhos Prontos..." << endl;
  struct Trabalho *trab;
  trab = (Trabalho *) malloc(sizeof(Trabalho));
  trab->tid = idTarefa;        
  trab->f = t;
  trab->numFib = *x;
  trab->res = 0;
  listaTrabalhosProntos.push_front(trab);
  cout << "Lista de Trabalhos Prontos: ";
  printIdLista(listaTrabalhosProntos);
  return 0;
}

int wait;

int sync( int tId, int **res ){
    
    cout << "sync foi chamado para executar a tarefa " << tId <<"..." << endl;
    bool tarefaEstaEmListaTrabalhosProntos = false;
    bool tarefaEstaEmListaTrabalhosTerminados = false;
    int  ResultadoTarefa;
    Trabalho *trabalhoPronto;
    cout << "sync percorrendo lista de prontos..." << endl;
    for(int i = 0; i < listaTrabalhosProntos.size(); i++){ //caso 1   
        if(tId == listaTrabalhosProntos.at(i)->tid){ //tarefa na lista de prontas
          cout << "tarefa " << tId << " está na lista de prontos! Executando..." << endl;
          tarefaEstaEmListaTrabalhosProntos = true;
        
          trabalhoPronto = listaTrabalhosProntos.at(i);
          listaTrabalhosProntos.erase(listaTrabalhosProntos.begin()+i);  
          *res = (int*) trabalhoPronto->f((void*) &(trabalhoPronto->numFib));
          trabalhoPronto->res = **res;
         
          
          cout << "Tarefa "<< tId << " executada com sucesso! Lista de tarefas terminadas: ";
          printIdLista(listaTrabalhosTerminados);
          cout << "Lista de tarefas prontas: ";
          printIdLista(listaTrabalhosProntos);
          return **res;
        }     
    }
    if (!tarefaEstaEmListaTrabalhosProntos){//caso 2: verifica se está em listaTrabalhosTerminados e a retira 
        cout << "Não está em prontos. Verificando se está em trabalhos terminados..." << endl;
        for (int i = 0; i < listaTrabalhosTerminados.size(); i++){
          if(tId == listaTrabalhosTerminados.at(i)->tid){
            ResultadoTarefa = listaTrabalhosTerminados.at(i)->res;
            cout << "Tarefa " << tId << " Está em trabalhos terminados, removendo.." << endl;
            tarefaEstaEmListaTrabalhosTerminados = true;
            listaTrabalhosTerminados.erase(listaTrabalhosTerminados.begin()+i);
            cout <<"Tarefa " << tId << " removida!" << endl;
            return ResultadoTarefa;
          }
        }               
    } 
      
    if (!tarefaEstaEmListaTrabalhosTerminados && !tarefaEstaEmListaTrabalhosProntos){//caso 3: tarefa esta sendo executada, esperar um pouco;
        for (wait = 0; wait < 3; wait++)
            cout << "Tarefa " << tId << " pode estar sendo executada por meuPV. Esperando..." << endl;  
    }
    cout << "Chamada de tarefa incorreta!" << endl;
    return 0;
}


