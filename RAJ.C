#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>  // For fmod

// Constants for expiration
#define EXPIRATION_YEAR 2025
#define EXPIRATION_MONTH 3
#define EXPIRATION_DAY 20
#define DEFAULT_PACKET_SIZE 999  // Default packet size (1 KB)
#define DEFAULT_THREAD_COUNT 1200  // Default number of threads

// Global variables to track progress
unsigned long total_packets_sent = 0;
unsigned long total_bytes_sent = 0;
int keep_running = 1;
char *global_payload = NULL;

// Structure to store attack parameters
typedef struct {
    const char *target_ip;        // Changed to const char * for target IP address (IPv4)
    int target_port;              // Target port
    int duration;                 // Duration for the attack
    int packet_size;              // Size of packets to send
    int thread_id;                // Thread ID for identification
} attack_params;

// Function to handle SIGINT (Ctrl+C)
void handle_signal(int sig) {
    keep_running = 0;
    printf("\n\033[1;31mAttack interrupted by user.\033[0m\n");
}

// Function to initialize attack parameters
void init_attack_params(attack_params *params, const char *ip, int port, int duration, int packet_size, int thread_id) {
    params->target_ip = (char *)ip;  // Cast to (char *) to avoid warnings
    params->target_port = port;
    params->duration = duration;
    params->packet_size = packet_size;
    params->thread_id = thread_id;
}

// Function to generate a random payload
void generate_random_payload(char *payload, int size) {
    for (int i = 0; i < size; i++) {
        payload[i] = rand() % 256;  // Random byte (0-255)
    }
}

// Example UDP flood function (simulating attack)
void *udp_flood(void *arg) {
    attack_params *params = (attack_params *)arg;
    unsigned long local_packets_sent = 0;
    unsigned long local_bytes_sent = 0;

    // Simulate sending UDP packets
    while (keep_running && local_packets_sent < 1000000) {
        // Simulate packet sending
        local_packets_sent++;
        local_bytes_sent += params->packet_size;
        usleep(100);  // Simulate a small delay for each packet
    }

    total_packets_sent += local_packets_sent;
    total_bytes_sent += local_bytes_sent;
    return NULL;
}

// Function to display progress in a single line
void display_progress(time_t start_time, int duration) {
    time_t now = time(NULL);
    int elapsed = (int)difftime(now, start_time);
    int remaining = duration - elapsed;

    if (remaining < 0) remaining = 0;

    // Clear the current line and move the cursor to the beginning
    printf("\033[2K\r");

    // Print everything in one line
    printf("\033[1;36mTime Remaining: %02d:%02d | \033[1;34mPackets Sent: %lu | \033[1;35mData Sent: %.2f MB\033[0m",
           remaining / 60, remaining % 60, total_packets_sent, (double)total_bytes_sent / (1024 * 1024));

    fflush(stdout); // Flush the output to update the line
}

// Stylish printout function with real-time expiry
void print_stylish_text() {
    // Get the current time
    time_t now;
    time(&now);

    // Calculate expiry time
    struct tm expiry_date = {0};
    expiry_date.tm_year = EXPIRATION_YEAR - 1900; // Year since 1900
    expiry_date.tm_mon = EXPIRATION_MONTH - 1;    // Month (0-11)
    expiry_date.tm_mday = EXPIRATION_DAY;         // Day of the month
    time_t expiry_time = mktime(&expiry_date);

    // Calculate remaining time
    double remaining_seconds = difftime(expiry_time, now);
    int remaining_days = (int)(remaining_seconds / (60 * 60 * 24));
    int remaining_hours = (int)fmod((remaining_seconds / (60 * 60)), 24); // Use fmod for double
    int remaining_minutes = (int)fmod((remaining_seconds / 60), 60);      // Use fmod for double
    int remaining_seconds_int = (int)fmod(remaining_seconds, 60);         // Use fmod for double

    // Print the stylish text with real-time expiry
    printf("\n");
    printf("\033[1;35m╔════════════════════════════════════════╗\033[0m\n");
    printf("\033[1;35m║ \033[1;36m★ \033[1;32mR A J O W N E R \033[1;36m★ \033[1;35m║\033[0m\n");
    printf("\033[1;35m╠════════════════════════════════════════╣\033[0m\n");
    printf("\033[1;35m║ \033[1;33m✦ DEVELOPED BY: \033[1;32m@RAJOWNER90 \033[1;35m║\033[0m\n");
    printf("\033[1;35m║ \033[1;33m✦ EXPIRY TIME: \033[1;32m%d days, %02d:%02d:%02d \033[1;35m║\033[0m\n", 
           remaining_days, remaining_hours, remaining_minutes, remaining_seconds_int);
    printf("\033[1;35m║ \033[1;33m✦ AAGYA TERA BAAP BY: \033[1;32mRAJOWNER \033[1;35m║\033[0m\n");
    printf("\033[1;35m║ \033[1;33m✦ YE FULL FREE DDOS BANARY HAI \033[1;35m║\033[0m\n");
    printf("\033[1;35m║ \033[1;31m✦ ! [ SELL KRNE WALO BAHEN CHUDALO ] \033[1;35m║\033[0m\n");
    printf("\033[1;35m║ \033[1;33m✦ YE FILE EXPIRY KE BAAD NEW FILE PAID MILEGA \033[1;35m║\033[0m\n");
    printf("\033[1;35m║ \033[1;33m✦ CONTACT: \033[1;32m@RAJOWNER90 \033[1;35m║\033[0m\n");
    printf("\033[1;35m╠════════════════════════════════════════╣\033[0m\n");
    printf("\033[1;35m║ \033[1;36m★ \033[1;32mP O W E R F U L \033[1;36m★ \033[1;35m║\033[0m\n");
    printf("\033[1;35m╚════════════════════════════════════════╝\033[0m\n\n");
}

// Main function
int main(int argc, char *argv[]) {
    // Print stylish text
    print_stylish_text();

    // Get the current time
    time_t now;
    time(&now);

    struct tm *local = localtime(&now);
    if (local->tm_year + 1900 > EXPIRATION_YEAR ||
        (local->tm_year + 1900 == EXPIRATION_YEAR && local->tm_mon + 1 > EXPIRATION_MONTH) ||
        (local->tm_year + 1900 == EXPIRATION_YEAR && local->tm_mon + 1 == EXPIRATION_MONTH && local->tm_mday > EXPIRATION_DAY)) {
        printf("\033[1;31mExpired. khatam ho gya h developer se connect kr @RAJOWNER90.\033[0m\n");
        return EXIT_FAILURE;
    }

    // Check for minimum arguments (IP and PORT are mandatory)
    if (argc < 3) {
        printf("\033[1;33mExample: %s 192.168.1.1 80 60 \033[0m\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int duration = (argc > 3) ? atoi(argv[3]) : 60; // Default duration: 60 seconds
    int packet_size = (argc > 4) ? atoi(argv[4]) : DEFAULT_PACKET_SIZE; // Default packet size
    int thread_count = (argc > 5) ? atoi(argv[5]) : DEFAULT_THREAD_COUNT; // Default thread count

    if (packet_size <= 0 || thread_count <= 0) {
        printf("\033[1;31mInvalid packet size or thread count. Using defaults.\033[0m\n");
        packet_size = DEFAULT_PACKET_SIZE;
        thread_count = DEFAULT_THREAD_COUNT;
    }

    signal(SIGINT, handle_signal);

    // Allocate and pre-generate the shared payload
    global_payload = (char *)malloc(packet_size);
    if (!global_payload) {
        perror("\033[1;31mFailed to allocate memory for payload\033[0m");
        return EXIT_FAILURE;
    }
    generate_random_payload(global_payload, packet_size);

    pthread_t threads[thread_count];
    attack_params params[thread_count];

    time_t start_time = time(NULL);

    for (int i = 0; i < thread_count; i++) {
        init_attack_params(&params[i], target_ip, target_port, duration, packet_size, i);
        pthread_create(&threads[i], NULL, udp_flood, &params[i]);
    }

    // Display progress while threads are running
    while (keep_running && time(NULL) < start_time + duration) {
        display_progress(start_time, duration);
        usleep(100000); // Update every 100ms (0.1 seconds)
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n\033[1;32mAttack completed.\033[0m \033[1;34mTotal packets sent:\033[0m \033[1;35m%lu\033[0m\n", total_packets_sent);
    printf("\033[1;34mTotal data sent:\033[0m \033[1;35m%.2f MB\033[0m\n", (double)total_bytes_sent / (1024 * 1024));
    free(global_payload);

    return EXIT_SUCCESS;

} 

       gcc -o attack_program raj.c -pthread