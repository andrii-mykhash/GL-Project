# Description

Client connets to server, recieve unocupied position of dot, "draw" and can change position. Field with 79x23 dimention. Server accept connections, and inform clients about dots coordinates and amount of connection.

## Client

- Draw field (79,23) with Dots
- Connects to server
- Change Dot coordinates (x,y)
- Recieve Dots position of all clients

## Server

- At client connect, send random unocupied position (x,y)
- Recieve new position and send them to clients
- Inform about quantity of connected clients
- Cannot be collisions: two clients cannot obtain same coordinates [A(12,21) != B(12,21)]

### Example

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
