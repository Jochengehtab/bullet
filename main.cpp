#include <iostream>
#include <fstream>
#include <string>

const size_t BUFFER_SIZE_MB = 128;

void appendBinaryFiles(const std::string &sourceFile, const std::string &targetFile, size_t bufferSize)
{
    // Open the source file in binary mode for reading
    std::ifstream source(sourceFile, std::ios::binary);
    if (!source)
    {
        std::cerr << "Error: Unable to open source file: " << sourceFile << std::endl;
        return;
    }

    // Open the target file in binary mode for appending
    std::ofstream target(targetFile, std::ios::binary | std::ios::app);
    if (!target)
    {
        std::cerr << "Error: Unable to open target file: " << targetFile << std::endl;
        return;
    }

    // Buffer for copying chunks of data
    char *buffer = new char[bufferSize];

    // Copy the source file to the target file
    while (source)
    {
        // Read a chunk of data
        source.read(buffer, bufferSize);
        // Write the read chunk to the target
        target.write(buffer, source.gcount());
    }

    // Check for errors
    if (!source.eof() && source.fail())
    {
        std::cerr << "Error occurred while reading source file." << std::endl;
    }
    if (target.fail())
    {
        std::cerr << "Error occurred while writing to target file." << std::endl;
    }

    std::cout << "File appended successfully!" << std::endl;

    // Clean up dynamically allocated memory for buffer
    delete[] buffer;

    // Close files
    source.close();
    target.close();
}

int main()
{
    std::string sourceFile;
    std::string targetFile;

    // User inputs for source and target files
    std::cout << "Enter the path of the source file: ";
    std::getline(std::cin, sourceFile);

    std::cout << "Enter the path of the target file: ";
    std::getline(std::cin, targetFile);

    // Ask for the buffer size in MB
    size_t bufferSizeMB;
    std::cout << "Enter the buffer size in MB (default is " << BUFFER_SIZE_MB << " MB): ";
    std::cin >> bufferSizeMB;
    std::cin.ignore(); // Ignore the newline character left by std::cin

    // Convert the buffer size from MB to bytes
    size_t bufferSize = bufferSizeMB * 1024 * 1024; 

    // Confirm the operation
    std::cout << "\nYou are about to append the contents of '" << sourceFile
              << "' to '" << targetFile << "' using a buffer size of " << bufferSizeMB
              << " MB (" << bufferSize << " bytes). Do you want to proceed? (y/n): ";

    char confirm;
    std::cin >> confirm;
    std::cin.ignore(); // Ignore the newline character left by std::cin

    // Proceed if the user confirms
    if (confirm == 'y' || confirm == 'Y')
    {
        appendBinaryFiles(sourceFile, targetFile, bufferSize);
    }
    else
    {
        std::cout << "Operation canceled." << std::endl;
    }

    return 0;
}