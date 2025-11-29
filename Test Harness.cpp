#include <iostream>
#include "OrderBook.h"

int main()
{
    std::cout << "=== Orderbook Testing ===" << std::endl << std::endl;
    Orderbook book;
    
    // Test 1: Add some limit orders to the book
    std::cout << "Test 1: Adding limit orders to orderbook" << std::endl;
    Order buyLimit1(OrderType::GoodTillCancel, 1, 99.0, Side::Buy, 100.0);
    Order buyLimit2(OrderType::GoodTillCancel, 2, 98.0, Side::Buy, 50.0);
    Order sellLimit1(OrderType::GoodTillCancel, 3, 101.0, Side::Sell, 80.0);
    Order sellLimit2(OrderType::GoodTillCancel, 4, 102.0, Side::Sell, 60.0);
    
    std::cout << "Buy limit @99 (Volume: 100): ";
    OrderOutcome result1 = book.ExecuteTrade(buyLimit1);
    std::cout << static_cast<int>(result1) << " (Expected: 4 - AddedToOrderbook)" << std::endl;
    
    std::cout << "Buy limit @98 (Volume: 50): ";
    OrderOutcome result2 = book.ExecuteTrade(buyLimit2);
    std::cout << static_cast<int>(result2) << " (Expected: 4 - AddedToOrderbook)" << std::endl;
    
    std::cout << "Sell limit @101 (Volume: 80): ";
    OrderOutcome result3 = book.ExecuteTrade(sellLimit1);
    std::cout << static_cast<int>(result3) << " (Expected: 4 - AddedToOrderbook)" << std::endl;
    
    std::cout << "Sell limit @102 (Volume: 60): ";
    OrderOutcome result4 = book.ExecuteTrade(sellLimit2);
    std::cout << static_cast<int>(result4) << " (Expected: 4 - AddedToOrderbook)" << std::endl;
    
    std::cout << std::endl;
    
    // Test 2: Market order that crosses the spread
    std::cout << "Test 2: Market buy order (should fill from asks)" << std::endl;
    Order marketBuy(OrderType::Market, 5, 0.0, Side::Buy, 70.0);
    std::cout << "Market buy (Volume: 70): ";
    OrderOutcome result5 = book.ExecuteTrade(marketBuy);
    std::cout << static_cast<int>(result5) << " (Expected: 0 - FullyFilled)" << std::endl;
    std::cout << std::endl;
    
    // Test 3: Limit order that crosses immediately
    std::cout << "Test 3: Aggressive limit order (crosses spread)" << std::endl;
    Order aggressiveBuy(OrderType::GoodTillCancel, 6, 102.0, Side::Buy, 50.0);
    std::cout << "Buy limit @102 (Volume: 50): ";
    OrderOutcome result6 = book.ExecuteTrade(aggressiveBuy);
    std::cout << static_cast<int>(result6) << " (Expected: 0 - FullyFilled)" << std::endl;
    std::cout << std::endl;
    
    // Test 4: Fill-or-Kill with sufficient volume
    std::cout << "Test 4: FOK with sufficient volume" << std::endl;
    Order sellFOK1(OrderType::GoodTillCancel, 7, 97.0, Side::Sell, 100.0);
    book.ExecuteTrade(sellFOK1);
    
    Order buyFOK(OrderType::FillOrKill, 8, 97.0, Side::Buy, 80.0);
    std::cout << "FOK buy @97 (Volume: 80): ";
    OrderOutcome result7 = book.ExecuteTrade(buyFOK);
    std::cout << static_cast<int>(result7) << " (Expected: 0 - FullyFilled)" << std::endl;
    std::cout << std::endl;
    
    // Test 5: FOK with insufficient volume (should cancel)
    std::cout << "Test 5: FOK with insufficient volume (should cancel)" << std::endl;
    Order buyFOK2(OrderType::FillOrKill, 9, 97.0, Side::Buy, 500.0);
    std::cout << "FOK buy @97 (Volume: 500): ";
    OrderOutcome result8 = book.ExecuteTrade(buyFOK2);
    std::cout << static_cast<int>(result8) << " (Expected: 3 - Cancelled)" << std::endl;
    std::cout << std::endl;
    
    // Test 6: IOC partial fill
    std::cout << "Test 6: IOC order (partial fill expected)" << std::endl;
    Order sellIOC(OrderType::GoodTillCancel, 10, 96.0, Side::Sell, 30.0);
    book.ExecuteTrade(sellIOC);
    
    Order buyIOC(OrderType::ImmediateOrCancel, 11, 96.0, Side::Buy, 100.0);
    std::cout << "IOC buy @96 (Volume: 100, only 30 available): ";
    OrderOutcome result9 = book.ExecuteTrade(buyIOC);
    std::cout << static_cast<int>(result9) << " (Expected: 1 - PartiallyFilledAndCancelled)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== OrderOutcome Legend ===" << std::endl;
    std::cout << "0 = FullyFilled" << std::endl;
    std::cout << "1 = PartiallyFilledAndCancelled" << std::endl;
    std::cout << "2 = PartiallyFilledAndAddedToBook" << std::endl;
    std::cout << "3 = Cancelled" << std::endl;
    std::cout << "4 = AddedToOrderbook" << std::endl;
    
    std::cout << std::endl << "Press Enter to exit...";
    std::cin.get();
    
    return 0;
}