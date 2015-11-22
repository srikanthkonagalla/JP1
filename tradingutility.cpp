/****************************************************************
* This file provides the functions to do the following activities
* 1) Calculate Dividend Yield
* 2) Calculate P/E Ratio
* 3) Store Trades 
* 4) Calculate Volume Weighted Stock Price
* 5) Calculate Geometric Mean
******************************************************************/

#include <iostream>
#include <stdexcept>
#include <exception>
#include <ctime>
#include <map>
#include <string>
#include <cstring>
#include <cmath>
using namespace std;

#define COMMON "Common"
#define PREFERRED "Preferred"

/***************************************************************************************
* Function : calculateDividendYield
* Description : This function takes the following parameters and 
*               returns Dividend Yield. This function throws an 
*               invalid argument exception if the trade type is 
*               neither Common nor Preferred.
*  Paraneter 1 : Price
*  Paraneter 2 : Trade type , should be Common or Preferred.
*  Paraneter 3 : Last Dividend , value considered only when trade type is Common
*  Paraneter 4 : Fixed Dividend, value considered only when trade type is Preferred
*  Paraneter 5 : Par Value, value considered only when trade type is Preferred
*******************************************************************************************/

double calculateDividendYield(long price, 
                              string tradeType, 
                              double lastDividend, 
                              double fixedDividend, 
                              long parValue) 
{
   if (tradeType == COMMON) return lastDividend/price;
   else if (tradeType == PREFERRED) return (fixedDividend*parValue)/price;
   else throw invalid_argument ("Invalid Trade Type");
}

/******************************************************************
* Function : calculatePERatio
* Description : This function takes the following parameters and 
*               returns P/E Ratio. 
*  Paraneter 1 : Price
*  Paraneter 2 : Dividend.
*******************************************************************/

double calculatePERatio(long price, 
                        double dividend)
{
    return price/dividend;
}

/******************************************************************
* Class : Trade
* Description : This class stores all the trade information.
*******************************************************************/


class Trade {
    private:
       string timeStampStr;
       long quantity;
       char buySellIndicator;
       long price;

     public:
        Trade (string inputTimeStamp, long inputQuantity, char inputBuySellIndicator, long inputPrice)
        : timeStampStr(inputTimeStamp), 
          quantity(inputQuantity), 
          buySellIndicator(inputBuySellIndicator),
          price(inputPrice) 
        {
        };
        string getTimeStamp() { return timeStampStr; };
        long getQuantity() { return quantity; };
        char getBuySellIndicator() { return buySellIndicator; };
        long getPrice() { return price; };
};

/******************************************************************
* Class : noTradesFound
* Description : This is an exception class. An object of this class
*               is thrown when there are no trades for the given 
*               criteria.
*******************************************************************/


class noTradesFound : public exception {
  virtual const char * what() const _GLIBCXX_USE_NOEXCEPT
  {
     return "No Trades Found";
  }
} NoTradesFound;

/**************************************************************************************************
* Class : StoreTrades
* Description : This class stores the trades given and provides the
*               the following functions.
*
*** Function 1: recordTrade
*** Function Description : This function takes the following parameters
***          and creates a trade and stores it in memory
*** Parameter 1 : timeStamp - date and time of trade, it should be in format YYYY-MM-DD HH24:MI:SS
*** Parameter 2 : quantity - quantity of trade
*** Parameter 3 : buySellIndicator - buy or sell indicator of the trade should be 'B' or 'C'
*** Parameter 4 : price - price of the trade

*** Function 2: getVolumeWeightedStockPrice
*** Function Description : This function calculates volumes weighted stock price of all the trades
***         happened in the last 15 minutes. If there are no trades then it throws exception NoTradesFound.

*** Function 3: calculateGeometricMean
*** Function Description : This function calculates and returns geometric mean of all the trades.

******************************************************************************************************/


class StoreTrades {
    private:
       multimap<time_t, Trade*> trades;

   public:
      int recordTrade(string timeStamp, long quantity, char buySellIndicator, long price)
      {
         Trade *trade = new Trade(timeStamp, quantity, buySellIndicator, price);
         time_t tradeTime;
         struct tm tm;
         memset(&tm, 0, sizeof(struct tm));
         strptime(timeStamp.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
         tradeTime = mktime(&tm);
         trades.insert(pair<time_t, Trade*>(tradeTime, trade)); 
      };
      double getVolumeWeightedStockPrice()
      {
         time_t currentTime;
         time_t fifteenMinutesBackTime;
     
         time(&currentTime);
         fifteenMinutesBackTime = currentTime - (15*60);

         double accumulatedPrice = 0.0;
         double accumulatedQuantity = 0.0;

         for(multimap<time_t, Trade*>::iterator it = trades.lower_bound(fifteenMinutesBackTime);
             it != trades.end();
             it++)
         {
            accumulatedQuantity += (*it).second->getQuantity();
            accumulatedPrice += ( (*it).second->getQuantity() * (*it).second->getPrice() ) ;
         }
 
         if (accumulatedQuantity != 0.0)
         {
             return accumulatedPrice/accumulatedQuantity;
         }
         else
         {
            throw NoTradesFound;
         }
 
      };
      double calculateGeometricMean()
      {
         double accumulatedPrice = 1.0;

         for(multimap<time_t, Trade*>::iterator it = trades.begin();
             it != trades.end();
             it++)
         {
            accumulatedPrice *= (*it).second->getPrice();
         }
         if (trades.size() != 0)
         {
             return pow(accumulatedPrice, 1/(double) (trades.size()) );
         }
         else
         {
            throw NoTradesFound;
         }
      };
	  ~StoreTrades()
      {
         for(multimap<time_t, Trade*>::iterator it = trades.begin();
             it != trades.end();
             it++)
         {
            delete((*it).second);
         }
       };
};

