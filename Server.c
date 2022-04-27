#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
// include <netinet/in.h>
// include <arpa/inet.h>

/* Misc constants */
#define MAXLINE 8192 /* Max text line length */
#define LISTENQ 1024 /* Second argument to listen() */

float total = 1000;

struct portfolio
{
     char email[20];
     char name[20];
     char pw[20];
     int company[10];
     float price[10];
};

struct price
{
     char com_name[10][10];
     float price[10];
};

struct prev_price
{
     float price[10];
};

struct request
{
     char filename[20];
     char character;
     int com_idx;
     int quantity;
     float price;
} req[100];

struct thread
{
     int connfd, i;
     socklen_t clientlen;
     struct sockaddr_storage clientaddr; /* Enough room for any addr */
     char client_hostname[MAXLINE], client_port[MAXLINE];

} tot_s[100];

int open_listenfd(char *port)
{
     struct addrinfo hints, *listp, *p;
     int listenfd, optval = 1;
     char host[MAXLINE], service[MAXLINE];
     int flags;

     /* Get a list of potential server addresses */
     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
     hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address AI_PASSIVE - used on server for TCP passive connection, AI_ADDRCONFIG - to use both IPv4 and IPv6 addresses */
     hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number instead of service name*/
     getaddrinfo(NULL, port, &hints, &listp);

     /* Walk the list for one that we can bind to */
     for (p = listp; p; p = p->ai_next)
     {
          /* Create a socket descriptor */
          if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
               continue; /* Socket failed, try the next */

          /* Eliminates "Address already in use" error from bind */
          setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, // line:netp:csapp:setsockopt
                     (const void *)&optval, sizeof(int));

          flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name and port number instead of service name */
          getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, service, MAXLINE, flags);
          printf("host:%s, service:%s\n", host, service);

          /* Bind the descriptor to the address */
          if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
               break;      /* Success */
          close(listenfd); /* Bind failed, try the next */
     }

     /* Clean up */
     freeaddrinfo(listp);
     if (!p) /* No address worked */
          return -1;

     /* Make it a listening socket ready to accept connection requests */
     if (listen(listenfd, LISTENQ) < 0)
     {
          close(listenfd);
          return -1;
     }
     return listenfd;
}

void *myfun(void *s)
{
     printf("New client connected: %u\n", (unsigned int)pthread_self());
     struct thread *st = (struct thread *)s;
     st->clientlen = sizeof(struct sockaddr_storage);
     // st->connfd = accept(st->listenfd, (struct sockaddr *)&(st->clientaddr), &(st->clientlen));
     getnameinfo((struct sockaddr *)&(st->clientaddr), st->clientlen, st->client_hostname, MAXLINE, st->client_port, MAXLINE, 0);
     printf("Connected to (%s, %s)\n", st->client_hostname, st->client_port);
     printf("Start Communication with Client\n");
     size_t n;
     char buf[MAXLINE];
     struct portfolio t;
     n = read(st->connfd, buf, MAXLINE);
     buf[n] = '\0';
     printf("%s\n", buf);
     strcpy(req[st->i].filename, buf);
     FILE *fptr6;
     fptr6 = fopen(req[st->i].filename, "r");
     fread(&t, sizeof(struct portfolio), 1, fptr6);
     fclose(fptr6);
     struct price p;
     FILE *fptr1;
     fptr1 = fopen("Share_price.txt", "r");
     fread(&p, sizeof(struct price), 1, fptr1);
     fclose(fptr1);
     int company_idx;
     while ((n = read(st->connfd, buf, MAXLINE)) != 0)
     {
          int flag = 0;

          buf[n] = '\0';

          printf("%s : %s", t.name, buf);

          if (buf[0] == '\n')
               break;

          char com[10], quantity[10], price[10], j = 0, i = 0, k = 0;
          //Getting company name, quantity and price from client
          while (j <= n - 2)
          {
               if (buf[j] == ' ')
               {
                    if (i == 1)
                         com[k] = '\0';
                    else if (i == 2)
                         quantity[k] = '\0';
                    else if (i == 3)
                         price[k] = '\0';
                    k = 0;
                    i++;
                    j++;

                    continue;
               }
               if (i == 0)
               {
                    j++;
                    continue;
               }
               if (i == 1)
               {
                    com[k] = buf[j];
                    k++;
                    j++;
               }
               if (i == 2)
               {
                    quantity[k] = buf[j];
                    k++;
                    j++;
               }
               if (i == 3)
               {
                    price[k] = buf[j];
                    j++;
                    k++;
               }
          }

          //indexing company based on their names
          if (strcmp(com, "RIL") == 0)
               company_idx = 0;
          else if (strcmp(com, "TATA") == 0)
               company_idx = 1;
          else if (strcmp(com, "ADANI") == 0)
               company_idx = 2;
          else if (strcmp(com, "HDFC") == 0)
               company_idx = 3;
          else if (strcmp(com, "SBI") == 0)
               company_idx = 4;
          else if (strcmp(com, "NYKAA") == 0)
               company_idx = 5;
          else if (strcmp(com, "PAYTM") == 0)
               company_idx = 6;
          else if (strcmp(com, "INFOSYS") == 0)
               company_idx = 7;
          else if (strcmp(com, "BOB") == 0)
               company_idx = 8;
          else if (strcmp(com, "WIPRO") == 0)
               company_idx = 9;

          req[st->i].character = buf[0];
          req[st->i].com_idx = company_idx;
          req[st->i].quantity = atoi(quantity);
          req[st->i].price = atof(price);
          sleep(10);
          fflush(stdout);

          //Checking whether SELL request is present for BUY request and vice versa
          for (int i = 0; i < 100; i++)
          {
               if (req[st->i].character == 'S' && req[i].character == 'B')
               {
                    if (req[i].com_idx == req[st->i].com_idx)
                    {
                         if (req[st->i].quantity == 0 && req[i].quantity == 0)
                         {
                              strcpy(buf, "Transaction Successful!\n");
                              write(st->connfd, buf, strlen(buf));
                              flag = 1;
                         }
                         else if (req[st->i].price <= req[i].price && req[st->i].quantity > 0 && req[i].quantity > 0)
                         {
                              struct prev_price prev_p;

                              for (int j = 0; j < 10; j++)
                              {
                                   prev_p.price[j] = p.price[j];
                              }
                              // updating previous price of stocks 
                              FILE *fptr7;
                              fptr7 = fopen("Prev_price.txt", "w");
                              fwrite(&prev_p, sizeof(struct prev_price), 1, fptr7);
                              fclose(fptr7);
                              p.price[company_idx] = ((total - req[st->i].quantity) * p.price[company_idx] + req[st->i].quantity * req[st->i].price) / total;
                              // t.company[company_idx]-=req[st->i].quantity;

                              //updating txt file based on client transaction
                              // BUYER
                              struct portfolio buyer, seller;
                              FILE *fptr2;
                              fptr2 = fopen(req[i].filename, "r");
                              fread(&buyer, sizeof(struct portfolio), 1, fptr2);
                              fclose(fptr2);
                              buyer.company[company_idx] += req[st->i].quantity;
                              printf("\n\nUpdated Price: %f, Updated Quantity: %d\n", p.price[company_idx], buyer.company[company_idx]);
                              buyer.price[company_idx] = p.price[company_idx];
                              fptr2 = fopen(req[i].filename, "w");
                              fwrite(&buyer, sizeof(struct portfolio), 1, fptr2);
                              fclose(fptr2);
                              fflush(stdout);
                              printf("Name: %s\n", buyer.name);
                              printf("Email: %s\n", buyer.email);
                              printf("PW: %s\n", buyer.pw);
                              printf("QUantity: %d\n", buyer.company[company_idx]);
                              printf("Price: %f\n", buyer.price[company_idx]);

                              // SELLER
                              FILE *fptr3;
                              fptr3 = fopen(req[st->i].filename, "r");
                              fread(&seller, sizeof(struct portfolio), 1, fptr3);
                              fclose(fptr3);
                              seller.company[company_idx] -= req[st->i].quantity;
                              printf("Updated Price: %f, Updated Quantity: %d\n ", p.price[company_idx], seller.company[company_idx]);
                              seller.price[company_idx] = p.price[company_idx];
                              fptr3 = fopen(req[st->i].filename, "w");
                              fwrite(&seller, sizeof(struct portfolio), 1, fptr3);
                              fclose(fptr3);
                              fflush(stdout);
                              printf("Name: %s\n", seller.name);
                              printf("Email: %s\n", seller.email);
                              printf("PW: %s\n", seller.pw);
                              printf("QUantity: %d\n", seller.company[company_idx]);
                              printf("Price: %f\n", seller.price[company_idx]);
                              req[st->i].quantity = 0;
                              req[i].quantity = 0;

                              //updating current price of stocks
                              FILE *fptr8;
                              fptr8 = fopen("Share_price.txt", "w");
                              fwrite(&p, sizeof(struct price), 1, fptr8);
                              fclose(fptr8);

                              strcpy(buf, "Transaction Successful!");
                              write(st->connfd, buf, strlen(buf));
                              flag = 1;
                         }
                    }
               }
               else if (req[st->i].character == 'B' && req[i].character == 'S')
               {
                    fflush(stdout);

                    if (req[i].com_idx == req[st->i].com_idx)
                    {
                         if (req[st->i].quantity == 0 && req[i].quantity == 0)
                         {
                              strcpy(buf, "Transaction Successful!\n");
                              write(st->connfd, buf, strlen(buf));
                              flag = 1;
                         }
                         else if (req[st->i].price >= req[i].price && req[st->i].quantity > 0 && req[i].quantity > 0)
                         {
                              struct prev_price prev_p;

                              for (int j = 0; j < 10; j++)
                              {
                                   prev_p.price[j] = p.price[j];
                              }

                              // updating previous price of stocks 
                              FILE *fptr7;
                              fptr7 = fopen("Prev_price.txt", "w");
                              fwrite(&prev_p, sizeof(struct prev_price), 1, fptr7);
                              fclose(fptr7);
                              p.price[company_idx] = ((total - req[st->i].quantity) * p.price[company_idx] + req[st->i].quantity * req[st->i].price) / total;
                              // t.company[company_idx]-=req[st->i].quantity;

                              //updating txt file based on client transaction
                              // BUYER
                              struct portfolio seller, buyer;
                              FILE *fptr2;
                              fptr2 = fopen(req[i].filename, "r");
                              fread(&seller, sizeof(struct portfolio), 1, fptr2);
                              fclose(fptr2);
                              seller.company[company_idx] -= req[st->i].quantity;
                              printf("\n\nUpdated Price: %f, Updated Quantity: %d\n", p.price[company_idx], seller.company[company_idx]);
                              seller.price[company_idx] = p.price[company_idx];
                              fptr2 = fopen(req[i].filename, "w");
                              fwrite(&seller, sizeof(struct portfolio), 1, fptr2);
                              fclose(fptr2);
                              fflush(stdout);

                              printf("Name: %s\n", seller.name);
                              printf("Email: %s\n", seller.email);
                              printf("PW: %s\n", seller.pw);
                              printf("QUantity: %d\n", seller.company[company_idx]);
                              printf("Price: %f\n", seller.price[company_idx]);

                              // SELLER
                              FILE *fptr3;
                              fptr3 = fopen(req[st->i].filename, "r");
                              fread(&buyer, sizeof(struct portfolio), 1, fptr3);
                              fclose(fptr3);
                              buyer.company[company_idx] += req[st->i].quantity;
                              printf("\n\nUpdated Price: %f, Updated Quantity: %d\n", p.price[company_idx], buyer.company[company_idx]);
                              buyer.price[company_idx] = p.price[company_idx];
                              fptr3 = fopen(req[st->i].filename, "w");
                              fwrite(&buyer, sizeof(struct portfolio), 1, fptr3);
                              fclose(fptr3);

                              printf("Name: %s\n", buyer.name);
                              printf("Email: %s\n", buyer.email);
                              printf("PW: %s\n", buyer.pw);
                              printf("QUantity: %d\n", buyer.company[company_idx]);
                              printf("Price: %f\n", buyer.price[company_idx]);
                              fflush(stdout);
                              req[st->i].quantity = 0;
                              req[i].quantity = 0;

                              //updating current price of stocks
                              FILE *fptr8;
                              fptr8 = fopen("Share_price.txt", "w");
                              fwrite(&p, sizeof(struct price), 1, fptr8);
                              fclose(fptr8);

                              strcpy(buf, "Transaction Successful!");
                              write(st->connfd, buf, strlen(buf));
                              flag = 1;
                         }
                    }
               }
          }
          if (flag == 0)
          {
               strcpy(buf, "Transaction Declined!");
               write(st->connfd, buf, strlen(buf));
          }
     }

     printf("End Communication with Client,pthread : %u\n", (unsigned int)pthread_self());
}

int main(int argc, char **argv)
{
     int listenfd, i = 0;
     listenfd = open_listenfd(argv[1]);
     pthread_t tt[100];

     while (1)
     {
          printf("Iteration: %d\n", i);
          if (tot_s[i].connfd = (accept(listenfd, (struct sockaddr *)&(tot_s[i].clientaddr), &(tot_s[i].clientlen))))
          {
               tot_s[i].i = i;
               int err = pthread_create(&tt[i], NULL, myfun, (void *)&tot_s[i]);//allocating thread to client
               i++;
          }
     }
     exit(0);
}
