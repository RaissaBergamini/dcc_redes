#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>

#define FILE_SIZE 1000


/*-------------------------------------------------------------
Protótipo: main(int argc, char *argv[])
Função: Executar todas as funções do cliente
Entrada: (int) endereco do host, (int) porta para comunicação, (string) nome do arquivo, (int) tamanho do buffer
Saída: Status da conexão (0 - falta parametros, 1 - sucesso, 2 - falha)
-------------------------------------------------------------*/

int main (int argc, char * argv[])
{
  //Definicao de variaveis
  int sock, r_val_read = 1;
  struct sockaddr_in  server;
  struct hostent *host, *gethostbyname();
  int validacao;
  int status = 2;
  // char t[30]="Será que vai funcionar", texto_arquivo[FILE_SIZE];
  FILE *arq;
  
  int sent_status, j, count = 0, bytes = 0;

  //Criacao de variaveis para verificacao de desempenho
  struct timeval tvalBefore, tvalAfter;
  gettimeofday (&tvalBefore, NULL);

  //verifica se o número de parâmetros enviados está correto
  if (argc < 5)
  {
    printf("Utilização: ./client host_do_servidor porto_servidor nome_arquivo buffer_length_buffer\n");
    return status = 0;
  }

  //atribui parâmetros as variáveis adequadas
  int buffer_length = atoi(argv[4]);
  char msg[buffer_length];
  char buf[buffer_length];

  //Cria e verifica se o socket foi criado corretamente
  if ((sock = socket(AF_INET,SOCK_STREAM,0))==-1)
  {
    printf("Erro ao criar o socket\n");
    exit(1);
  }

  //Definição da estrutura de endereçamento
  host = gethostbyname (argv[1]);
  memcpy ((char*)&server.sin_addr, (char*)host->h_addr, host->h_length);
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[2]));
  
  //Tenta conectar-se ao servidor
  if ( connect(sock, (struct sockaddr *)&server, sizeof (server) ) == -1)
  {
    printf("\nErro de conexão com o servidor através de Socket\n");
    exit(1);
  }
  //printf("Consegui conectar!\n");
  bzero(msg, sizeof(msg));

  // Envia nome do arquivo para o servidor
  for(j=0; argv[3][j] != '\0'; ++j){
    if (count >= buffer_length){
      //envia mensagem pro cliente
      sent_status = write(sock, msg, sizeof(msg)); //(void *) t

      //Se write retornou -1, houve erro na conexão
      if (sent_status < 0){
        perror("writing on stream socket");
        close(sock);
        return status;
      }

      //Caso contrario, mensagem enviada com sucesso
      //printf("Consegui enviar para o cliente uma mensagem: %s\n", msg);
      //Conta bytes enviaqdos
      bytes += count;
      //Contador de caracteres na mensagem
      count = 0;
    } 

    msg[count] =  argv[3][j];  
    count += 1;
  }

  //Manda a mensagem que esta no buffer
  if (count <= buffer_length){
      if(count < buffer_length){
        msg[count] = '\0';
      }
         //printf("Consegui enviar para o cliente uma mensagem: %s\n", msg);
        sent_status = write(sock, msg, sizeof(msg));
        //Se write retornou -1, houve erro na conexão
        if (sent_status < 0){
          perror("writing on stream socket");
          close(sock);
          return status;
          //trabuffer_lengthento de erro 
        }
      bytes += count;
  }

  sent_status = write(sock, "/", 1);
  //Se write retornou -1, houve erro na conexão
  //printf("Consegui enviar para o cliente uma mensagem: %s\n", "/");
  if (sent_status < 0){
    perror("writing on stream socket");
    close(sock);
    return status;
  }

  bzero(msg, sizeof(msg));
  int n_msg =0;
  FILE *output;
  output =fopen("output.txt", "w");
  if(output==NULL) {
      perror("Error opening file.");
      exit(1);
  }
  bytes = 0;
  //Le o conteudo do arquivo de texto enviado pelo servidor
  while (r_val_read > 0 ){ //|| n_msg == 0
    bzero(buf, sizeof(buf));
    r_val_read = recv(sock, buf, buffer_length, 0); //(char *)
    //printf("Consegui dar o read: %d\n", r_val_read);

    //Se read não conseguiu ler nenhum dado, retorna -1
    if ((r_val_read) < 0){
      perror("reading stream message");
      close(sock); 
      return status;
    }

    //Se a mensagem tiver terminado, sai do loop
    else if (r_val_read == 0)
      printf("Nothing to be read\n");

    else if (r_val_read ==1 ){
      if(buf[0] == '/'){
      //printf("Ending read\n");
      break;
      }
    }

    //Senão, continua lendo do buffer
    else{
      //printf("%s\n", buf);
      //printf("Vou tentar escrever no arquivo: %s\n", buf);
      buf[r_val_read] = '\0';
      fprintf(output, "%s", buf);
      //printf("-->%s\n", buf);
      n_msg +=1;
      bytes +=sizeof(buf);
    }
  } 
  fclose(output);
  //Medidas de desempenho
  gettimeofday (&tvalAfter, NULL);
  printf("Tempo em us: %ld us\n",
            ((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L
           +tvalAfter.tv_usec) - tvalBefore.tv_usec);
  // printf ("/n Bytes enviados: %d\n",   bytes);
}