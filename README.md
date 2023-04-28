# CS214 - tictactoe (Jack Lin - jjl327)
## Implementation Notes
### Design
I implemented a server that could manage at most one active game at a time. This means that there are at most 2 clients connecting to the server and that no additional clients can register. Additionally, since there are only 2 clients, the server can always predict which client will send the next message.
### Helpful Notes
I used this simple model below to implement the read() and write() functions within the server and the client code. The trickiest part about this whole process was that in order for all parties to keep the same message logs, client 1 and client 2 had to read() and write() at different times.

**Server:**
- receive from client 1
- send to client 1
- send to client 2
- receive from client 2
- sent to client 2
- send to client 1

**Client 1:**
- send to server
- receive from server (action confirmation)
- receive from server (player 2's actions)

**Client 2:**
- receive from server (player 1's actions)
- send to server
- receive from server (action confirmation)

Since tic-tac-toe is turn-based, player 1 and player 2 must take their turn at different times, leading to many conditional blocks in the code.

## Useful Functions
### ttts.c
**interpret()**
This function is the glue that holds the server together. It manages many things including:
1. Reading messages from the clients and then reacting accordingly
2. Filling in the buffer with an appropriate message to send back to the clients
3. To control the "success" variable which controlled conditional statements within the main loop of ttts

### ttt.c
**action()**

This function mainly allows the user to take their turn with different choices:
1. MOVE
2. RSGN
3. DRAW


The move function was the trickiest to implement since I had to implement a do-while loop in the main function to keep track whether or not the user has played a legal move.

**reaction()**

This function is mostly for controlling the conditional clauses in the main function. It is used to react to messages incoming from the server.
## Error Testing
Testing this server-client program was unusual, since multiple terminals were needed. But it was also simple to do since the only thing I really needed to do was play multiple games and test edge-cases. There were multiple scenarios that I tested:

1. Victories (All rows, cols, and diagonals)
2. Draws (No more empty spaces)
3. Agreed Draws (Through requests)
4. Draw Rejections
5. Resignations
