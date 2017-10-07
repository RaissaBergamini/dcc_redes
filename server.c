/* 
 *  Bibliotecas Padrões que devem ser incluidas
 */
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
#include <stdbool.h>

#define FILE_NAME_SIZE 200

/*-------------------------------------------------------------
Protótipo: inicializatabela (dadoscaminho tabela1[MAX_TAM][MAX_TAM], int altura, int largura)
Função: colocar os valores inicias na tabela
Entrada: uma tabela de itens do tipo dadoscaminho (um ponteiro para dados), a altura e a largura do mapa.
Saída: Tabela com os valores iniciais setados.
-------------------------------------------------------------*/

int main (int argc, char * argv[])
{
  //verifica se o número de parâmetros enviados está correto
  int sock, length, buffer_length, connected, r_val_read = 1, port;
  int status = 2;
  if (argc < 3){
    printf("Erro! Falta informações: porta_servidor ou buffer_length_buffer");
    status = 0;
    return status; 
  }
  
  //atribui parâmetros as variáveis adequadas
  port = atoi(argv[1]); 
  buffer_length =atoi(argv[2]);

  //criação de variaveis uteis para a manipulação de sockets
  struct sockaddr_in  servidor, cliente;
  char buf[buffer_length];
  char arquivo[FILE_NAME_SIZE];
  char msg[buffer_length];

  //Cria e verifica se o socket foi criado corretamente
  sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock==-1)
  {
    perror("opening stream socket");
    // perror("Erro ao criar o socket\n");
    return status;
    // exit(1);
  }

   //Definição da estrutura de endereçamento
  servidor.sin_family = AF_INET;
  int address = INADDR_ANY;
  servidor.sin_addr.s_addr = htonl (address);
  printf("Socket has name %d\n", address);
  //htons -> convert from host to network format
  servidor.sin_port = htons(port);
  printf("Socket port #%d\n",ntohs(servidor.sin_port));

  //Associação da estrutura de enderaçamento ao socket
  if (bind(sock,(struct sockaddr *)&servidor,sizeof(servidor)) == -1) // <0
  {
    perror("binding stream socket");
    return status;
    // printf("Erro no bind!\n");
    // exit(1);
  }

  //Escutando... (maximo de 10 clientes na fila)
  listen(sock,10);
  length = sizeof (cliente);
  int n_msg = 0;
  int count = 0;
  int bytes = 0;
  int sent_status; 
  bool end_file = false;
  bzero(arquivo, sizeof(arquivo));
  FILE *arq;
  char ch;
  for (;;) {
    connected = accept(sock, (struct sockaddr *)&cliente,&length);
    printf("Conectou 1 cliente....\n");
    if (connected == -1){
      perror("accept");
      return status;
    }
    else {
      while (r_val_read > 0){
        bzero(buf, sizeof(buf));
        //Tenta ler mensagem do socket
        r_val_read = read(connected, buf, buffer_length); //(char *)
        printf("Consegui dar o read: %d\n", r_val_read);
        //Se read não conseguiu ler nenhum dado, retorna -1
        if ((r_val_read) < 0){
          perror("reading stream message");
          close(connected);
          close(sock);
          return status;
        }
        //Se a mensagem tiver terminado, read retorna 0
        else if (r_val_read ==1 ){
          if(buf[0] == '/'){
          printf("Ending read\n");
          break;
          }
        }
        //Senão, continua lendo do buffer
        else{
          printf("%s\n", buf);
          printf("Vou tentar dar strcat em: %s e %s\n", arquivo, buf);
          int i = 0;
          for(i=0; i < sizeof(buf); i++ ){
            arquivo[buffer_length*n_msg + i] = buf[i];
          }
          n_msg += 1;
          printf("Consegui dar strcat\n");
          printf("-->%s\n", buf);
          printf("Valor de Rvalue = %d\n", r_val_read);
        }
      }
      bzero(msg, sizeof(msg));
      printf("Terminei a leitura\n");
      printf("Consegui ler o nome do arquivo: %s\n", arquivo);
  //     break;
  //   }
  // }
  // close(sock);
  //   //Iniciar outra conexao
  // for (;;) {
  //   connected = accept(sock, (struct sockaddr *)&cliente,&length);
  //   printf("Conectou 1 cliente....\n");
  //   if (connected == -1){
  //     perror("accept");
  //     return status;
  //   }
  //   else {
      // arquivo = "nome_do_arquivo.txt";
      arq = fopen ("nome_do_arquivo.txt", "r"); // abrir o arquivo para ler
      printf("Consegui abrir o arquivo.\n");
      if (arq == NULL) {
        printf ("Houve um erro ao abrir o arquivo.\n");
        close(sock);
        return status;
      } 
      printf("Consegui abrir o arquivo.\n");
      while(end_file == false){
        if (count >= buffer_length){
          msg[count] = '\0';
          //envia mensagem pro clientee
          printf("Tentar mandar para o cliente\n");
          sent_status = write(connected, msg, sizeof(msg)); //(void *) &resp
          // validacao = write (connected, (void *) &resp, 1);
          //Se write retornou -1, houve erro na conexão
          printf("Status de envio: %d\n", sent_status);
          if (sent_status < 0){
            perror("writing on stream socket");
            close(sock);
            return status;
          }
          printf("Consegui enviar para o cliente uma mensagem: %s\n", msg);
          //Caso contrario, mensagem enviada com sucesso
          bytes += count;
          count = 0;
          bzero(msg, sizeof(msg));
          // msg = "";
        } 
        ch = getc(arq); 
        if (ch == EOF){
          end_file = true;
          continue; 
        }  
        printf("Consegui ler um char: %c\n", ch);
        // printf("%c",ch);
        msg[count]= ch;  
        count += 1;
        // vch = getc(arq);

      // }
      // printf("O nome do arquivo foi lido e vamos enviar seu conteudo para o cliente!\n" );
      }

      
      if (msg != ""){
          sent_status = write(connected, msg, sizeof(msg));
          //Se write retornou -1, houve erro na conexão
          if (sent_status < 0){
            perror("writing on stream socket");
            close(sock);
            return status;
            //trabuffer_lengthento de erro 
          }
          printf("Consegui enviar para o cliente uma mensagem: %s\n", msg);
        bytes += count;
      }

      sent_status = write(connected, "/", 1);
      printf("Consegui enviar para o cliente uma mensagem: %s\n", "/");
      //Fecha conexão e arquivo
      break;
      // }
      // Calcula tempo
        
     }
   }
  fclose(arq);
  printf ("Nome do Arquivo: %s, /n Bytes enviados: %d\n", arquivo, bytes);
  printf("Closing connection");
  close(sock);
  // unlink(address);
  status = 1;
  return status;
}
  

  /*
 *  A partir deste ponto o clientee já está em comunicação com o
 *  servidor através do socket ---> sock
 */
 
  /*
 *  Insira neste ponto todo o código necessário para a comunicação
 *  clientee/servidor
 */
  
  /*  
 *  Le e escreve pelo socket 
 */
 //  resp ='A';

 //  /*  
 // *  O clientee lê do servidor um caracter
 // */
 //  validacao = write (connected, (void *) &resp, 1);
 //  /*
 // *  Ecoa no terminal do clientee o caracter recebido do servidor
 // */
 //  printf("\nConsegui escrever para o clientee o caracter: %c\n\n",resp);
  
 //  /*  
 // *  O clientee escreve no servidor uma string de 30 caracteres
 // */
 //  validacao = read (connected, (char *) t, 30);
 //  /*
 // *  Ecoa no terminal do clientee a string enviada para o servidor
 // */
 //  printf("\nConsegui ler do clientee a string: %s\n",t);

