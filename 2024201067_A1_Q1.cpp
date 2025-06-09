// Q1.cpp - File Reverser with Progress Display (Corrected Version)
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

class FileReverser {
private:
    static const int BUFFER_SIZE = 8192; // 8KB buffer for efficiency
    static const int PROGRESS_BAR_WIDTH = 50;

    void printProgress(long long current, long long total) {
        if (total == 0) return;

        int percentage = (int)((current * 100) / total);
        int filled = (percentage * PROGRESS_BAR_WIDTH) / 100;

        std::cout << "\r\033[K";

        // Display progress bar
        std::cout << "\033[36m╭─── File Processing ───╮\033[0m\n";
        // std::cout << "\033[36m│ \033[0m";

        for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
            if (i < filled) {
                if (i < PROGRESS_BAR_WIDTH / 3)
                    std::cout << "\033[42m \033[0m";
                else if (i < 2 * PROGRESS_BAR_WIDTH / 3)
                    std::cout << "\033[43m \033[0m";
                else
                    std::cout << "\033[41m \033[0m";
            } else {
                std::cout << "\033[47m \033[0m";
            }
        }

        std::cout << "\033[36m │\033[0m\n";
        std::cout << "\033[36m│ \033[1m" << percentage << "% Complete\033[0m";
        std::cout << " \033[90m(" << current << "/" << total << " bytes)\033[0m";

        // Padding to align the progress bar text
        int padding = PROGRESS_BAR_WIDTH - 15;
        for (int i = 0; i < padding; i++) std::cout << " ";
        std::cout << "\n";
        std::cout << "\033[36m╰───────────────────────╯\033[0m";

        std::cout.flush();

        if (percentage < 100) {
            std::cout << "\033[3A";
        }
    }

    void createDirectory(const char* dirPath) {
        struct stat st;
        if (stat(dirPath, &st) == -1) {
            // Create directory with rwx permissions for user only, as required.
            if (mkdir(dirPath, 0700) == -1) {
                perror("Error creating directory");
                _exit(1);
            }
        }
    }

    void reverseBuffer(char* buffer, int size) {
        for (int i = 0; i < size / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[size - 1 - i];
            buffer[size - 1 - i] = temp;
        }
    }

    long long getFileSize(int fd) {
        struct stat st;
        if (fstat(fd, &st) == -1) {
            perror("Error getting file size");
            return -1;
        }
        return st.st_size;
    }

public:
    void reverseEntireFile(const char* inputFile, const char* outputFile) {
        std::cout << "\033[1;34m🔄 Starting Full File Reversal...\033[0m\n\n";

        int inputFd = open(inputFile, O_RDONLY);
        if (inputFd == -1) {
            perror("Error opening input file");
            _exit(1);
        }

        createDirectory("Assignment1");

        // Create output file with rw permissions for user only, as required.
        int outputFd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (outputFd == -1) {
            perror("Error creating output file");
            close(inputFd);
            _exit(1);
        }

        long long fileSize = getFileSize(inputFd);
        long long totalProcessed = 0;
        char buffer[BUFFER_SIZE];

        lseek(inputFd, 0, SEEK_END);

        while (totalProcessed < fileSize) {
            long long currentPos = fileSize - totalProcessed;
            long long readSize = (currentPos >= BUFFER_SIZE) ? BUFFER_SIZE : currentPos;

            lseek(inputFd, currentPos - readSize, SEEK_SET);

            ssize_t bytesRead = read(inputFd, buffer, readSize);
            if (bytesRead <= 0) break;

            reverseBuffer(buffer, bytesRead);

            ssize_t bytesWritten = write(outputFd, buffer, bytesRead);
            if (bytesWritten != bytesRead) {
                perror("Error writing to output file");
                break;
            }

            totalProcessed += bytesRead;
            printProgress(totalProcessed, fileSize);
        }

        close(inputFd);
        close(outputFd);

        std::cout << "\n\n\033[1;32m✅ File reversal completed successfully!\033[0m\n";
        std::cout << "\033[36m📁 Output saved to: \033[1m" << outputFile << "\033[0m\n\n";
    }

    void reversePartialFile(const char* inputFile, const char* outputFile,
                           long long startIdx, long long endIdx) {
        std::cout << "\033[1;34m🔄 Starting Partial File Reversal...\033[0m\n";
        std::cout << "\033[33m📍 Reversing sections: [0-" << startIdx << "] and ["
                  << endIdx << "-EOF]\033[0m\n\n";

        int inputFd = open(inputFile, O_RDONLY);
        if (inputFd == -1) {
            perror("Error opening input file");
            _exit(1);
        }

        createDirectory("Assignment1");

        // Create output file with rw permissions for user only, as required.
        int outputFd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (outputFd == -1) {
            perror("Error creating output file");
            close(inputFd);
            _exit(1);
        }

        long long fileSize = getFileSize(inputFd);
        long long totalToProcess = startIdx + (fileSize - endIdx);
        long long totalProcessed = 0;
        char buffer[BUFFER_SIZE];

        // Process first part (0 to startIdx) - reversed
        long long pos = startIdx;
        while (pos > 0 && totalProcessed < totalToProcess) {
            long long readSize = (pos >= BUFFER_SIZE) ? BUFFER_SIZE : pos;
            lseek(inputFd, pos - readSize, SEEK_SET);

            ssize_t bytesRead = read(inputFd, buffer, readSize);
            if (bytesRead <= 0) break;

            reverseBuffer(buffer, bytesRead);
            write(outputFd, buffer, bytesRead);

            pos -= bytesRead;
            totalProcessed += bytesRead;
            printProgress(totalProcessed, totalToProcess);
        }

        // Copy middle part (startIdx to endIdx) - unchanged
        lseek(inputFd, startIdx, SEEK_SET);
        long long middleSize = endIdx - startIdx;
        while (middleSize > 0) {
            long long readSize = (middleSize >= BUFFER_SIZE) ? BUFFER_SIZE : middleSize;
            ssize_t bytesRead = read(inputFd, buffer, readSize);
            if (bytesRead <= 0) break;

            write(outputFd, buffer, bytesRead);
            middleSize -= bytesRead;
        }

        // Process last part (endIdx to EOF) - reversed
        pos = fileSize;
        while (pos > endIdx && totalProcessed < totalToProcess) {
            long long readSize = ((pos - endIdx) >= BUFFER_SIZE) ? BUFFER_SIZE : (pos - endIdx);
            lseek(inputFd, pos - readSize, SEEK_SET);

            ssize_t bytesRead = read(inputFd, buffer, readSize);
            if (bytesRead <= 0) break;

            reverseBuffer(buffer, bytesRead);
            write(outputFd, buffer, bytesRead);

            pos -= bytesRead;
            totalProcessed += bytesRead;
            printProgress(totalProcessed, totalToProcess);
        }

        close(inputFd);
        close(outputFd);

        std::cout << "\n\n\033[1;32m✅ Partial file reversal completed successfully!\033[0m\n";
        std::cout << "\033[36m📁 Output saved to: \033[1m" << outputFile << "\033[0m\n\n";
    }
};

int main(int argc, char* argv[]) {
    std::cout << "\033[1;36m";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    🚀 FILE REVERSER v2.0                     ║\n";
    std::cout << "║                Advanced Operating Systems                    ║\n";
    std::cout << "║                      Assignment 1 - Q1                       ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\033[0m\n";

    if (argc < 3) {
        std::cout << "\033[1;31m❌ Usage: " << argv[0] << " <input_file> <flag> [start_index] [end_index]\033[0m\n";
        std::cout << "\033[33m💡 Flag 0: Reverse entire file\033[0m\n";
        std::cout << "\033[33m💡 Flag 1: Reverse partial file (requires start and end indices)\033[0m\n";
        return 1;
    }

    const char* inputFile = argv[1];
    int flag = atoi(argv[2]);

    const char* filename = strrchr(inputFile, '/');
    filename = filename ? filename + 1 : inputFile;

    char outputFile[256];
    snprintf(outputFile, sizeof(outputFile), "Assignment1/%d_%s", flag, filename);

    FileReverser reverser;

    if (flag == 0) {
        reverser.reverseEntireFile(inputFile, outputFile);
    } else if (flag == 1) {
        if (argc != 5) {
            std::cout << "\033[1;31m❌ Flag 1 requires start and end indices\033[0m\n";
            return 1;
        }
        long long startIdx = atoll(argv[3]);
        long long endIdx = atoll(argv[4]);
        reverser.reversePartialFile(inputFile, outputFile, startIdx, endIdx);
    } else {
        std::cout << "\033[1;31m❌ Invalid flag. Use 0 or 1.\033[0m\n";
        return 1;
    }

    return 0;
}