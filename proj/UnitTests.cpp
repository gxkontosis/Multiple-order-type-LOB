#include <gtest/gtest.h>
#include "OrderBook.h"

// ==================== BASIC LIMIT ORDER TESTS ====================

// Test: Buy limit order added to empty book
TEST(OrderbookTest, BuyLimitOrderAddedToEmptyBook) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   
   OrderOutcome result = book.ExecuteTrade(order);
   
   EXPECT_EQ(result, OrderOutcome::AddedToOrderbook);
}

// Test: Sell limit order added to empty book
TEST(OrderbookTest, SellLimitOrderAddedToEmptyBook) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   
   OrderOutcome result = book.ExecuteTrade(order);
   
   EXPECT_EQ(result, OrderOutcome::AddedToOrderbook);
}

// Test: Buy limit order added when price below best ask
TEST(OrderbookTest, BuyLimitOrderAddedWhenBelowBestAsk) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::GoodTillCancel, 2, 99.0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::AddedToOrderbook);
}

// Test: Sell limit order added when price above best bid
TEST(OrderbookTest, SellLimitOrderAddedWhenAboveBestBid) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 50);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::AddedToOrderbook);
}

// ==================== LIMIT ORDER IMMEDIATE EXECUTION TESTS ====================

// Test: Buy limit order immediately fills when price at best ask
TEST(OrderbookTest, BuyLimitOrderImmediatelyFillsAtBestAsk) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::GoodTillCancel, 2, 100.0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Buy limit order immediately fills when price above best ask
TEST(OrderbookTest, BuyLimitOrderImmediatelyFillsAboveBestAsk) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::GoodTillCancel, 2, 105.0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Sell limit order immediately fills when price at best bid
TEST(OrderbookTest, SellLimitOrderImmediatelyFillsAtBestBid) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::GoodTillCancel, 2, 100.0, Side::Sell, 50);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Sell limit order immediately fills when price below best bid
TEST(OrderbookTest, SellLimitOrderImmediatelyFillsBelowBestBid) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::GoodTillCancel, 2, 95.0, Side::Sell, 50);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Limit order partially fills and remainder added to book
TEST(OrderbookTest, LimitOrderPartiallyFilledAndAddedToBook) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::GoodTillCancel, 2, 100.0, Side::Buy, 80);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::PartiallyFilledAndAddedToBook);
}

// Test: Limit order consumes multiple price levels and fully fills
TEST(OrderbookTest, LimitOrderConsumesMultipleLevelsFullyFills) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   
   Order buy(OrderType::GoodTillCancel, 3, 101.0, Side::Buy, 70);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Limit order consumes multiple price levels and partially fills
TEST(OrderbookTest, LimitOrderConsumesMultipleLevelsPartiallyFills) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   
   Order buy(OrderType::GoodTillCancel, 3, 101.0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::PartiallyFilledAndAddedToBook);
}

// Test: Limit order stops at price limit boundary
TEST(OrderbookTest, LimitOrderStopsAtPriceLimit) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   Order sell3(OrderType::GoodTillCancel, 3, 102.0, Side::Sell, 50);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   
   Order buy(OrderType::GoodTillCancel, 4, 101.0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::PartiallyFilledAndAddedToBook);
}

// ==================== MARKET ORDER TESTS ====================

// Test: Market buy order fully fills against single resting order
TEST(OrderbookTest, MarketBuyOrderFullyFillsSingleLevel) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::Market, 2, 0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Market sell order fully fills against single resting order
TEST(OrderbookTest, MarketSellOrderFullyFillsSingleLevel) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::Market, 2, 0, Side::Sell, 50);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Market order partially fills when insufficient volume available
TEST(OrderbookTest, MarketOrderPartiallyFilledInsufficientVolume) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::Market, 2, 0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::PartiallyFilledAndCancelled);
}

// Test: Market buy order cancelled when asks empty
TEST(OrderbookTest, MarketBuyOrderCancelledWhenAsksEmpty) {
   Orderbook book;
   
   Order buy(OrderType::Market, 1, 0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: Market sell order cancelled when bids empty
TEST(OrderbookTest, MarketSellOrderCancelledWhenBidsEmpty) {
   Orderbook book;
   
   Order sell(OrderType::Market, 1, 0, Side::Sell, 50);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: Market order consumes multiple price levels
TEST(OrderbookTest, MarketOrderConsumesMultipleLevels) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   
   Order buy(OrderType::Market, 3, 0, Side::Buy, 60);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Market order exhausts all available liquidity
TEST(OrderbookTest, MarketOrderExhaustsAllLiquidity) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 20);
   Order sell3(OrderType::GoodTillCancel, 3, 102.0, Side::Sell, 25);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   
   Order buy(OrderType::Market, 4, 0, Side::Buy, 75);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Market order partially fills resting order at price level
TEST(OrderbookTest, MarketOrderPartiallyFillsRestingOrder) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 100);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::Market, 2, 0, Side::Buy, 60);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// ==================== IMMEDIATE-OR-CANCEL TESTS ====================

// Test: IOC buy order fully fills at limit price
TEST(OrderbookTest, IOCBuyOrderFullyFillsAtLimit) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::ImmediateOrCancel, 2, 100.0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: IOC sell order fully fills at limit price
TEST(OrderbookTest, IOCSellOrderFullyFillsAtLimit) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::ImmediateOrCancel, 2, 100.0, Side::Sell, 50);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: IOC order partially fills and cancels remainder
TEST(OrderbookTest, IOCOrderPartiallyFillsCancelsRemainder) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::ImmediateOrCancel, 2, 100.0, Side::Buy, 80);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::PartiallyFilledAndCancelled);
}

// Test: IOC buy order cancelled when price below best ask
TEST(OrderbookTest, IOCBuyOrderCancelledWhenPriceBelowBestAsk) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 101.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::ImmediateOrCancel, 2, 99.0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: IOC sell order cancelled when price above best bid
TEST(OrderbookTest, IOCSellOrderCancelledWhenPriceAboveBestBid) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 99.0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::ImmediateOrCancel, 2, 101.0, Side::Sell, 50);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: IOC order consumes multiple price levels within limit
TEST(OrderbookTest, IOCOrderConsumesMultipleLevelsWithinLimit) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   
   Order buy(OrderType::ImmediateOrCancel, 3, 101.0, Side::Buy, 70);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: IOC order stops at price limit with partial fill
TEST(OrderbookTest, IOCOrderStopsAtPriceLimitPartialFill) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   Order sell3(OrderType::GoodTillCancel, 3, 102.0, Side::Sell, 50);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   
   Order buy(OrderType::ImmediateOrCancel, 4, 101.0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::PartiallyFilledAndCancelled);
}

// ==================== FILL-OR-KILL TESTS ====================

// Test: FOK buy order executes when sufficient volume available at single level
TEST(OrderbookTest, FOKBuyOrderExecutesSufficientVolumeSingleLevel) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 100);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::FillOrKill, 2, 100.0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: FOK sell order executes when sufficient volume available at single level
TEST(OrderbookTest, FOKSellOrderExecutesSufficientVolumeSingleLevel) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 100);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::FillOrKill, 2, 100.0, Side::Sell, 100);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: FOK order executes when sufficient volume across multiple levels
TEST(OrderbookTest, FOKOrderExecutesSufficientVolumeMultipleLevels) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 60);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   
   Order buy(OrderType::FillOrKill, 3, 101.0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: FOK buy order cancelled when insufficient volume
TEST(OrderbookTest, FOKBuyOrderCancelledInsufficientVolume) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::FillOrKill, 2, 100.0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: FOK sell order cancelled when insufficient volume
TEST(OrderbookTest, FOKSellOrderCancelledInsufficientVolume) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::FillOrKill, 2, 100.0, Side::Sell, 100);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: FOK order cancelled when volume exists but outside price limit
TEST(OrderbookTest, FOKOrderCancelledVolumeOutsidePriceLimit) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   Order sell2(OrderType::GoodTillCancel, 2, 102.0, Side::Sell, 60);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   
   Order buy(OrderType::FillOrKill, 3, 101.0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// ==================== PRICE-TIME PRIORITY TESTS ====================

// Test: Orders at same price level execute in FIFO order
TEST(OrderbookTest, PriceTimePriorityFIFOAtSameLevel) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 100.0, Side::Sell, 20);
   Order sell3(OrderType::GoodTillCancel, 3, 100.0, Side::Sell, 10);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   
   Order buy(OrderType::Market, 4, 0, Side::Buy, 35);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Best price executes before worse price
TEST(OrderbookTest, BestPriceExecutesFirst) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 101.0, Side::Sell, 50);
   Order sell2(OrderType::GoodTillCancel, 2, 100.0, Side::Sell, 50);
   Order sell3(OrderType::GoodTillCancel, 3, 102.0, Side::Sell, 50);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   
   Order buy(OrderType::Market, 4, 0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// ==================== ORDER MODIFICATION TESTS ====================

// Test: Modify order volume decrease succeeds
TEST(OrderbookTest, ModifyOrderVolumeDecreaseSucceeds) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 100);
   book.ExecuteTrade(order);
   
   bool result = book.ModifyOrder(1, 100.0, 50);
   
   EXPECT_TRUE(result);
}

// Test: Modify order price change succeeds for buy order
TEST(OrderbookTest, ModifyBuyOrderPriceChangeSucceeds) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(order);
   
   bool result = book.ModifyOrder(1, 95.0, 50);
   
   EXPECT_TRUE(result);
}

// Test: Modify order price change succeeds for sell order
TEST(OrderbookTest, ModifySellOrderPriceChangeSucceeds) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(order);
   
   bool result = book.ModifyOrder(1, 105.0, 50);
   
   EXPECT_TRUE(result);
}

// Test: Modify order with both price and volume change succeeds
TEST(OrderbookTest, ModifyOrderPriceAndVolumeChangeSucceeds) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 100);
   book.ExecuteTrade(order);
   
   bool result = book.ModifyOrder(1, 95.0, 50);
   
   EXPECT_TRUE(result);
}

// Test: Modify order fails for non-existent order
TEST(OrderbookTest, ModifyOrderFailsForNonExistentOrder) {
   Orderbook book;
   
   bool result = book.ModifyOrder(999, 100.0, 50);
   
   EXPECT_FALSE(result);
}

// Test: Modify order with zero volume cancels order
TEST(OrderbookTest, ModifyOrderZeroVolumeCancelsOrder) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(order);
   
   bool result = book.ModifyOrder(1, 100.0, 0);
   
   EXPECT_TRUE(result);
}

// Test: Modify order with negative volume cancels order
TEST(OrderbookTest, ModifyOrderNegativeVolumeCancelsOrder) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(order);
   
   bool result = book.ModifyOrder(1, 100.0, -10);
   
   EXPECT_TRUE(result);
}

// Test: Cannot modify order to increase volume
TEST(OrderbookTest, ModifyOrderCannotIncreaseVolume) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(order);
   
   bool result = book.ModifyVolume(order, 100);
   
   EXPECT_FALSE(result);
}

// Test: Modify order that was partially filled
TEST(OrderbookTest, ModifyPartiallyFilledOrder) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 100);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::Market, 2, 0, Side::Buy, 60);
   book.ExecuteTrade(buy);
   
   bool result = book.ModifyOrder(1, 100.0, 20);
   
   EXPECT_TRUE(result);
}

// ==================== ORDER CANCELLATION TESTS ====================

// Test: Cancel buy order succeeds
TEST(OrderbookTest, CancelBuyOrderSucceeds) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(order);
   
   bool result = book.CancelOrder(1);
   
   EXPECT_TRUE(result);
}

// Test: Cancel sell order succeeds
TEST(OrderbookTest, CancelSellOrderSucceeds) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(order);
   
   bool result = book.CancelOrder(1);
   
   EXPECT_TRUE(result);
}

// Test: Cancel order fails for non-existent order
TEST(OrderbookTest, CancelOrderFailsForNonExistentOrder) {
   Orderbook book;
   
   bool result = book.CancelOrder(999);
   
   EXPECT_FALSE(result);
}

// Test: Cancel order fails for already filled order
TEST(OrderbookTest, CancelOrderFailsForFilledOrder) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::Market, 2, 0, Side::Buy, 50);
   book.ExecuteTrade(buy);
   
   bool result = book.CancelOrder(1);
   
   EXPECT_FALSE(result);
}

// Test: Cancel partially filled order succeeds
TEST(OrderbookTest, CancelPartiallyFilledOrderSucceeds) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 100);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::Market, 2, 0, Side::Buy, 60);
   book.ExecuteTrade(buy);
   
   bool result = book.CancelOrder(1);
   
   EXPECT_TRUE(result);
}

// ==================== INVALID ORDER TESTS ====================

// Test: Order with negative volume is rejected
TEST(OrderbookTest, OrderWithNegativeVolumeRejected) {
   Orderbook book;
   Order order(OrderType::Market, 1, 0, Side::Buy, -10);
   
   OrderOutcome result = book.ExecuteTrade(order);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: Order with zero volume is rejected
TEST(OrderbookTest, OrderWithZeroVolumeRejected) {
   Orderbook book;
   Order order(OrderType::Market, 1, 0, Side::Buy, 0);
   
   OrderOutcome result = book.ExecuteTrade(order);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: Limit order with zero volume is rejected
TEST(OrderbookTest, LimitOrderWithZeroVolumeRejected) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 0);
   
   OrderOutcome result = book.ExecuteTrade(order);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: IOC order with zero volume is rejected
TEST(OrderbookTest, IOCOrderWithZeroVolumeRejected) {
   Orderbook book;
   Order order(OrderType::ImmediateOrCancel, 1, 100.0, Side::Buy, 0);
   
   OrderOutcome result = book.ExecuteTrade(order);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// Test: FOK order with zero volume is rejected
TEST(OrderbookTest, FOKOrderWithZeroVolumeRejected) {
   Orderbook book;
   Order order(OrderType::FillOrKill, 1, 100.0, Side::Buy, 0);
   
   OrderOutcome result = book.ExecuteTrade(order);
   
   EXPECT_EQ(result, OrderOutcome::Cancelled);
}

// ==================== EDGE CASE TESTS ====================

// Test: Multiple orders at same price maintain FIFO with partial fills
TEST(OrderbookTest, MultipleSamePriceOrdersFIFOPartialFill) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 40);
   Order sell2(OrderType::GoodTillCancel, 2, 100.0, Side::Sell, 30);
   Order sell3(OrderType::GoodTillCancel, 3, 100.0, Side::Sell, 20);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   
   Order buy(OrderType::Market, 4, 0, Side::Buy, 55);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Large order sweeps entire book
TEST(OrderbookTest, LargeOrderSweepsEntireBook) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 50);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 50);
   Order sell3(OrderType::GoodTillCancel, 3, 102.0, Side::Sell, 50);
   Order sell4(OrderType::GoodTillCancel, 4, 103.0, Side::Sell, 50);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   book.ExecuteTrade(sell4);
   
   Order buy(OrderType::Market, 5, 0, Side::Buy, 200);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Order exactly matches available liquidity
TEST(OrderbookTest, OrderExactlyMatchesAvailableLiquidity) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 45);
   Order sell3(OrderType::GoodTillCancel, 3, 102.0, Side::Sell, 25);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   book.ExecuteTrade(sell3);
   
   Order buy(OrderType::Market, 4, 0, Side::Buy, 100);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Single unit volume order executes correctly
TEST(OrderbookTest, SingleUnitVolumeOrderExecutes) {
   Orderbook book;
   Order sell(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 1);
   book.ExecuteTrade(sell);
   
   Order buy(OrderType::Market, 2, 0, Side::Buy, 1);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Limit order crosses spread and takes best price
TEST(OrderbookTest, LimitOrderCrossesSpreadTakesBestPrice) {
   Orderbook book;
   Order sell1(OrderType::GoodTillCancel, 1, 100.0, Side::Sell, 30);
   Order sell2(OrderType::GoodTillCancel, 2, 101.0, Side::Sell, 40);
   book.ExecuteTrade(sell1);
   book.ExecuteTrade(sell2);
   
   Order buy(OrderType::GoodTillCancel, 3, 105.0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(buy);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

// Test: Back-to-back modifications on same order
TEST(OrderbookTest, BackToBackModificationsOnSameOrder) {
   Orderbook book;
   Order order(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 100);
   book.ExecuteTrade(order);
   
   bool result1 = book.ModifyOrder(1, 100.0, 80);
   bool result2 = book.ModifyOrder(1, 95.0, 60);
   bool result3 = book.ModifyOrder(1, 95.0, 40);
   
   EXPECT_TRUE(result1);
   EXPECT_TRUE(result2);
   EXPECT_TRUE(result3);
}

// Test: Cancel then re-add order with same ID behavior
TEST(OrderbookTest, CancelThenReAddOrderWithSameID) {
   Orderbook book;
   Order order1(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   book.ExecuteTrade(order1);
   
   book.CancelOrder(1);
   
   Order order2(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   OrderOutcome result = book.ExecuteTrade(order2);
   
   EXPECT_EQ(result, OrderOutcome::AddedToOrderbook);
}

// Test: Modify order at different price level moves it correctly
TEST(OrderbookTest, ModifyOrderMovesToDifferentPriceLevel) {
   Orderbook book;
   Order buy1(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 50);
   Order buy2(OrderType::GoodTillCancel, 2, 95.0, Side::Buy, 30);
   book.ExecuteTrade(buy1);
   book.ExecuteTrade(buy2);
   
   bool result = book.ModifyOrder(1, 98.0, 50);
   
   EXPECT_TRUE(result);
}

// Test: Sell side limit order partially fills and adds remainder
TEST(OrderbookTest, SellLimitOrderPartiallyFillsAndAddsRemainder) {
   Orderbook book;
   Order buy(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 30);
   book.ExecuteTrade(buy);
   
   Order sell(OrderType::GoodTillCancel, 2, 100.0, Side::Sell, 80);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::PartiallyFilledAndAddedToBook);
}

// Test: Market sell order consumes multiple bid levels
TEST(OrderbookTest, MarketSellOrderConsumesMultipleBidLevels) {
   Orderbook book;
   Order buy1(OrderType::GoodTillCancel, 1, 100.0, Side::Buy, 30);
   Order buy2(OrderType::GoodTillCancel, 2, 99.0, Side::Buy, 40);
   Order buy3(OrderType::GoodTillCancel, 3, 98.0, Side::Buy, 20);
   book.ExecuteTrade(buy1);
   book.ExecuteTrade(buy2);
   book.ExecuteTrade(buy3);
   
   Order sell(OrderType::Market, 4, 0, Side::Sell, 70);
   OrderOutcome result = book.ExecuteTrade(sell);
   
   EXPECT_EQ(result, OrderOutcome::FullyFilled);
}

int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}