// Q2.cpp - File and Directory Permission Checker (Formatted Output with Header)
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
#include <string> 

class FileValidator {
private:
    void printHeader() {
        std::cout << "\033[1;35m";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                 🔍 FILE VALIDATOR v2.0                       ║\n";
        std::cout << "║                Advanced Operating Systems                    ║\n";
        std::cout << "║                      Assignment 1 - Q2                       ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        std::cout << "\033[0m\n";
    }

    void printAlignedPermission(const std::string& text, bool hasPermission) {
        const int textWidth = 45;
        std::cout << text;

        for (int i = text.length(); i < textWidth; ++i) {
            std::cout << " ";
        }

        std::cout << ": " << (hasPermission ? "\033[1;32mYes\033[0m" : "\033[1;31mNo\033[0m") << "\n";
    }

    bool checkFileExists(const char* filepath) {
        struct stat st;
        return stat(filepath, &st) == 0;
    }

    bool isReversed(const char* originalFile, const char* reversedFile) {
        int fd1 = open(originalFile, O_RDONLY);
        int fd2 = open(reversedFile, O_RDONLY);

        if (fd1 == -1 || fd2 == -1) {
            if (fd1 != -1) close(fd1);
            if (fd2 != -1) close(fd2);
            return false;
        }

        struct stat st1, st2;
        fstat(fd1, &st1);
        fstat(fd2, &st2);

        if (st1.st_size != st2.st_size) {
            close(fd1);
            close(fd2);
            return false;
        }

        const int BUFFER_SIZE = 8192;
        char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
        long long fileSize = st1.st_size;

        for (long long pos = 0; pos < fileSize; pos += BUFFER_SIZE) {
            long long readSize = (pos + BUFFER_SIZE > fileSize) ? (fileSize - pos) : BUFFER_SIZE;
            lseek(fd1, pos, SEEK_SET);
            read(fd1, buffer1, readSize);
            lseek(fd2, fileSize - pos - readSize, SEEK_SET);
            read(fd2, buffer2, readSize);

            for (int i = 0; i < readSize / 2; i++) {
                char temp = buffer2[i];
                buffer2[i] = buffer2[readSize - 1 - i];
                buffer2[readSize - 1 - i] = temp;
            }

            if (memcmp(buffer1, buffer2, readSize) != 0) {
                close(fd1);
                close(fd2);
                return false;
            }
        }
        close(fd1);
        close(fd2);
        return true;
    }

    void printPermissions(const char* filepath, const char* type) {
        struct stat st;
        if (stat(filepath, &st) == -1) {
            std::cout << "Could not get permissions for " << type << "\n";
            return;
        }

        std::string typeStr(type);
        printAlignedPermission("User has read permissions on " + typeStr,    (st.st_mode & S_IRUSR));
        printAlignedPermission("User has write permission on " + typeStr,   (st.st_mode & S_IWUSR));
        printAlignedPermission("User has execute permission on " + typeStr, (st.st_mode & S_IXUSR));

        printAlignedPermission("Group has read permissions on " + typeStr,   (st.st_mode & S_IRGRP));
        printAlignedPermission("Group has write permission on " + typeStr,  (st.st_mode & S_IWGRP));
        printAlignedPermission("Group has execute permission on " + typeStr, (st.st_mode & S_IXGRP));

        printAlignedPermission("Others has read permissions on " + typeStr,  (st.st_mode & S_IROTH));
        printAlignedPermission("Others has write permission on " + typeStr, (st.st_mode & S_IWOTH));
        printAlignedPermission("Others has execute permission on " + typeStr,(st.st_mode & S_IXOTH));
    }

public:
    void validateFiles(const char* newFile, const char* oldFile, const char* directory) {
        printHeader(); 

        bool dirExists = checkFileExists(directory);
        std::cout << "Directory is created: "
                  << (dirExists ? "\033[1;32mYes\033[0m" : "\033[1;31mNo\033[0m") << "\n";

        bool filesReversed = isReversed(oldFile, newFile);
        std::cout << "Whether file contents are reversed in newfile: "
                  << (filesReversed ? "\033[1;32mYes\033[0m" : "\033[1;31mNo\033[0m") << "\n";

        struct stat stOld, stNew;
        bool sameSize = (stat(oldFile, &stOld) == 0 && stat(newFile, &stNew) == 0 &&
                        stOld.st_size == stNew.st_size);
        std::cout << "Both Files Sizes are Same: "
                  << (sameSize ? "\033[1;32mYes\033[0m" : "\033[1;31mNo\033[0m") << "\n";

        printPermissions(newFile, "newfile");
        printPermissions(oldFile, "oldfile");
        printPermissions(directory, "directory");
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <newfile_path> <oldfile_path> <directory_path>" << "\n";
        return 1;
    }

    FileValidator validator;
    validator.validateFiles(argv[1], argv[2], argv[3]);

    return 0;
}