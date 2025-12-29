#include "OrderBook.h"
#include <iostream>
#include <cassert>

void PrintOutcome(const std::string& testName, OrderOutcome outcome) {
    std::cout << testName << ": ";
    switch(outcome) {
        case OrderOutcome::FullyFilled: 
            std::cout << "FullyFilled"; break;
        case OrderOutcome::PartiallyFilledAndCancelled: 
            std::cout << "PartiallyFilledAndCancelled"; break;
        case OrderOutcome::PartiallyFilledAndAddedToBook: 
            std::cout << "PartiallyFilledAndAddedToBook"; break;
        case OrderOutcome::Cancelled: 
            std::cout << "Cancelled"; break;
        case OrderOutcome::AddedToOrderbook: 
            std::cout << "AddedToOrderbook"; break;
    }
    std::cout << std::endl;
}
/*
int main() {
    Orderbook book;
    
    std::cout << "=== TEST 1: Basic Limit Orders ===" << std::endl;
    Order limitSell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
    Order limitSell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 30);
    Order limitBuy1(OrderType::GoodTillCancel, 3, 99.0, Side::Buy, 40);
    Order limitBuy2(OrderType::GoodTillCancel, 4, 98.0, Side::Buy, 20);
    
    PrintOutcome("Sell@100x50", book.ExecuteTrade(limitSell1));  // Should add to book
    PrintOutcome("Sell@101x30", book.ExecuteTrade(limitSell2));  // Should add to book
    PrintOutcome("Buy@99x40", book.ExecuteTrade(limitBuy1));     // Should add to book
    PrintOutcome("Buy@98x20", book.ExecuteTrade(limitBuy2));     // Should add to book
    
    std::cout << "\n=== TEST 2: Market Order - Full Fill ===" << std::endl;
    Order marketBuy1(OrderType::Market, 5, 0, Side::Buy, 50);
    PrintOutcome("Market Buy x50", book.ExecuteTrade(marketBuy1)); // Should fully fill at 100
    
    std::cout << "\n=== TEST 3: Market Order - Partial Fill ===" << std::endl;
    Order marketBuy2(OrderType::Market, 6, 0, Side::Buy, 100);
    PrintOutcome("Market Buy x100", book.ExecuteTrade(marketBuy2)); // Partially filled (only 30 left at 101)
    
    std::cout << "\n=== TEST 4: Limit Order - Immediate Fill ===" << std::endl;
    Order limitSell3(OrderType::GoodTillCancel, 7, 97.0, Side::Sell, 25);
    PrintOutcome("Sell@97x25", book.ExecuteTrade(limitSell3)); // Should fill against buy@99
    
    std::cout << "\n=== TEST 5: IOC - Full Fill ===" << std::endl;
    Order limitSell4(OrderType::GoodTillCancel, 8, 100.0, Side::Sell, 20);
    book.ExecuteTrade(limitSell4); // Add some asks back
    
    Order iocBuy1(OrderType::ImmediateOrCancel, 9, 100.0, Side::Buy, 20);
    PrintOutcome("IOC Buy@100x20", book.ExecuteTrade(iocBuy1)); // Should fully fill
    
    std::cout << "\n=== TEST 6: IOC - Partial Fill ===" << std::endl;
    Order limitSell5(OrderType::GoodTillCancel, 10, 101.0, Side::Sell, 10);
    book.ExecuteTrade(limitSell5);
    
    Order iocBuy2(OrderType::ImmediateOrCancel, 11, 101.0, Side::Buy, 50);
    PrintOutcome("IOC Buy@101x50", book.ExecuteTrade(iocBuy2)); // Partial fill, cancel rest
    
    std::cout << "\n=== TEST 7: Fill-or-Kill - Success ===" << std::endl;
    Order limitSell6(OrderType::GoodTillCancel, 12, 102.0, Side::Sell, 100);
    book.ExecuteTrade(limitSell6);
    
    Order fokBuy1(OrderType::FillOrKill, 13, 102.0, Side::Buy, 100);
    PrintOutcome("FOK Buy@102x100", book.ExecuteTrade(fokBuy1)); // Should fully fill
    
    std::cout << "\n=== TEST 8: Fill-or-Kill - Failure ===" << std::endl;
    Order limitSell7(OrderType::GoodTillCancel, 14, 103.0, Side::Sell, 50);
    book.ExecuteTrade(limitSell7);
    
    Order fokBuy2(OrderType::FillOrKill, 15, 103.0, Side::Buy, 200);
    PrintOutcome("FOK Buy@103x200", book.ExecuteTrade(fokBuy2)); // Should cancel (not enough volume)
    
    std::cout << "\n=== TEST 9: Order Modification ===" << std::endl;
    Order limitBuy3(OrderType::GoodTillCancel, 16, 95.0, Side::Buy, 100);
    book.ExecuteTrade(limitBuy3);
    
    bool modSuccess = book.ModifyOrder(16, 96.0, 80);
    std::cout << "Modify Order 16 (price 95->96, vol 100->80): " 
              << (modSuccess ? "Success" : "Failed") << std::endl;
    
    std::cout << "\n=== TEST 10: Order Cancellation ===" << std::endl;
    bool cancelSuccess = book.CancelOrder(16);
    std::cout << "Cancel Order 16: " 
              << (cancelSuccess ? "Success" : "Failed") << std::endl;
    
    std::cout << "\n=== TEST 11: Edge Cases ===" << std::endl;
    Order invalidOrder(OrderType::Market, 17, 0, Side::Buy, -10);
    PrintOutcome("Market Buy x-10 (invalid)", book.ExecuteTrade(invalidOrder)); // Should cancel
    
    Order emptyMarket(OrderType::Market, 18, 0, Side::Sell, 50);
    PrintOutcome("Market Sell x50 (empty bids)", book.ExecuteTrade(emptyMarket)); // Should cancel
    
    std::cout << "\n=== All Tests Complete ===" << std::endl;
    
    return 0;
}

*/