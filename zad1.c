#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 64

int main(int argc, char *argv[]){
    int fd_tab[2];
    int inputFile, r, i;
    pid_t pid;
    char buffer[BUFFER_SIZE];

    char* fileName = argv[1];   // Pobieramy nazwę pliku z argumentu

    if(fileName == NULL){       // Sprawdzamy, czy podano argument, jeśli nie, to wyświetlamy komunikat i zamykamy program
        fprintf(stderr, "Nie podano nazwy pliku.\n");
        return 1;
    }

    if((inputFile = open(fileName, O_RDONLY)) < 0){    // Próbujemy otworzyć plik i jeśli się nie uda wyświetlamy komunikat i zamykamy program
        fprintf(stderr, "Błąd przy próbie otwarcia pliku.\n");
        return 2;
    }

    if(pipe(fd_tab) < 0){       // Tworzymy potok i jeśli wystąpił błąd to wyświetlamy komunikat i zamykamy program
        fprintf(stderr, "Błąd przy tworzeniu potoku.\n");
        return 3;
    }
    
    if(pid = fork() == 0){      // Kod dla procesu potomnego
        close(fd_tab[1]);       // Zamykamy zapisywanie do potoku

        while((read(fd_tab[0], &buffer, BUFFER_SIZE)) > 0){   
            write(STDOUT_FILENO, "#", 1);               // i wypisujemy dane wraz ze wstawionymi "#" 
            write(STDOUT_FILENO, &buffer, BUFFER_SIZE);
            write(STDOUT_FILENO, "#\n", 2);
            for(i = 0; i < BUFFER_SIZE; i++){           // czyścimy bufor, aby w przypadku danych nie zapełniających całego jego rozmiaru,
                buffer[i] = ' ';                        // nie dostały się do niego dane z poprzednich odczytów
            }
        }

        close(fd_tab[0]);
    }
    else{                       // Kod dla rodzica 
        close(fd_tab[0]);       // Zamykamy odczytywanie z potoku

        printf("read attempt\n");
        while ((r = read(inputFile, &buffer, BUFFER_SIZE)) > 0){ // dopóki nie napotkamy błędu lub EOF
            if(write(fd_tab[1], &buffer, r) < 0){       // próbujemy zapisać dane do potoku
                fprintf(stderr, "Błąd przy próbie zapisu do potoku.\n");    // i jeśli wystąpi błąd to wyświetlamy komuniat i zamykamy program
                return 5;
            }
        }
        close(inputFile);       // zamykamy deskryptor pliku
    }
    return 0;
}