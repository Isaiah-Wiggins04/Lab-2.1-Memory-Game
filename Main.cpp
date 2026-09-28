#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#ifdef USE_NCURSES
#include <ncurses.h>
#endif

#ifdef USE_SFML
#include <SFML/Graphics.hpp>
#endif

struct Card {
    char value;
    bool isFaceUp;
    bool isMatched;

    Card(char val) : value(val), isFaceUp(false), isMatched(false) {}
};

struct Node {
    Card data;
    Node* next;

    Node(Card c) : data(c), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
    int length;

public:
    LinkedList() : head(nullptr), length(0) {}

    ~LinkedList() {
        Node* current = head;
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }

    void insert(Card card) {
        Node* newNode = new Node(card);
        if (!head) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
        length++;
    }

    Card& getAt(int index) {
        Node* temp = head;
        for (int i = 0; i < index; i++) temp = temp->next;
        return temp->data;
    }

    void removeAt(int index) {
        if (index < 0 || index >= length) return;
        if (index == 0) {
            Node* temp = head;
            head = head->next;
            delete temp;
        } else {
            Node* prev = head;
            for (int i = 0; i < index - 1; i++) prev = prev->next;
            Node* temp = prev->next;
            prev->next = temp->next;
            delete temp;
        }
        length--;
    }

    void clear() {
        while (length > 0) removeAt(0);
    }

    int size() const { return length; }
};

class MemoryGame {
private:
    LinkedList cards;
    int rows, cols;
    int firstFlipIndex, secondFlipIndex;
    int moves;

    void initializeDeck() {
        char values[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        for (int i = 0; i < 8; i++) {
            cards.insert(Card(values[i]));
            cards.insert(Card(values[i]));
        }
    }

    void shuffle() {
        std::vector<Card> temp;
        for (int i = 0; i < cards.size(); i++) {
            temp.push_back(cards.getAt(i));
        }

        for (int i = temp.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            Card holder = temp[i];
            temp[i] = temp[j];
            temp[j] = holder;
        }

        cards.clear();
        for (int i = 0; i < temp.size(); i++) {
            cards.insert(temp[i]);
        }
    }

public:
    MemoryGame(int r = 4, int c = 4) : rows(r), cols(c) {
        firstFlipIndex = -1;
        secondFlipIndex = -1;
        moves = 0;
        initializeDeck();
        shuffle();
    }

    bool flipCard(int row, int col) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) return false;
        int index = row * cols + col;
        if (cards.getAt(index).isFaceUp || cards.getAt(index).isMatched) return false;

        cards.getAt(index).isFaceUp = true;

        if (firstFlipIndex == -1) {
            firstFlipIndex = index;
            return false;
        }
        secondFlipIndex = index;
        return true;
    }

    bool allMatched() {
        for (int i = 0; i < cards.size(); i++) {
            if (!cards.getAt(i).isMatched) return false;
        }
        return true;
    }

    bool checkMatch() {
        if (firstFlipIndex == -1 || secondFlipIndex == -1) return false;
        Card& c1 = cards.getAt(firstFlipIndex);
        Card& c2 = cards.getAt(secondFlipIndex);
        moves++;
        if (c1.value == c2.value) {
            c1.isMatched = true;
            c2.isMatched = true;
        } else {
            c1.isFaceUp = false;
            c2.isFaceUp = false;
        }
        firstFlipIndex = -1;
        secondFlipIndex = -1;
        return allMatched();
    }

    int getMoves() { return moves; }

    void displayTerminal() {
        std::cout << "\nMemory Game Grid:\n";
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int idx = r * cols + c;
                Card& card = cards.getAt(idx);
                if (card.isMatched) {
                    std::cout << "[ ] ";
                } else if (card.isFaceUp) {
                    std::cout << "[" << card.value << "] ";
                } else {
                    std::cout << "[*] ";
                }
            }
            std::cout << "\n";
        }
    }

    void playTerminal() {
        while (!allMatched()) {
            displayTerminal();
            int row, col;
            std::cout << "Enter row and col (0 to 3): ";
            if (!(std::cin >> row >> col)) return;

            if (flipCard(row, col)) {
                displayTerminal();
                std::string pause;
                std::cout << "Type anything and press enter to continue: ";
                std::cin >> pause;
                checkMatch();
            }
        }
        std::cout << "You win! Moves: " << moves << "\n";
    }

#ifdef USE_NCURSES
    void drawNcurses(int cursorRow, int cursorCol) {
        clear();
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int idx = r * cols + c;
                Card& card = cards.getAt(idx);
                int x = c * 4;
                int y = r * 2;

                if (r == cursorRow && c == cursorCol) {
                    attron(A_BOLD | A_REVERSE);
                }

                if (card.isMatched) {
                    if (has_colors()) attron(COLOR_PAIR(3));
                    mvprintw(y, x, "[ ]");
                    if (has_colors()) attroff(COLOR_PAIR(3));
                } else if (card.isFaceUp) {
                    if (has_colors()) attron(COLOR_PAIR(2));
                    mvprintw(y, x, "[%c]", card.value);
                    if (has_colors()) attroff(COLOR_PAIR(2));
                } else {
                    if (has_colors()) attron(COLOR_PAIR(1));
                    mvprintw(y, x, "[*]");
                    if (has_colors()) attroff(COLOR_PAIR(1));
                }

                attroff(A_BOLD | A_REVERSE);
            }
        }
        mvprintw(rows * 2 + 1, 0, "Arrows move, SPACE flips, Q quits.");
        refresh();
    }

    void displayNcurses() {
        if (initscr() == NULL) {
            std::cout << "Could not start ncurses.\n";
            return;
        }
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);
        if (has_colors()) {
            start_color();
            init_pair(1, COLOR_WHITE, COLOR_BLUE);
            init_pair(2, COLOR_BLACK, COLOR_WHITE);
            init_pair(3, COLOR_GREEN, COLOR_BLACK);
        }

        int cursorRow = 0;
        int cursorCol = 0;
        bool quit = false;

        while (!allMatched() && !quit) {
            drawNcurses(cursorRow, cursorCol);
            int ch = getch();

            if (ch == KEY_UP) {
                if (cursorRow > 0) cursorRow--;
            } else if (ch == KEY_DOWN) {
                if (cursorRow < rows - 1) cursorRow++;
            } else if (ch == KEY_LEFT) {
                if (cursorCol > 0) cursorCol--;
            } else if (ch == KEY_RIGHT) {
                if (cursorCol < cols - 1) cursorCol++;
            } else if (ch == ' ' || ch == '\n') {
                if (flipCard(cursorRow, cursorCol)) {
                    drawNcurses(cursorRow, cursorCol);
                    napms(1000);
                    checkMatch();
                }
            } else if (ch == 'q' || ch == 'Q') {
                quit = true;
            }
        }

        if (allMatched()) {
            clear();
            mvprintw(rows, 0, "You Win! Moves: %d. Press any key.", moves);
            refresh();
            getch();
        }
        endwin();
    }
#endif

#ifdef USE_SFML
    void drawSFML(sf::RenderWindow& window, sf::Font& font, bool showWin) {
        window.clear();
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int idx = r * cols + c;
                Card& card = cards.getAt(idx);

                if (card.isMatched) continue;

                sf::RectangleShape rect(sf::Vector2f(90, 90));
                rect.setPosition(c * 100 + 5, r * 100 + 5);
                if (card.isFaceUp) {
                    rect.setFillColor(sf::Color::White);
                } else {
                    rect.setFillColor(sf::Color::Blue);
                }
                window.draw(rect);

                if (card.isFaceUp) {
                    sf::Text text(std::string(1, card.value), font, 50);
                    text.setPosition(c * 100 + 30, r * 100 + 20);
                    text.setFillColor(sf::Color::Black);
                    window.draw(text);
                }
            }
        }

        if (showWin) {
            sf::Text winText("You Win!", font, 50);
            winText.setPosition(100, 180);
            winText.setFillColor(sf::Color::White);
            window.draw(winText);
        }
        window.display();
    }

    void displaySFML() {
        sf::RenderWindow window(sf::VideoMode(cols * 100, rows * 100), "Memory Game");
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cout << "Could not load arial.ttf. Put it in the same folder.\n";
            return;
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    int x = event.mouseButton.x / 100;
                    int y = event.mouseButton.y / 100;
                    if (flipCard(y, x)) {
                        drawSFML(window, font, false);
                        sf::sleep(sf::seconds(1));
                        checkMatch();
                    }
                }
            }

            if (allMatched()) {
                drawSFML(window, font, true);
                sf::sleep(sf::seconds(3));
                window.close();
            } else {
                drawSFML(window, font, false);
            }
        }
    }
#endif
};

int main(int argc, char* argv[]) {
    srand(time(0));
    MemoryGame game;

    std::string mode = "terminal";
    if (argc > 1) mode = argv[1];

    if (mode == "terminal") {
        game.playTerminal();
    } else if (mode == "ncurses") {
#ifdef USE_NCURSES
        game.displayNcurses();
#else
        std::cout << "Compile with -DUSE_NCURSES -lncurses to use this mode.\n";
#endif
    } else if (mode == "sfml") {
#ifdef USE_SFML
        game.displaySFML();
#else
        std::cout << "Compile with -DUSE_SFML and the SFML libraries to use this mode.\n";
#endif
    } else {
        std::cout << "Use: ./game terminal, ./game ncurses, or ./game sfml\n";
    }

    return 0;
}