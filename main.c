//da gestire le renne di babbo natale con
//struct, pipe e allocazione dinamica

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef struct
{
    char nome[50];
    int velocita;   //km/h
    int resistenza; // 1-10

} Renna;

int main(void)
{
    //processo figlio = elfo responsabile delle renne

    int fd[2]; //0 = lettura, 1 = scrittura
    pid_t ritorno_fork;

    if (pipe(fd) == -1)
    {
        perror("Errore nella creazione della pipe");
        exit(-1);
    }

    ritorno_fork = fork();

    //figlio
    if (ritorno_fork == 0)
    {
        close(fd[0]); //chiuso lettura perché deve mandare la info al padre

        //inizializzazione di numeri casuali
        srand(time(NULL));

        //int n = rand() % 10;
        //n avrà valore tra 0 e 9

        int n = rand() % 8; //tra 0 e 7
        n += 5; //tra 5 e 12

        n +=1; //faccio +1 per aggiungere il numero
        write(fd[1], &n, sizeof(int));
        //mando il numero cosi legge quante renne ci sono
        n -= 1; //faccio -1 per usarlo per la creazione delle renne

        Renna renna;
        renna.nome[0] = 'R';
        renna.nome[1] = 'e';
        renna.nome[2] = 'n';
        renna.nome[3] = 'n';
        renna.nome[4] = 'a';
        renna.nome[5] = '_';

        for (int i = 1; i <= n; i ++)
        {
            if (i < 10)
            {
                renna.nome[6] = (char)(i + '0'); //per farlo in ascii
                renna.nome[7] = '\0';
            }
            else
            {
                renna.nome[6] = '1';
                renna.nome[7] = (char)(i % 10 + '0');
                renna.nome[8] = '\0';
            }
            renna.velocita = rand() % 50 + 51; // tra 50 e 100 km/h
            renna.resistenza = rand() % 10 + 1;

            //uso il canale 1 per scrivere
            write(fd[1], &renna, sizeof(Renna));
        }
        //renna sentinella
        renna.nome[6] = 'S';
        renna.nome[7] = '\0';
        renna.velocita = -1;

        write(fd[1], &renna, sizeof(Renna));

        close(fd[1]);
    }
    //padre
    else
    {
        close(fd[1]); //chiudo scrittura

        char *rennaGuida;
        int nRenne, maxVelocita = 0;
        read(fd[0], &nRenne, sizeof(int));

        Renna *registroRenne = malloc(sizeof(Renna) * nRenne);

        for (int i = 0; i < nRenne; i ++)
        {
            read(fd[0], &registroRenne[i], sizeof (Renna));
            if (registroRenne[i].velocita > maxVelocita)
            {
                maxVelocita = registroRenne[i].velocita;
                rennaGuida = registroRenne[i].nome;
            }
            printf("\n%s con una velocità di %d km/h e una resistenza di valore %d",
            registroRenne[i].nome, registroRenne[i].velocita, registroRenne[i].resistenza);
        }

        printf("\n\n%s guiderà la slitta con velocità %d km/h!", rennaGuida, maxVelocita);

        close(fd[0]);
        wait(NULL);
    }

    return 0;
}