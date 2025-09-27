#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define LOG_TAG "caca"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void generateRandomSuffix(char* buffer, size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < length; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        buffer[i] = charset[key];
    }
    buffer[length] = '\0'; // Null-terminate the string
}

void createApagnan(const char* message) {
    const char* filePath = "/data/data/dji.go.v5/files/quoicoubeh.txt";

    FILE* file = fopen(filePath, "a");
    if (file == NULL) {
        LOGI("Failed to create file: %s", filePath);
        return;
    }
    size_t bytesWritten = fwrite(message, sizeof(char), strlen(message), file);
    if (bytesWritten < strlen(message)) {
        LOGI("Failed to write to file: %s", filePath);
    } else {
        LOGI("File created successfully: %s", filePath);
    }

    fclose(file);
}

void createFile(const char* filePath, const char* content, size_t size) {
    FILE* file = fopen(filePath, "a");
    if (file == NULL) {
        LOGI("Failed to create file: %s", filePath);
        return;
    }

    size_t bytesWritten = fwrite(content, sizeof(char), size, file);
    if (bytesWritten < size) {
        const char* msg = "Failed to write to file";
        createApagnan(msg);
    } else {
        LOGI("File created successfully: %s", filePath);
    }

    fclose(file);
}

void parseDexFiles() {
    const char* mapsFilePath = "/proc/self/maps";
    const char* outputDir = "/data/data/dji.go.v5/files/dumps/";
    FILE* mapsFile = fopen(mapsFilePath, "r");
    if (mapsFile == NULL) {
        createApagnan("Failed to open maps file.\n");
        return;
    }

    char line[256];
    int dexFileCount = 0; // Counter for file suffix
    createApagnan("Searching for DEX memory regions!\n");
    bool dumpMemory = false; // Flag to control dumping
    while (fgets(line, sizeof(line), mapsFile)) {
        // Log the line read from maps for debugging
        createApagnan(line);
        
        // Check if the line contains "libAppGuard"
        if (strstr(line, "app_process64")) {
            dumpMemory = true; // Start dumping memory
            continue; // Move to the next line
        }

        // if (strstr(line, "libFRCorkscrew")) {
        //     dumpMemory = false; // Start dumping memory
        //     continue; // Move to the next line
        // }

        // Check if we are in dumping mode and the line does not contain "/"
        if (dumpMemory && strstr(line, "0 r")) {
          
            unsigned long startAddr, endAddr;
            sscanf(line, "%lx-%lx", &startAddr, &endAddr);
            
            // Calculate the size of the memory region
            size_t size = endAddr - startAddr;
            createApagnan("Dumping memory region...\n");
            
            int memFd = open("/proc/self/mem", O_RDONLY);
            if (memFd < 0) {
                createApagnan("Failed to open /proc/self/mem.\n");
                continue;
            }

            if (lseek(memFd, startAddr, SEEK_SET) == -1) {
                createApagnan("Failed to seek to memory address.\n");
                close(memFd);
                continue;
            }

            // Allocate buffer to hold the memory content
            char* buffer = (char*)malloc(size);
            if (buffer) {
                ssize_t bytesRead = read(memFd, buffer, size);
                if (bytesRead < 0) {
                    createApagnan("Failed to read memory region.\n");
                } else {
                    // Create a unique filename for the memory dump with an incrementing integer suffix
                    char outputFilePath[512];
                    // snprintf(outputFilePath, sizeof(outputFilePath), "%s%d.dump", outputDir, dexFileCount);       
                    snprintf(outputFilePath, sizeof(outputFilePath), "%s%lx.dump", outputDir, startAddr);
                    // Write the memory content to the file
                    createFile(outputFilePath, buffer, bytesRead);
                    createApagnan("Memory region dumped successfully.\n");
                }
                free(buffer);
            } else {
                createApagnan("Failed to allocate memory for dump content\n");
            }

            close(memFd);
        }
    }

    fclose(mapsFile);
}

// Thread function to run in the background
void* backgroundTask(void* arg) {
    // while (true) {
    const char* msg = "apagnan\n";
    createApagnan(msg);
    sleep(5);
    parseDexFiles();
    // }
    return NULL;
}

// Constructor function that will be called when the library is loaded
__attribute__((constructor)) void initLibrary() {
    pthread_t threadId;
    if (pthread_create(&threadId, NULL, backgroundTask, NULL) != 0) {
        LOGI("Failed to create background thread");
    }
}
