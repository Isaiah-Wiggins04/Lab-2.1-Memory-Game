# Report

## Part 1 Questions

**Q1. Why are linked lists inefficient for random access in games? Why is a linked list not ideal for grid-based games?**

A linked list has no direct access by index. Every time the game needs a card at a grid position, getAt walks from the head one node at a time, so it takes O(n) time. An array or vector jumps straight to any position in O(1) time. A grid game accesses cards by position constantly (every flip, every match check, every redraw), so the list does a lot of extra work. With 16 cards it is fast enough, but it would get slow on a larger grid.

**Q2. What challenges came up while building the game?**

- Shuffling a linked list directly is hard. I copied the cards to a vector, shuffled the vector, cleared the list, and inserted the cards again.
- The sample code checked for a match right after the second flip, so a wrong pair was never visible. I draw the board first, wait a moment, and then call checkMatch.
- Matched cards are marked instead of removed, so the grid positions stay the same.

## Part 2 Questions

**Q3. What are the pros and cons of ncurses compared to cout?**

Pros:
- Keyboard input works without pressing enter.
- It supports colors and a movable cursor.
- It redraws the screen in place instead of printing a new grid each time.

Cons:
- It needs extra setup (initscr, endwin, and a linked library).
- The terminal can be left broken if the program exits without calling endwin.

**Q4. How does the efficiency of ncurses redraws compare to using a vector?**

Each ncurses redraw calls getAt once for every card. Because getAt walks from the head, drawing card number 15 takes 15 steps. Drawing all 16 cards takes about 0 + 1 + 2 + ... + 15 = 120 steps per frame. A vector would take only 16 steps, one per card. This is small enough not to notice with 16 cards, but it would get slow on a larger grid.

**Q5. How does ncurses improve the user experience over plain cout?**

With cout the player has to type a row and column and press enter, and a new grid prints below the old one every turn. With ncurses the player moves a highlighted cursor with the arrow keys and flips with one key press. The board stays in one place and updates in place, and colors make hidden, face up, and matched cards easy to tell apart.

**Q6. Why is cleanup with endwin() important?**

ncurses changes how the terminal behaves (no echo, hidden cursor, special key handling). Calling endwin() puts the terminal back to normal. If the program ends without it, the terminal can be left with no visible typing or a broken display.

**Q7. Are there memory leaks? (valgrind)**

Result of valgrind: 