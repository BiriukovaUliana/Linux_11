#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

void sigbus_handler(int sig, siginfo_t *si, void *unused) {
    printf("\n--- Отримано сигнал SIGBUS ---\n");
    printf("Адреса помилки: %p\n", si->si_addr);

    switch (si->si_code) {
        case BUS_ADRERR:
            printf("Причина: BUS_ADRERR (Помилка mmap/спроба доступу за межі файлу)\n");
            break;
        case BUS_OBJERR:
            printf("Причина: BUS_OBJERR (Апаратна помилка або збій об'єкта пам'яті)\n");
            break;
        default:
            printf("Причина: Код %d (Інша помилка шини)\n", si->si_code);
            break;
    }
    
    exit(EXIT_FAILURE);
}

int main() {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigbus_handler;

    if (sigaction(SIGBUS, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Обробник SIGBUS встановлено. Симулюємо помилку...\n");

    int fd = open("test_bus", O_RDWR | O_CREAT | O_TRUNC, 0666);
    write(fd, "test", 4); 
    
    char *map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    ftruncate(fd, 0);
    
    printf("Спроба доступу до обрізаного файлу через mmap...\n");
    char c = map[0]; 

    return 0;
}
