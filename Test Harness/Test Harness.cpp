#include <iostream>
#include "OrderBook.h"

// Paste all your classes here (Order, CompletedOrders, Orderbook, etc.)
// Or if you have them in separate header files:
// #include "Order.h"
// #include "Orderbook.h"

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

    OrderOutcome result1 = book.ExecuteTrade(buyLimit1);
    std::cout << "Buy limit @99: " << static_cast<int>(result1) << std::endl;
    
    OrderOutcome result2 = book.ExecuteTrade(buyLimit2);
    std::cout << "Buy limit @98: " << static_cast<int>(result2) << std::endl;
    
    OrderOutcome result3 = book.ExecuteTrade(sellLimit1);
    std::cout << "Sell limit @101: " << static_cast<int>(result3) << std::endl;
    
    OrderOutcome result4 = book.ExecuteTrade(sellLimit2);
    std::cout << "Sell limit @102: " << static_cast<int>(result4) << std::endl;
    
    std::cout << std::endl;

    // Test 2: Market order that crosses the spread
    std::cout << "Test 2: Market buy order (should fill from asks)" << std::endl;
    Order marketBuy(OrderType::Market, 5, 0.0, Side::Buy, 70.0);
    OrderOutcome result5 = book.ExecuteTrade(marketBuy);
    std::cout << "Market buy 70 volume: " << static_cast<int>(result5) << std::endl;
    std::cout << std::endl;

    // Test 3: Limit order that crosses immediately
    std::cout << "Test 3: Aggressive limit order (crosses spread)" << std::endl;
    Order aggressiveBuy(OrderType::GoodTillCancel, 6, 102.0, Side::Buy, 50.0);
    OrderOutcome result6 = book.ExecuteTrade(aggressiveBuy);
    std::cout << "Buy limit @102 (50 volume): " << static_cast<int>(result6) << std::endl;
    std::cout << std::endl;

    // Test 4: Fill-or-Kill with sufficient volume
    std::cout << "Test 4: FOK with sufficient volume" << std::endl;
    Order sellFOK1(OrderType::GoodTillCancel, 7, 97.0, Side::Sell, 100.0);
    book.ExecuteTrade(sellFOK1); // Add volume first
    
    Order buyFOK(OrderType::FillOrKill, 8, 97.0, Side::Buy, 80.0);
    OrderOutcome result7 = book.ExecuteTrade(buyFOK);
    std::cout << "FOK buy 80 volume @97: " << static_cast<int>(result7) << std::endl;
    std::cout << std::endl;

    // Test 5: FOK with insufficient volume (should cancel)
    std::cout << "Test 5: FOK with insufficient volume (should cancel)" << std::endl;
    Order buyFOK2(OrderType::FillOrKill, 9, 97.0, Side::Buy, 500.0);
    OrderOutcome result8 = book.ExecuteTrade(buyFOK2);
    std::cout << "FOK buy 500 volume @97: " << static_cast<int>(result8) << std::endl;
    std::cout << std::endl;

    // Test 6: IOC partial fill
    std::cout << "Test 6: IOC order (partial fill expected)" << std::endl;
    Order sellIOC(OrderType::GoodTillCancel, 10, 96.0, Side::Sell, 30.0);
    book.ExecuteTrade(sellIOC); // Add some volume
    
    Order buyIOC(OrderType::ImmediateOrCancel, 11, 96.0, Side::Buy, 100.0);
    OrderOutcome result9 = book.ExecuteTrade(buyIOC);
    std::cout << "IOC buy 100 volume @96 (only 30 available): " << static_cast<int>(result9) << std::endl;
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