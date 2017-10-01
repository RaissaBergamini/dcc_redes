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
int main (int argc, char * argv[])
{
  //Definicao de variaveis
  int sock, r_val_read = 1;
  struct sockaddr_in  server;
  struct hostent *host, *gethostbyname();
  int validacao;
  int status = 2;
  char t[30]="Será que vai funcionar", texto_arquivo[FILE_SIZE];
  FILE *arq;
  
  int sent_status, j, count = 0, bytes = 0;

  //verifica se o número de parâmetros enviados está correto
  if (argc < 5)
  {
    printf("Utilização: ./client host_do_servidor porto_servidor nome_arquivo buffer_length_buffer\n");
    exit(EXIT_FAILURE);
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
  printf("Consegui conectar!\n");
  
  //Envia nome do arquivo para o servidor
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
      printf("Consegui enviar para o cliente uma mensagem: %s\n", msg);
      //Conta bytes enviados
      bytes += count;
      //Contador de caracteres na mensagem
      count = 0;
    } 

    // printf("Vou tentar printar o char\n");
    // printf("%c",argv[3][j]);
    // printf("Vou tentar dar strcat em: %s e %c\n", msg, argv[3][j]);
    msg[count] =  argv[3][j];  
    // printf("Consegui dar strcat\n");
    //Conta quantos caracteres foram adicionados
    count += 1;
  }
  
  //Manda a mensagem que esta no buffer
  if (count <= buffer_length){
      if(count < buffer_length){
        msg[count] = '\0';
      }
         printf("Consegui enviar para o cliente uma mensagem: %s\n", msg);
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
  sent_status = write(sock, "", 0);
  //Se write retornou -1, houve erro na conexão
  printf("Consegui enviar para o cliente uma mensagem: %s\n", "");
  if (sent_status < 0){
    perror("writing on stream socket");
    close(sock);
    return status;
  }
  // close(sock);

  // Tenta conectar-se ao servidor
  // if ( connect(sock, (struct sockaddr *)&server, sizeof (server) ) == -1)
  // {
  //   printf("\nErro de conexão com o servidor através de Socket\n");
  //   exit(1);
  // }
  // printf("Consegui conectar!\n");

  int n_msg =0;
  //Le o conteudo do arquivo de texto enviado pelo servidor
  while (r_val_read > 0){
    r_val_read = read(sock, buf, buffer_length); //(char *)
    printf("Consegui dar o read: %d\n", r_val_read);

    //Se read não conseguiu ler nenhum dado, retorna -1
    if ((r_val_read) < 0){
      perror("reading stream message");
      close(sock); 
      return status;
    }

    //Se a mensagem tiver terminado, read retorna 0
    else if (r_val_read == 0)
      printf("Ending connection\n");

    //Senão, continua lendo do buffer
    else{
      printf("%s\n", buf);
      printf("Vou tentar dar strcat em: %s e %s\n", texto_arquivo, buf);
      int i = 0;
      for(i=0; i < sizeof(buf); i++ ){
        texto_arquivo[buffer_length*n_msg + i] = buf[i];
      }
      n_msg += 1;
      printf("Consegui dar strcat\n");
      printf("-->%s\n", buf);
    }
  } 
}

      
  /*
 *  A partir deste ponto o cliente já está em comunicação com o
 *  servidor através do socket ---> sock
 */

  /*
 *  Insira neste ponto todo o código necessário para a comunicação
 *  cliente/servidor
 */

  /*  
 // *  O cliente lê do servidor um caracter
 // */
 //  validacao = read (sock, (char *) &resp, 1);
 //  /*
 // *  Ecoa no terminal do cliente o caracter recebido do servidor
 // */
 //  printf("\nConsegui ler do servidor o caracter: %c\n\n",resp);

    
 // *  O cliente escreve no servidor uma string de 30 caracteres
 
 //  validacao = write (sock, (void *)t, 30);
 //  /*
 // *  Ecoa no terminal do cliente a string enviada para o servidor
 // */
 //  printf("\nConsegui escrever para o servidor a string: %s\n",t);
