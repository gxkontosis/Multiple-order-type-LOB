Limit Order Book Engine

A C++ implementation of a limit order book matching engine supporting multiple order types and standard exchange operations.

Overview
This project implements a limit order book (LOB) that matches buy and sell orders according to price-time priority. The engine supports four order types commonly used in financial exchanges and provides O(1) order modification and cancellation through efficient data structure design.

Order Types
Market Orders: Execute immediately at the best available price, consuming liquidity across multiple price levels
Limit Orders (Good-Till-Cancel): Execute at specified price or better, with unfilled portions resting in the book
Immediate-or-Cancel (IOC): Execute immediately up to the limit price, cancelling any unfilled portion
Fill-or-Kill (FOK): Execute the entire order immediately or cancel if insufficient volume exists
