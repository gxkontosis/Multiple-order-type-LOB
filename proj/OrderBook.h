#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <map>
#include <deque>
#include <unordered_map>
#include <iostream>

#include "CompletedOrders.h"

class Orderbook
{
public:

   OrderOutcome ExecuteTrade(Order& order);
   ID GetNextOrderId() { return ++nextOrderID; }  // Helper to generate IDs
   
   bool ModifyOrder(ID orderID, Price newPrice, Volume newVolume);
   bool ModifyVolume(Order& order, Volume newVolume);
   bool CancelOrder(ID orderID);

   // I also want to add: current level - price last trade took place at and remaining cash on that level
   // Modify/Cancel order
   // Order history.
   // Unit tests.

private:
   static ID nextOrderID;
   
   std::map<Price, std::deque<Order>> asks;
   std::map<Price, std::deque<Order>, std::greater<Price>> bids;
   std::unordered_map<ID, OrderLocation> orderbookReference;

   CompletedOrders completedOrders;

   Volume ConsumeOrderbookEntry(const Volume remaining, std::deque<Order>& queue);
   void HandleFilledOrder(std::deque<Order>& queue);
   bool CanProcessOrder(const Order& order) const;
   
   // Two overloads for different map types
   bool HasSufficientVolume(const Order& order, const std::map<double, std::deque<Order>>& bookSide) const;
   bool HasSufficientVolume(const Order& order, const std::map<double, std::deque<Order>, std::greater<double>>& bookSide) const;
   
   bool HasSufficientBuyVolume(const Order& order, const std::map<double, std::deque<Order>>& asks) const;
   bool HasSufficientSellVolume(const Order& order, const std::map<double, std::deque<Order>, std::greater<double>>& bids) const;
   
   OrderOutcome HandleMarketOrder( Order& order);
   OrderOutcome HandleLimitOrder( Order& order);
   OrderOutcome HandleFillOrKill( Order& order);
   OrderOutcome HandleIOC( Order& order);

   OrderOutcome CleanupOrder(Order& order, const Volume accumulated, const Volume required);
};

#endif