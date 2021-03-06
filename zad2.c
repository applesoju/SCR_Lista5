#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 64

int main(int argc, char *argv[]){
    int fd_tab[2];
    int inputFile, r, i;
    pid_t pid;
    char buffer[BUFFER_SIZE];
    char fileName[50];

    struct timespec time1, time2;
    
    time1.tv_nsec = 0;
    time1.tv_sec = 15;

    time2.tv_nsec = 5000000;
    time2.tv_sec = 0;

    if(pipe(fd_tab) < 0){       // Tworzymy potok i jeśli wystąpił błąd to wyświetlamy komunikat i zamykamy program
        fprintf(stderr, "Błąd przy tworzeniu potoku.\n");
        return 3;
    }
    
    if(pid = fork() == 0){      // Kod dla procesu potomnego

        printf("[Czekanie na zamknięcie potoku rodzica.]\n");

        nanosleep(&time1, &time2);

        close(fd_tab[1]);       // Zamykamy zapisywanie do potoku

        printf("[Potok rodzica został zamknięty.]\n");
        
        close(STDIN_FILENO);    // Zamykamy stdin
        dup(fd_tab[0]);         // Na miejsce stdin kopiujemy odczyt z potoku

        close(fd_tab[0]);       // Zamykamy odczyt z potoku

        execlp("display", "-", NULL);
    }
    else{                       // Kod dla rodzica 
        close(fd_tab[0]);       // Zamykamy odczytywanie z potoku

        printf("Proszę o podanie nazwy pliku obrazu:\n");
        scanf("%s", fileName);

        if((inputFile = open(fileName, O_RDONLY)) < 0){    // Próbujemy otworzyć plik i jeśli się nie uda wyświetlamy komunikat i zamykamy program
            fprintf(stderr, "Błąd przy próbie otwarcia pliku.\n");
            return 2;
        }   

        while ((r = read(inputFile, &buffer, BUFFER_SIZE)) > 0){            // dopóki nie napotkamy błędu lub EOF
            if(write(fd_tab[1], &buffer, r) < 0){                           // próbujemy zapisać dane do potoku
                fprintf(stderr, "Błąd przy próbie zapisu do potoku.\n");    // i jeśli wystąpi błąd to wyświetlamy komuniat i zamykamy program
                return 5;
            }
        }
        close(inputFile);       // zamykamy deskryptor pliku
    }
    return 0;
}