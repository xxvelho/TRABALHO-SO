#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <mpfr.h>
#include <time.h>
#include <stdbool.h>

#define NUM_THREADS 4      // Número de threads trabalhadoras
#define TEMPO_REQ 500      // Constante de tempo que thread dispatcher deve ler o arquivo texto
#define REQ_FILE "requests.txt"     // Nome do arquivo de requisições
#define OUTPUT_FILE "%d.txt" // Formato do nome do arquivo de saída

pthread_mutex_t mutex;    // Mutex para sincronização das threads
pthread_cond_t cond;      // Variável de condição para aguardar novas requisições

typedef struct {
    int i;
    int tempoespera;
} Request;

Request* request_queue;  // Vetor de requisições
int queue_size = 0;      // Tamanho atual do vetor
int queue_capacity = 0;  // Capacidade máxima do vetor
bool all_requests_processed = false;  // Indica se todas as requisições foram processadas
int req_index = 1; //Indica o indice da requisição

char* calculate_pi(int iterations) {
    char* pi_str;
    mpfr_t pi;
    mpfr_init2(pi, iterations + 10); //+10 pq estava ficando com 0000 no final
    mpfr_const_pi(pi, MPFR_RNDN);
    //pi_str = mpfr_get_str(pi_str, NULL, 10, 0, pi, MPFR_RNDN); //2 opção para guardar string vom valor de pi (não funcionou)
    mpfr_asprintf(&pi_str, "%.*Rf", iterations, pi);
    mpfr_clear(pi);
    return pi_str;
}

void process_request(int thread_id, int i, int tempoespera, int req_index) {
    sleep(tempoespera / 1000);  // Simula o tempo de execução da requisição
    // Calcula o valor de PI com i dígitos
    char* pi = calculate_pi(i);

    // Gera o nome do arquivo de saída com base no identificador da thread
    char output_file[20];
    sprintf(output_file, OUTPUT_FILE, thread_id);

    // Abre o arquivo de saída em modo de escrita
    FILE* file = fopen(output_file, "a");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de saída");
        exit(EXIT_FAILURE);
    }

    // Escreve os parâmetros recebidos e o valor de PI no arquivo de saída
    fprintf(file, "Requisição %d: %d;%d;%s\n", req_index, i, tempoespera, pi);

    // Fecha o arquivo de saída
    fclose(file);
}

void* worker_thread(void* arg) {
    int thread_id = *((int*)arg);
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutex);

        // Aguarda até que haja uma requisição no vetor ou todas as requisições tenham sido processadas
        while (queue_size == 0 && !all_requests_processed) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Se todas as requisições foram processadas, a thread pode sair do loop
        if (queue_size == 0 && all_requests_processed) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // Pega a próxima requisição do vetor
        Request request = request_queue[0];
        queue_size--;

        // Desloca as requisições restantes no vetor para a posição correta
        for (int i = 0; i < queue_size; i++) {
            request_queue[i] = request_queue[i + 1];
        }

        pthread_mutex_unlock(&mutex);

        // Processa a requisição
        process_request(thread_id, request.i, request.tempoespera, req_index++);
    }

    pthread_exit(NULL);
}

void* dispatcher_thread(void* arg) {
    srand(time(NULL));
    int num_requests = *((int*)arg);
    free(arg);

    FILE* file = fopen(REQ_FILE, "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo de requisições");
        exit(EXIT_FAILURE);
    }

    // Aloca o vetor de requisições com base no número de requisições
    request_queue = (Request*)malloc(sizeof(Request) * num_requests);
    queue_capacity = num_requests;

    // Gera as requisições aleatórias e as escreve no arquivo
    for (int i = 0; i < num_requests; i++) {
        int rand_i = rand() % 91 + 10;     // Número aleatório de 10 a 100
        int rand_tempoespera = rand() % 1001 + 500;   // Tempo aleatório de 500 a 1500
        fprintf(file, "%d;%d\n", rand_i, rand_tempoespera);

        // Adiciona a requisição ao vetor
        Request request;
        request.i = rand_i;
        request.tempoespera = rand_tempoespera;
        pthread_mutex_lock(&mutex);
        request_queue[queue_size] = request;
        queue_size++;
        
        pthread_mutex_unlock(&mutex);

        // Notifica uma das threads trabalhadoras de que há uma nova requisição disponível
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        

        sleep(TEMPO_REQ / 1000);
    }

    fclose(file);

    // Marca que todas as requisições foram processadas
    pthread_mutex_lock(&mutex);
    all_requests_processed = true;
    pthread_mutex_unlock(&mutex);

    // Notifica todas as threads trabalhadoras para que possam sair do loop
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <quantidade de requisições>\n", argv[0]);
        return 1;
    }

    int num_requests = atoi(argv[1]);

    pthread_t workers[NUM_THREADS];
    pthread_t dispatcher;

    // Inicializa o mutex e a variável de condição
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Cria a thread dispatcher
    int* dispatcher_arg = (int*)malloc(sizeof(int));
    *dispatcher_arg = num_requests;

    if (pthread_create(&dispatcher, NULL, dispatcher_thread, dispatcher_arg) != 0) {
        printf("Erro ao criar a thread dispatcher");
        exit(EXIT_FAILURE);
    }

    // Cria as threads trabalhadoras
    for (int i = 0; i < NUM_THREADS; i++) {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;

        if (pthread_create(&workers[i], NULL, worker_thread, thread_id) != 0) {
            printf("Erro ao criar a thread trabalhadora %d", i);
            exit(EXIT_FAILURE);
        }
    }

    // Aguarda a conclusão da thread dispatcher
    if (pthread_join(dispatcher, NULL) != 0) {
        printf("Erro ao aguardar a conclusão da thread dispatcher");
        exit(EXIT_FAILURE);
    }

    // Aguarda a conclusão das threads trabalhadoras
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(workers[i], NULL) != 0) {
            printf("Erro ao aguardar a conclusão da thread trabalhadora %d", i);
            exit(EXIT_FAILURE);
        }
    }

    // Libera o mutex e a variável de condição
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
