#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"


class Advisorbot
{
public:
    Advisorbot();
    /** Call this to start the sim */
    void init();
private:
    void printMenu();
    void printHelp();
    void printMarketStats();
    void enterAsk();
    void enterBid();
    void printWallet();
    void gotoNextTimeframe();
    std::string getUserOption();
    void processUserInput(std::string userInput);
    void startingMessage();
    void printProducts();
    std::string provideHelp(std::string userHelpInput);

    std::vector<OrderBookEntry> getEntries(std::string type, std::string product, std::string currentTime);
    
    std::string printMaxOrMin(std::string command, std::string product, std::string type);
    bool isValidProduct(std::string UerEnteredProduct);
    std::string printAvg(std::string product, std::string type, int timesteps);

    std::string predictVal(std::string command, std::string product, std::string type);

    double getAvg(std::vector<OrderBookEntry> &entries);
    double getMode(std::vector<OrderBookEntry> &entries);
    double getMedian(std::vector<OrderBookEntry> &entries);
    double getVaraince(std::vector<OrderBookEntry>& entries, double mean);
    double getMaxOrMin(std::string command, std::vector<OrderBookEntry>& entries);
    double getSknewness(double& mean, double& median, double& standardDeviation);

    std::string superStats(std::string product, std::string type);

    bool isRunning = true;

    std::string currentTime;
    //store old time frames
    std::vector<std::string> timeframes;

    //3000 lines
    OrderBook orderBook{ "20200317.csv" };

    //1m lines
    //OrderBook orderBook{"20200600.csv"};

    Wallet wallet;

};

