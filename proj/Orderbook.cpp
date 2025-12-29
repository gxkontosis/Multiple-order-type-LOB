#include "OrderBook.h"

ID Orderbook::nextOrderID = 0;

// Description: Main entry point for processing orders, validates and 
// routes to appropriate handler based on order type.
OrderOutcome Orderbook::ExecuteTrade(Order& order)
{
   if ( !CanProcessOrder(order) )
   {
       completedOrders.Add(std::move(order));
       return OrderOutcome::Cancelled;
   }

   switch (order.GetType())
   {
      case OrderType::Market:
         return HandleMarketOrder(order);

      case OrderType::FillOrKill:
         return HandleFillOrKill(order);

      case OrderType::ImmediateOrCancel:
         return HandleIOC(order);

      case OrderType::GoodTillCancel:
         return HandleLimitOrder(order);

      default:
         return OrderOutcome::Cancelled;
   }
}

// Description: Modifies an existing resting order's price and volume, 
// maintaining price-time priority on volume decrease.
bool Orderbook::ModifyOrder(const ID orderID, const Price newPrice, const Volume newVolume)
{
   if (newVolume <= 0)
   {
      return CancelOrder(orderID);
   }

   auto refIt = orderbookReference.find(orderID);

   if (refIt == orderbookReference.end())
      return false;

   const Price oldPrice = refIt->second.price;
   const Side side = refIt->second.side;

   if ( side == Side::Buy )
   {
      for (auto it = bids[oldPrice].begin(); it != bids[oldPrice].end(); ++it)
      {
         if ( it->GetId() == orderID )
         {
            ModifyVolume( *it, newVolume);
         
            if (oldPrice != newPrice)
            {
               bids[newPrice].push_back(std::move(*it));
               (void)bids[oldPrice].erase(it);
               refIt->second.price = newPrice;
            }
            break;
         }
      }
   }
   else
   {
      for (auto it = asks[oldPrice].begin(); it != asks[oldPrice].end(); ++it)
      {
         if ( it->GetId() == orderID )
         {
            ModifyVolume( *it, newVolume);
         
            if (oldPrice != newPrice)
            {
               asks[newPrice].push_back(std::move(*it));
               (void)asks[oldPrice].erase(it);
               refIt->second.price = newPrice;
            }
            break;
         }
      }
   }

   return true;
}

// Description: Updates an order's remaining volume, rejecting increases 
// to maintain queue priority fairness.
bool Orderbook::ModifyVolume(Order& order, const Volume newVolume)
{
   if (newVolume > order.GetRemainingVolume())
   {
      return false;
   }

   if (newVolume < order.GetRemainingVolume())
   {
      order.SetRemainingVolume(newVolume);
      return true;
   }

   return true;
}

// Description: Removes an order from the orderbook and reference map.
bool Orderbook::CancelOrder(const ID orderID)
{
   auto refIt = orderbookReference.find(orderID);

   if (refIt == orderbookReference.end())
      return false;

   const Price price = refIt->second.price;
   const Side side = refIt->second.side;

   if ( side == Side::Buy )
   {
      for (auto it = bids[price].begin(); it != bids[price].end(); ++it)
      {
         if ( it->GetId() == orderID )
         {
            orderbookReference.erase(refIt);
            (void)bids[price].erase(it);
            break;
         }
      }
   }
   else
   {
      for (auto it = asks[price].begin(); it != asks[price].end(); ++it)
      {
         if ( it->GetId() == orderID )
         {
            orderbookReference.erase(refIt);
            (void)asks[price].erase(it);
            break;
         }
      }
   }
   return true;
}

// Description: Finalizes order processing by updating remaining volume 
// and moving to completed orders list.
OrderOutcome Orderbook::CleanupOrder(Order& order, const Volume accumulated, const Volume required)
{
   if (accumulated >= required)
   {
      order.SetRemainingVolume(0);
   }
   else
   {
      order.SetRemainingVolume(required - accumulated);
   }

   if (order.GetRemainingVolume() == 0)
   {
      completedOrders.Add(std::move(order));
      return OrderOutcome::FullyFilled;
   }
   else
   {
      completedOrders.Add(std::move(order));
      return OrderOutcome::PartiallyFilledAndCancelled;
   }
}

// Description: Removes fully filled order from queue, updates reference 
// map, and adds to completed orders.
void Orderbook::HandleFilledOrder(std::deque<Order>& queue)
{
   Order order = std::move(queue.front());
   orderbookReference.erase(order.GetId());

   order.SetRemainingVolume(0);
   completedOrders.Add(std::move(order));
   queue.pop_front();   
}

// Description: Executes market order by consuming liquidity across all 
// available price levels until filled or exhausted.
OrderOutcome Orderbook::HandleMarketOrder(Order& order)
{
   const Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   const Side orderSide = order.GetSide();
   Volume remaining = 0;
   
   if (orderSide == Side::Buy)
   {
      auto it = asks.begin();

      while (!asks.empty() && accumulated < required) 
      {
         auto& queue = it->second;

         while (!queue.empty() && accumulated < required) 
         {
            remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
   
         if (queue.empty())
            it = asks.erase(it);
      }
   }
   else
   {
      auto it = bids.begin();
      while (!bids.empty() && accumulated < required)
      {
         auto& queue = it->second;

         while (!queue.empty() && accumulated < required) 
         {
            remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            it = bids.erase(it);
      }  
   }
   
   return CleanupOrder(order, accumulated, required);
}

// Description: Executes Fill-or-Kill order atomically after validation 
// confirms sufficient volume exists.
OrderOutcome Orderbook::HandleFillOrKill(Order& order)
{
   return HandleMarketOrder(order);
}

// Description: Executes Immediate-or-Cancel order up to the limit price,
// cancelling any unfilled portion.
OrderOutcome Orderbook::HandleIOC(Order& order)
{
   const Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   const Price limit = order.GetPrice();
   const Side orderSide = order.GetSide();
   Volume remaining = 0;

   if (orderSide == Side::Buy)
   {
      auto it = asks.begin();

      while (accumulated < required && !asks.empty() && limit >= it->first)
      {         
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if ( queue.empty() )
            it = asks.erase(it);
      }
   }
   else
   {
      auto it = bids.begin();

      while (accumulated < required && !bids.empty() && limit <= it->first)
      {         
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if ( queue.empty() )
            it = bids.erase(it);
      }  
   }   
   return CleanupOrder(order, accumulated, required);
}

// Description: Executes limit order, immediately filling at available 
// prices or adding remainder to book at specified price.
OrderOutcome Orderbook::HandleLimitOrder(Order& order)
{
   const Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   const Price limit = order.GetPrice();
   const Side orderSide = order.GetSide();

   if (orderSide == Side::Buy && (asks.empty() || limit < asks.begin()->first))
   {
      orderbookReference[order.GetId()] = {limit, orderSide};
      bids[limit].push_back(std::move(order));
      return OrderOutcome::AddedToOrderbook;
   }
   else if (orderSide == Side::Sell && (bids.empty() || limit > bids.begin()->first))
   {
      orderbookReference[order.GetId()] = {limit, orderSide};
      asks[limit].push_back(std::move(order));
      return OrderOutcome::AddedToOrderbook;
   }

   if (orderSide == Side::Buy)
   {
      auto it = asks.begin();

      while (accumulated < required && !asks.empty() && limit >= it->first)
      {
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            const Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
           it = asks.erase(it);
      }
   }
   else
   {
      auto it = bids.begin();

      while (accumulated < required && !bids.empty() && limit <= it->first)
      {         
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            const Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            it = bids.erase(it);
      }
   }
   
   if (accumulated < required)
   {
      order.SetRemainingVolume(required - accumulated);
      orderbookReference[order.GetId()] = {limit, orderSide};

      if (orderSide == Side::Buy)      
         bids[limit].push_back(std::move(order));
      else
         asks[limit].push_back(std::move(order));
      return OrderOutcome::PartiallyFilledAndAddedToBook;
   }
   
   completedOrders.Add(std::move(order));
   return OrderOutcome::FullyFilled;
}

// Description: Matches incoming order against top-of-book resting 
// order, consuming available volume.
Volume Orderbook::ConsumeOrderbookEntry(const Volume toBeFilledVolume, std::deque<Order>& queue)
{
   Order& topOfBook = queue.front();
   const Volume topOfBookVolume = topOfBook.GetRemainingVolume();

   if (toBeFilledVolume >= topOfBookVolume)
   {
      HandleFilledOrder(queue);
      return topOfBookVolume;
   }
   else
   {
      const Volume leftOver = topOfBookVolume - toBeFilledVolume;
      topOfBook.SetRemainingVolume(leftOver);
      return toBeFilledVolume;
   }
}

// Description: Validates order eligibility by checking volume, 
// available liquidity, and order-type-specific requirements.
bool Orderbook::CanProcessOrder(const Order& order) const
{
   if (order.GetInitialVolume() <= 0)
       return false;

   if (order.GetType() == OrderType::Market)
   {
      if ((order.GetSide() == Side::Buy && asks.empty()) ||
          (order.GetSide() == Side::Sell && bids.empty()))
      {
          return false;
      }
   }

   if (order.GetType() == OrderType::FillOrKill)
   {
       if (order.GetSide() == Side::Buy)
           return HasSufficientVolume(order, asks);
       else
           return HasSufficientVolume(order, bids);
   }

   if (order.GetType() == OrderType::ImmediateOrCancel)
   {
      if (order.GetSide() == Side::Buy)
      {
         if (!asks.empty() && order.GetPrice() < asks.begin()->first)
            return false;
      }
      else
      {
         if (!bids.empty() && order.GetPrice() > bids.begin()->first)
             return false;
      }
   }

   return true;
}

// Description: Overload for asks map to check if sufficient
// volume exists for Fill-or-Kill buy orders.
bool Orderbook::HasSufficientVolume(const Order& order,
                                    const std::map<double, std::deque<Order>>& bookSide) const
{
   return HasSufficientBuyVolume(order, bookSide);
}

// Description: Overload for bids map to check if sufficient volume exists for Fill-or-Kill sell orders.
bool Orderbook::HasSufficientVolume(const Order& order,
                                    const std::map<double, std::deque<Order>, std::greater<double>>& bookSide) const
{
   return HasSufficientSellVolume(order, bookSide);
}

// Description: Calculates total available volume in asks up to 
// limit price for Fill-or-Kill validation.
bool Orderbook::HasSufficientBuyVolume(
   const Order& order,
   const std::map<double, std::deque<Order>>& asks) const
{
   const Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   const Price limit = order.GetPrice();

   for (auto it = asks.begin(); it != asks.end(); ++it)
   {
      if (it->first > limit)
         break;

      for (const auto& restingOrder : it->second)
         accumulated += restingOrder.GetRemainingVolume();

      if (accumulated >= required)
         return true;
   }
   return false;
}

// Description: Calculates total available volume in bids down to limit price for Fill-or-Kill validation.
bool Orderbook::HasSufficientSellVolume(
   const Order& order,
   const std::map<double, std::deque<Order>, std::greater<double>>& bids) const
{
   const Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   const Price limit = order.GetPrice();

   for (auto it = bids.begin(); it != bids.end(); ++it)
   {
      if (it->first < limit)
         break;

      for (const auto& restingOrder : it->second)
         accumulated += restingOrder.GetRemainingVolume();

      if (accumulated >= required)
         return true;
   }
   return false;
}