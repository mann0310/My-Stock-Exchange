#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
// include <netinet/in.h>
// include <arpa/inet.h>

#define MAXLINE 8192 /* Max text line length */

void red()
{
     printf("\033[1;31m");
}

void green()
{
     printf("\033[1;32m");
}

void yellow()
{
     printf("\033[1;33m");
}

void blue()
{
     printf("\033[1;34m");
}

void mag()
{
     printf("\033[1;35m");
}

void cyan()
{
     printf("\033[1;36m");
}

void bold()
{
     printf("\e[1m");
}

void boff()
{
     printf("\e[0m");
}

void reset()
{
     printf("\033[0m");
}

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

int open_clientfd(char *hostname, char *port)
{
     int clientfd;
     struct addrinfo hints, *listp, *p;
     char host[MAXLINE], service[MAXLINE];
     int flags;
     /* Get a list of potential server addresses */
     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_socktype = SOCK_STREAM; /* Open a connection */
     hints.ai_flags = AI_NUMERICSERV; /* ... using a numeric port arg. */
     hints.ai_flags |= AI_ADDRCONFIG; /* Recommended for connections where we get IPv4 or IPv6 addresses */
     getaddrinfo(hostname, port, &hints, &listp);

     /* Walk the list for one that we can successfully connect to */
     for (p = listp; p; p = p->ai_next)
     {
          /* Create a socket descriptor */
          if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
               continue;                           /* Socket failed, try the next */
          flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name and port number instead of service name */
          getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, service, MAXLINE, flags);
          printf("host:%s, service:%s\n", host, service);
          /* Connect to the server */
          if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
          {
               printf("Connected to server %s at port %s\n", host, service);
               break; /* Success */
          }
          close(clientfd); /* Connect failed, try another */ // line:netp:openclientfd:closefd
     }

     /* Clean up */
     freeaddrinfo(listp);
     if (!p) /* All connects failed */
          return -1;

     else /* The last connect succeeded */
          return clientfd;
}

int main(int argc, char **argv)
{
     struct portfolio s, t;
     int clientfd, n, fd;
     char *host, *port, buf[MAXLINE];
     host = argv[1];
     port = argv[2];
     char mail[20], pw[20], *ext, store[20], filename[30];

     ext = ".txt";
     blue();
     printf("\n----------------------------------------\n");
     reset();
     printf("Press 1: Log in\n");
     printf("Press 2: New User\n");
     blue();
     printf("\n----------------------------------------\n\n");
     reset();
     printf("User Input: ");
     scanf("%d", &n);
     blue();
     printf("\n\n----------------------------------------\n\n");
     reset();
     printf("Enter Email: ");
     scanf("%s", mail);

     if (n == 1)
     {
          strcpy(store, mail);
          strcat(store, ext);
          strcpy(filename, store);
          // printf("%s\n", filename);

          FILE *fptr1;
          fptr1 = fopen(filename, "r");
          if (fread(&s, sizeof(struct portfolio), 1, fptr1) == 0)
          {
               printf("No user found!. If you are a new user, enter your information by pressing 2\n");
               fclose(fptr1);
          }
          else
          {
               fclose(fptr1);
               do
               {
                    printf("Enter Password: ");
                    scanf("%s", pw);
                    int cmp = strcmp(s.pw, pw);

                    if (cmp != 0)
                    {
                         printf("Invalid Email or password\n");
                         n++;
                         if (n == 4)
                         {
                              printf("Limit Exceeded!\n");
                              exit(0);
                         }
                         continue;
                    }
                    break;
               } while (n <= 3);
          }
     }

     else
     {
          char name[20];
          printf("Enter your Name: ");
          scanf("%s", name);
          printf("Enter your Password: ");
          scanf("%s", pw);
          // printf("%s",pw);
          strcpy(store, mail);
          strcat(store, ext);
          strcpy(filename, store);
          FILE *fptr;
          fptr = fopen(filename, "w");
          strcpy(t.email, mail);
          strcpy(t.name, name);
          strcpy(t.pw, pw);
          // printf("%s",t.pw);
          fwrite(&t, sizeof(struct portfolio), 1, fptr);
          fclose(fptr);
     }

     clientfd = open_clientfd(host, port);
     int inp;
     fgets(buf, MAXLINE, stdin);
     write(clientfd, filename, strlen(filename));
     do
     {
          blue();
          printf("\n\n----------------------------------------\n\n");
          reset();

          printf("\n");
          yellow();
          printf("1. Display All Stocks\n");
          reset();
          printf("\033[1;31m");
          printf("2. SELL\n");
          printf("\033[0m");
          printf("\033[0;32m");
          printf("3. BUY\n");
          printf("\033[0m");
          yellow();
          printf("4. My Portfolio\n");
          reset();
          yellow();
          printf("5. Exit\n");
          reset();
          printf("\n");
          blue();
          printf("\n\n----------------------------------------\n\n");
          reset();
          printf("Please enter your input: ");
          scanf("%d", &inp);
          fgets(buf, MAXLINE, stdin);
          blue();
          printf("\n\n----------------------------------------\n\n");
          reset();
          if (inp == 2 || inp == 3)
          {
               printf("Enter Company Name, Number of stocks you want to BUY/SELL: \n");
               fgets(buf, MAXLINE, stdin);
          }

          struct price access;
          struct portfolio fetch;
          struct prev_price p;

          FILE *fptr4, *fptr5, *fptr7;
          char final[20];
          int flag = 0;

          switch (inp)
          {
          case 1:
               fptr4 = fopen("Share_price.txt", "r");
               fread(&access, sizeof(struct price), 1, fptr4);
               fclose(fptr4);
               fptr7 = fopen("Prev_price.txt", "r");
               fread(&p, sizeof(struct prev_price), 1, fptr7);
               fclose(fptr7);
               fflush(stdout);

               bold();
               blue();
               printf("------------------------------------\n");
               reset();
               cyan();
               printf("\033[24m   Company               Price\n");
               boff();
               reset();
               blue();
               printf("------------------------------------\n");
               reset();
               for (int i = 0; i < 10; i++)
               {
                    // printf("%.7s %5.2f\n",access.com_name[i],access.price[i]);
                    yellow();
                    printf("-   %-15s|     ", access.com_name[i]);
                    reset();
                    if (p.price[i] < access.price[i])
                    {
                         green();
                         printf("%5.2f\n", access.price[i]);
                         reset();
                    }
                    else if (p.price[i] > access.price[i])
                    {
                         red();
                         printf("%5.2f\n", access.price[i]);
                         reset();
                    }
                    else
                    {
                         printf("%5.2f\n", access.price[i]);
                    }
               }

               blue();
               printf("------------------------------------\n");
               reset();
               break;

          case 2:
               strcpy(final, "SELL ");
               strcat(final, buf);
               printf("%s", final);
               break;

          case 3:
               strcpy(final, "BUY ");
               strcat(final, buf);
               printf("%s", final);
               break;

          case 4:
               fptr5 = fopen(filename, "r");
               fread(&fetch, sizeof(struct portfolio), 1, fptr5);
               fclose(fptr5);

               fptr4 = fopen("Share_price.txt", "r");
               fread(&access, sizeof(struct price), 1, fptr4);
               fclose(fptr4);

               bold();
               blue();
               printf("-------------------------------------------\n");
               reset();
               cyan();

               printf("\033[24m  Company         Quantity       Price\n");
               blue();
               printf("-------------------------------------------\n");
               reset();
               reset();
               boff();

               for (int i = 0; i < 10; i++)
               {
                    if (fetch.company[i] > 0)
                    {
                         yellow();
                         printf("%7s      | %8d     |", access.com_name[i], fetch.company[i]);
                         reset();
                         if (fetch.price[i] < access.price[i])
                         {
                              red();
                              printf("    %5.2f\n", fetch.price[i]);
                              reset();
                         }
                         else if (fetch.price[i] > access.price[i])
                         {
                              green();
                              printf("    %5.2f\n", fetch.price[i]);
                              reset();
                         }
                         else
                         {
                              printf("    %5.2f\n", fetch.price[i]);
                         }
                         flag = 1;
                    }
               }

               blue();
               printf("-------------------------------------------\n");
               reset();
               if (flag == 0)
               {
                    printf("You do not have any possession of Stocks!\n");
               }

               break;
          default:
               break;
          }

          // printf("\n");

          if (inp == 2 || inp == 3)
          {
               write(clientfd, final, strlen(final));
               if (final[0] == '\n')
                    break;
               fflush(stdout);
               n = read(clientfd, buf, MAXLINE);
               buf[n] = '\0';
               if (buf[n - 1] == 'd')
               {
                    printf("Server: ");
                    red();
                    printf("Transaction Declined!\n");
                    reset();
               }
               else
               {
                    printf("Server: ");
                    green();
                    printf("Transaction Successful!\n");
                    reset();
               }
               printf("Server: %s", buf);
          }

     } while (inp != 5);

     close(clientfd);
     exit(0);
}