#include "Advisorbot.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <algorithm>
#include <stdexcept>

Advisorbot::Advisorbot()
{

}

void Advisorbot::init()
{
    std::string input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    startingMessage();

    //user has seen the first timeframe
    timeframes.push_back(currentTime);

    while (isRunning)
    {
        printMenu();
        input = getUserOption();
        processUserInput(input);
    }
}


void Advisorbot::printMenu()
{

}

void Advisorbot::printHelp()
{
    std::cout << "The available commands are help, time, next, market, prod, wallet, ask, bid, max, min, stats, predict, avg and exit\ntype help <cmd> to learn more about a command" << std::endl;
}

void Advisorbot::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask,
            p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;



    }
}

void Advisorbot::enterAsk()
{
    std::cout << "advisorbot> Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "advisorbot> enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::ask
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "advisorbot> Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "advisorbot> Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "advisorbot> enterAsk Bad input " << std::endl;
        }
    }
}

void Advisorbot::enterBid()
{
    std::cout << "advisorbot> Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "advisorbot> enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::bid
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "advisorbot> Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "advisorbot> Wallet has insufficient funds . " << std::endl;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "advisorbot> enterBid Bad input " << std::endl;
        }
    }
}

void Advisorbot::printWallet()
{
    std::cout << "advisorbot> " << wallet.toString() << std::endl;
}

void Advisorbot::gotoNextTimeframe()
{
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl;
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }

    }
    //store currentTime in timeframes before advancing to next timeframe
    timeframes.push_back(currentTime);
    currentTime = orderBook.getNextTime(currentTime);
}


void Advisorbot::startingMessage()
{
    std::cout << "\nadvisorbot> Please enter a command, or help for a list of commands" << std::endl;
}


std::string Advisorbot::getUserOption()
{
    std::string userOption;
    std::string line;
    std::cout << "user> ";
    std::getline(std::cin, line);
  
    userOption = line;
    return userOption;
}

void Advisorbot::printProducts()
{
    std::cout << "advisorbot> ";
    std::vector<std::string> products =  orderBook.getKnownProducts();
    for (auto& product : products) {
        std::cout << product << ' ';
    }
    std::cout << std::endl;
}

std::string Advisorbot::provideHelp(std::string userHelpInput)
{
    if (userHelpInput == "time")
    {
        return "time -> shows you the current time";
    
    }
    else if (userHelpInput == "step")
    {
        return "step-> goes to next timeframe";
    }
    else if (userHelpInput == "market")
    {
        return "market-> shows you the present bids and asks for each product type in the market";
    }
    else if (userHelpInput == "prod")
    {
        return "prod-> lists all available products";
    }
    else if (userHelpInput == "wallet")
    {
        return "wallet-> shows you your wallet balance";
    }
    else if (userHelpInput == "ask")
    {
        return "ask -> enter an ask to be processed";
    }
    else if(userHelpInput == "bid")
    {
        return "bid -> enter a bid to be processed";
    }
    else if (userHelpInput == "max")
    {
        return "max -> find maximum bid or ask for product in current time step\ninput in the form -> max ETH/BTC ask";
    }
    else if (userHelpInput == "min")
    {
        return "min -> find minimum bid or ask for product in current time step\ninput in the form -> min ETH/BTC ask";
    }
    else if (userHelpInput == "stats")
    {
        return "stats -> shows you stats of the product type inputed\ninput in the form -> stats ETH/USDT ask";
    }
    else if (userHelpInput == "predict")
    {
        return "stats -> shows you price prediction based on past 10 timesteps\ninput in the form -> predict max ETH/BTC bid";
    }
    else if (userHelpInput == "avg")
    {
        return "avg -> shows you average price of product in specified previous number of timesteps\ninput in the form -> avg ETH/BTC ask 10";
    }
    else if (userHelpInput == "exit")
    {
        return "avg -> exits the program";
    }
    else
    {
        return "Invalid input type help to see accepted commands!";
    }

}


std::vector<OrderBookEntry> Advisorbot::getEntries(std::string type, std::string product, std::string currentTime)
{
    std::vector<OrderBookEntry> entries;

    if (type == "ask")
    {
        entries = orderBook.getOrders(OrderBookType::ask, product, currentTime);
    }
    else if (type == "bid")
    {
        entries = orderBook.getOrders(OrderBookType::bid, product, currentTime);
    }

    return entries;
}


std::string Advisorbot::printMaxOrMin(std::string command, std::string product, std::string type)
{   
    std::vector<OrderBookEntry> entries = getEntries(type, product, currentTime);

    double price = getMaxOrMin(command, entries);

        if(command == "min")
        { 
            return "The min ask for " + product + " is " + std::to_string(price); 
        }
        else if(command == "max")
        {
            return "The max ask for " + product + " is " + std::to_string(price);
        }
}


bool Advisorbot::isValidProduct(std::string UerEnteredProduct)
{
    //get valid products from orderbook
    std::vector<std::string> validProducts = orderBook.getKnownProducts();

    if (std::find(validProducts.begin(), validProducts.end(), UerEnteredProduct) != validProducts.end())
    {
        return true;
    }
    else {
        return false;
    }

}

std::string Advisorbot::printAvg(std::string product, std::string type, int timesteps)
{   

    // push in every entry from currentTime into entries
    std::vector<OrderBookEntry> entries = getEntries(type, product, currentTime);
    double avg = 0.0; //initialise double that holds avg

    //user has not passed enough timeframes
    if (timeframes.size() < timesteps)//unique timeframes are pushed into timeframs hence it's length stores unique timeframes
    {   
        //show user current average instead
        avg = getAvg(entries);
        std::string currentAvg = "\ncurrent timeframe average " +product+ +" "+ type +" price is "+ std::to_string(avg);
        return "advisorbot> you have only crossed: " + std::to_string(timeframes.size()) +" timestep(s)"+currentAvg;
    }
    else //user has passed enough timesteps
    {   

        //get entries from latest to latest - timesteps
        for (int i = timeframes.size(); i-- > (timeframes.size()-timesteps); )
        {
            std::vector<OrderBookEntry> oldEntries = getEntries(type, product, timeframes[i]);

            //move oldentries into entries
            std::move(oldEntries.begin(), oldEntries.end(), std::back_inserter(entries));
        }


        //get combined avg of all past timesteps
        avg = getAvg(entries);

        return "advisorbot> The average " + product + " " + type + " price over the last " + std::to_string(timesteps) + " timesteps was " + std::to_string(avg);
    }
    

    
}

std::string Advisorbot::predictVal(std::string command, std::string product, std::string type)
{
    //string that will be output to user
    std::string displayToUser;

    //here we store the max/min price for every timestep
    std::vector<double> prices;

    //only proceed if at least 10 timesteps have passed
    if (timeframes.size() >= 10)//timeframes contains number of passed timeframes
    {   

        //push past 10 timesteps average prices into averagePrices vector
        //loop backwards over previous 9 timesteps
        for (int i = timeframes.size(); i-- > (timeframes.size() - 10); )
        {   
            //get entries for current time in the loop
            std::vector<OrderBookEntry> entry = getEntries(type, product, timeframes[i]);


            //get max/min price for currrent time in a loop push max/min in prices vector
            //only push back if there are entries of the specified type
            if (entry.size() != 0){prices.push_back(getMaxOrMin(command, entry));} 
        }

        //since we have 10 timesteps, weighted moving avg takes 10 + 9 + 8 + 7 +...= 55
        double weight = 55;
        double numerator = 10.0;
        double totalsum = 0.0;

        for (auto& price : prices)
        {
            totalsum += (price * (numerator / weight));
            numerator -= 1.0; //most recent price has the most weight

            displayToUser = "advisorbot > The " + command + " " + type + " will likely be " + std::to_string(totalsum);
        }

    }
    else // not enough timesteps have passed
    {
        displayToUser = "advisorbot> You need a minimum of 10 timesteps to use the predict function";
    }
    

    return displayToUser;
}



double Advisorbot::getAvg(std::vector<OrderBookEntry>& entries)
{   
    double avg = 0.0;

    for (auto &entry : entries)
    {
        avg += entry.price;
    }
    
    return avg / entries.size();
}

double Advisorbot::getMode(std::vector<OrderBookEntry>& entries)
{
    //finding max frequency  
    int max_count = 1, count = 1;
    double res = entries[0].price;

    for (int i = 1; i < entries.size(); i++) {
        if (entries[i].price == entries[i - 1].price)
            count++;
        else {
            if (count > max_count) {
                max_count = count;
                res = entries[i - 1].price;
            }
            count = 1;
        }
    }

    // when the last element is most frequent 
    if (count > max_count)
    {
        max_count = count;
        res = entries[entries.size() - 1].price;
    }

    return res;
}

double Advisorbot::getMedian(std::vector<OrderBookEntry>& entries)
{
    int numEntries = entries.size();
    double median = 0.0;

    if (numEntries % 2 == 0)//even number of entries
    {
        double& first = entries[numEntries / 2 - 1].price;
        double& second = entries[numEntries / 2].price;
        median = (first + second) / 2;
    }
    else //odd number of entries
    {
        //float values get casted as an int
        median = entries[numEntries / 2].price;
    }

    return median;
}

double Advisorbot::getVaraince(std::vector<OrderBookEntry>& entries, double mean)
{
    double variance = 0.0;

    for (int i = 0; i < entries.size(); ++i)
    {
        variance += pow(entries[i].price - mean, 2);
    }
    return variance = variance / entries.size();
}

//get max or min price based on user argument
double Advisorbot::getMaxOrMin(std::string command, std::vector<OrderBookEntry> &entries)
{
    std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceAsc);
    double price = 0.0;

    if (command == "min")
    {
        return (entries.front()).price;
    }
    else if (command == "max")
    {
        return (entries.back()).price;
    }
}

double Advisorbot::getSknewness(double& mean, double& median, double& standardDeviation)
{ 
    return  3*(mean - median)/standardDeviation;
}



std::string Advisorbot::superStats(std::string product, std::string type)
{   
    std::vector<OrderBookEntry> entries = getEntries(type, product, currentTime);

    std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceAsc);

    double max = getMaxOrMin("max", entries);

    double min = getMaxOrMin("min", entries);

    double range = max - min;

    double mean = Advisorbot::getAvg(entries);

    double median = getMedian(entries);

    double mode = getMode(entries);

    double variance = getVaraince(entries, mean);

    double stdDeviation = sqrt(variance);

    double skewness = getSknewness(mean, median, stdDeviation);

    std::string skewType;

    if (skewness > -0.5 && skewness < 0.5) { skewType = "Aproximately symmetric"; }
    else if ((skewness < -0.5 && skewness > -1.0) && (skewness < 1.0 && skewness > 0.5)) { skewType = "Modertaely skewed"; }
    else {skewType = "Highly skewed"; }


    return "max is " + std::to_string(max) +
        "\nmin is " + std::to_string(min) +
        "\nrange is " + std::to_string(range) +
        "\nmean is " + std::to_string(mean) +
        "\nmode is " + std::to_string(mode) +
        "\nmedian is " + std::to_string(median) +
        "\nvariance is " + std::to_string(variance) +
        "\nstandard deviation is " + std::to_string(stdDeviation) +
        "\nskewness is " + std::to_string(skewness) +
        "\nprice data is " + skewType;
}




void Advisorbot::processUserInput(std::string userInput)
{
    //get users input and tokenize it by spaces
    std::vector<std::string> userArguments = CSVReader::tokenise(userInput, ' ');

    //isolate command as first word before space
    std::string command = userArguments[0];

    //if length 1 then user has typed either one of the 1 word commands
    if (userArguments.size() == 1)
    {
        if (command == "help")
        {
            printHelp();
        }
        else if (command == "time")
        {
            std::cout << "advisorbot> " << currentTime << std::endl;
        }
        else if (command == "step")
        {
            gotoNextTimeframe();
            std::cout << "advisorbot> now at " << currentTime << std::endl;
        }
        else if (command == "market")
        {
            std::cout << "advisorbot> " << std::endl;
            printMarketStats();
        }
        else if (command == "prod")
        {
            printProducts();
        }
        else if (command == "wallet")
        {
            printWallet();
        }
        else if (command == "ask")
        {
            enterAsk();
        }
        else if (command == "bid")
        {
            enterBid();
        }
        else if (command == "exit")
        {
            isRunning = false;//ends the program
        }
        else //user has typed command of length 1 word but non of them are valid commands
        {
            std::cout << "invalid input pls type help for valid commands" << std::endl;
        }
    }
    //if length 2 then user has typed either one of the 2 character commands
    else if (userArguments.size() == 2)
    {
        //meaning user has typed help <cmd> process char after help
        if (command == "help")
        {
            //return advisor bot query after user has typed help
            std::string helpComment = provideHelp(userArguments[1]);
            std::cout << "advisorbot> " << helpComment << std::endl;
        }
        else //user has typed arguments of length 2 but first arg was not help
        {
            std::cout << "invalid input pls type help for valid commands" << std::endl;
        }
    }
    //if length 3 then user has typed either one of the 3 character commands
    else if (userArguments.size() == 3)
    {
        if( isValidProduct(userArguments[1]) && (userArguments[2] == "bid" || userArguments[2] == "ask") )
        {
            if (command == "max" || command == "min")
            {   
                //arguments are max/min product and bid/ask
                std::string statement = printMaxOrMin(command, userArguments[1], userArguments[2]);
                //print out the min max statement or an error message
                std::cout << statement << std::endl;
            }
            else if (command == "stats")
            {   
                //arguments are product and type
                std::string statement = superStats(userArguments[1], userArguments[2]);
                //print output from kurtosis
                std::cout << statement << std::endl;
            }
            else// user did not input max/min or stats
            {
                std::cout << "invalid input pls type help for valid commands" << std::endl;
            }

        }
        else //user did not put in a valid product or argument 2 does not contain bid or ask
        {
            std::cout << "invalid input pls type help for valid commands" << std::endl;
        }
    }
    //if length 4 then user has typed either one of the 4 character commands
    else if (userArguments.size() == 4)
    {

        if (command == "predict" && isValidProduct(userArguments[2]))
        {
            //arguments passed are max/min product ask/bid
            std::string output = predictVal(userArguments[1], userArguments[2], userArguments[3]);
            std::cout << output << std::endl; //print output of predict
        }
        else if (command == "avg" && isValidProduct(userArguments[1]))
        {
            try //try block checks if user has input a string that cannot be turned into an int
            {
                const int timesteps{ std::stoi(userArguments[3]) };
                //arguments are product, ask/bid, and number of timesteps
                std::string output = printAvg(userArguments[1], userArguments[2], timesteps);
                std::cout << output << std::endl; //print output of printavg
            }
            catch (std::invalid_argument const& ex) //user has typed somthing that cannot be converted into a timestep
            {
                //print error message and return from processUserInput
                std::cout << "advisorbot> Input does not contain a valid timestep! " << std::endl;
                return;
            }
        }
        else //user has typed something other than predict or avg
        {
            std::cout << "invalid input pls type help for valid commands" << std::endl;
        }
    }
    else //argument length is greater than 4
    {
        std::cout << "invalid input pls type help for valid commands" << std::endl;
    }
}//end of processUserInput
