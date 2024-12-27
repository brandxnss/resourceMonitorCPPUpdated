#include <iostream>
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <thread>
#include <chrono>
#pragma comment(lib,"pdh.lib")

#define PDH_OK                      ((PDH_STATUS)0x00000000L)

int main() {
    // Initialize the PDH query handle and the counter handle
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    PDH_STATUS status;

    // Open a query handle
    status = PdhOpenQuery(NULL, 0, &query);
    if (status != PDH_OK) {
        std::cerr << "Failed to open PDH query. Error code: " << status << std::endl;
        return 1;
    }

    // Add the counter for total memory time (all sticks)
    status = PdhAddCounter(query, L"\\Memory\\% Committed Bytes In Use", 0, &counter);
    if (status != PDH_OK) {
        std::cerr << "Failed to add counter. Error code: " << status << std::endl;
        PdhCloseQuery(query);
        return 1;
    }

    // Start monitoring Memory usage in a loop
    while (true) {
        // Collect the data
        status = PdhCollectQueryData(query);
        if (status != PDH_OK) {
            std::cerr << "Failed to collect query data. Error code: " << status << std::endl;
            break;
        }

        // Retrieve the counter value
        PDH_FMT_COUNTERVALUE counterValue;
        status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterValue);
        if (status != PDH_OK) {
            std::cerr << "Failed to get formatted counter value. Error code: " << status << std::endl;
            break;
        }

        // Print the Memory usage percentage
        std::cout << "Memory Usage: " << counterValue.doubleValue << "%" << std::endl;

        // Wait for 3 seconds before updating the usage again
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Close the query handle
    PdhCloseQuery(query);
    return 0;
}