# Description

Client connects to the server, receive unoccupied position of dot, draw field and another info, and may change position. Field could be 80x24 dimension. Server accepts connections, and inform clients about dots coordinates and amount of connection.

## Client

- Connects to server
- Receive Dots position of all clients
- Draw field (80,24) with all users
- Change Dot coordinates (x,y)

## Server

- After client connect to server, send random unoccupied position (x,y)
- Receive new position and send them to clients
- Inform about quantity of connected clients
- Cannot be collisions: two clients cannot obtain same coordinates [A(12,21) != B(12,21)]

## Example

Field (4x10)

```text
+-----------+
|  *  *-->* |
|  ^        |
|  |        |
|  *        |
+-----------+
Connected: 2
Move: [w,a,s,d] or [arrow up, down, left, right]
```
