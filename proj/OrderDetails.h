#include <vector>

#include "Side.h"

using Price = double;
using ID = double;
using Volume = double;
using Quantity = double;

enum class OrderType
{
   GoodTillCancel,
   ImmediateOrCancel,
   FillOrKill,
   Market,
};

enum class OrderOutcome
{
   FullyFilled,
   PartiallyFilledAndCancelled,
   PartiallyFilledAndAddedToBook,
   Cancelled,
   AddedToOrderbook
};

struct OrderLocation
{
   Price price;
   Side side;
};