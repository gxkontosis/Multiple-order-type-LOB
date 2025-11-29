#include "OrderBook.h"

OrderOutcome Orderbook::ExecuteTrade(Order& order)
{
   // Quick eligibility check (cheap)
   if ( !CanProcessOrder(order) )
   {
       completedOrders.Add(std::move(order));
       return OrderOutcome::Cancelled;
   }

   // Dispatch based on order type
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

OrderOutcome Orderbook::CleanupOrder(Order& order, const Volume accumulated, const Volume required)
{
   if (accumulated >= required)
   {
      order.SetRemainingVolume(0);
   }
   else
   {
      order.SetRemainingVolume(accumulated);
   }

   if (order.GetRemainingVolume() == 0)
   {
      return OrderOutcome::FullyFilled;
   }
   else
   {
      return OrderOutcome::PartiallyFilledAndCancelled;
   }
}

// Tidy up order, remove it from top of book, move to completed list.
void Orderbook::HandleFilledOrder(std::deque<Order>& queue)
{
   queue.front().SetRemainingVolume(0);
   completedOrders.Add(std::move(queue.front()));
   queue.pop_front();
}

OrderOutcome Orderbook::HandleMarketOrder( Order& order)
{
   Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   Side orderSide = order.GetSide();
   
   if (orderSide == Side::Buy)
   {
      while (!asks.empty() && accumulated < required)
      {
         auto it = asks.begin();
         auto& queue = it->second;

         while (!queue.empty() && accumulated < required) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            asks.erase(it);
      }
   }
   else  // Sell side
   {
      while (!bids.empty() && accumulated < required)
      {
         auto it = bids.begin();
         auto& queue = it->second;

         while (!queue.empty() && accumulated < required) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            bids.erase(it);
      }  
   }
   
   return CleanupOrder(order, accumulated, required);
}

// Same logic as market order, might want to change something here.
OrderOutcome Orderbook::HandleFillOrKill( Order& order)
{
   Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   Side orderSide = order.GetSide();
   
   if (orderSide == Side::Buy)
   {
      while (!asks.empty() && accumulated < required)
      {
         auto it = asks.begin();
         auto& queue = it->second;

         while (!queue.empty() && accumulated < required) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            asks.erase(it);
      }
   }
   else  // Sell side
   {
      while (!bids.empty() && accumulated < required)
      {
         auto it = bids.begin();
         auto& queue = it->second;

         while (!queue.empty() && accumulated < required) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            bids.erase(it);
      }  
   }
   
   return CleanupOrder(order, accumulated, required);
}

OrderOutcome Orderbook::HandleIOC( Order& order)
{
   Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   Price limit = order.GetPrice();
   Side orderSide = order.GetSide();

   if (orderSide == Side::Buy)
   {
      while (accumulated < required && !asks.empty() && limit >= asks.begin()->first)
      {
         auto it = asks.begin();
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            asks.erase(it);
      }
   }
   else  // Sell side
   {
      while (accumulated < required && !bids.empty() && limit <= bids.begin()->first)
      {
         auto it = bids.begin();
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            bids.erase(it);
      }  
   }
   
   return CleanupOrder(order, accumulated, required);
}

OrderOutcome Orderbook::HandleLimitOrder( Order& order)
{
   Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   Price limit = order.GetPrice();
   Side orderSide = order.GetSide();

   // If price requested cannot be filled immediately, add to book.
   if ( orderSide == Side::Buy && (asks.empty() || limit < asks.begin()->first) )
   {
      bids[limit].push_back(order);
      return OrderOutcome::AddedToOrderbook;
   }
   else if ( orderSide == Side::Sell && (bids.empty() || limit > bids.begin()->first) )
   {
      asks[limit].push_back(order);
      return OrderOutcome::AddedToOrderbook;
   }

   //Order can Be filled.
   if (orderSide == Side::Buy)
   {
      while (accumulated < required && !asks.empty() && limit >= asks.begin()->first)
      {
         auto it = asks.begin();
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            asks.erase(it);
      }
   }
   else  // Sell side
   {
      while (accumulated < required && !bids.empty() && limit <= bids.begin()->first)
      {
         auto it = bids.begin();
         auto& queue = it->second;

         while (accumulated < required && !queue.empty()) 
         {
            Volume remaining = required - accumulated;
            accumulated += ConsumeOrderbookEntry(remaining, queue);
         }
         if (queue.empty())
            bids.erase(it);
      }
   }
   
   if (accumulated < required)
   {
      // Add unfilled portion to book
      Order partialOrder = order;
      partialOrder.SetRemainingVolume(required - accumulated);
      if (orderSide == Side::Buy)
         bids[limit].push_back(partialOrder);
      else
         asks[limit].push_back(partialOrder);
      return OrderOutcome::PartiallyFilledAndAddedToBook;
   }
   
   return OrderOutcome::FullyFilled;
}

Volume Orderbook::ConsumeOrderbookEntry(const Volume remaining, std::deque<Order>& queue)
{
   Order& topOfBook = queue.front();
   Volume remainingVolume = topOfBook.GetRemainingVolume();

   // If the order volume is less than what is required, fill the whole order and remove it from queue
   if (remaining >= remainingVolume)
   {
      HandleFilledOrder(queue);
      return remainingVolume;
   }
   else
   {
      Volume leftOver = remainingVolume - remaining;
      topOfBook.SetRemainingVolume(leftOver);
      return remaining;
   }
}

// Checks if all conditions to execute each order are fulfilled.
bool Orderbook::CanProcessOrder(const Order& order) const
{
   // Reject invalid quantities
   if (order.GetInitialQuantity() <= 0)
       return false;

   // Market order cannot execute if opposite side is empty
   if (order.GetType() == OrderType::Market)
   {
      if ((order.GetSide() == Side::Buy && asks.empty()) ||
          (order.GetSide() == Side::Sell && bids.empty()))
      {
          return false;
      }
      return true;
   }

   // FOK requires sufficient volume
   if (order.GetType() == OrderType::FillOrKill)
   {
       if (order.GetSide() == Side::Buy)
           return HasSufficientVolume(order, asks);
       else
           return HasSufficientVolume(order, bids);
   }

   //Reject IOC if requested price is beyond what is offered.
   if (order.GetType() == OrderType::ImmediateOrCancel)
   {
      if (order.GetSide() == Side::Buy)
      {
         if (!asks.empty() && order.GetPrice() > asks.begin()->first)
            return false;
      }
      else // Sell
      {
         if (!bids.empty() && order.GetPrice() < bids.begin()->first)
             return false;
      }
      return true;
   }

   // Limit orders are always valid to process
   return true;
}

// For asks (normal map)
bool Orderbook::HasSufficientVolume(const Order& order,
                                    const std::map<double, std::deque<Order>>& bookSide) const
{
   return HasSufficientBuyVolume(order, bookSide);
}

// For bids (reverse map)
bool Orderbook::HasSufficientVolume(const Order& order,
                                    const std::map<double, std::deque<Order>, std::greater<double>>& bookSide) const
{
   return HasSufficientSellVolume(order, bookSide);
}

bool Orderbook::HasSufficientBuyVolume(
   const Order& order,
   const std::map<double, std::deque<Order>>& asks) const
{
   Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   Price limit = order.GetPrice();

   for (auto it = asks.begin(); it != asks.end(); ++it)
   {
      // Stop if price exceeds our limit
      if (it->first > limit)
         break;

      // Sum volume at this level
      for (const auto& restingOrder : it->second)
         accumulated += restingOrder.GetRemainingVolume();

      if (accumulated >= required)
         return true;
   }

   return false;
}

bool Orderbook::HasSufficientSellVolume(
   const Order& order,
   const std::map<double, std::deque<Order>, std::greater<double>>& bids) const
{
   Volume required = order.GetInitialVolume();
   Volume accumulated = 0;
   Price limit = order.GetPrice();

   for (auto it = bids.begin(); it != bids.end(); ++it)
   {
      // Stop if price is below our limit
      if (it->first < limit)
         break;

      for (const auto& restingOrder : it->second)
         accumulated += restingOrder.GetRemainingVolume();

      if (accumulated >= required)
         return true;
   }

   return false;
}