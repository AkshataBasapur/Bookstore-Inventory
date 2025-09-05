#include <iostream>                 // For standard I/O
#include <cstdlib>                  // For rand(), srand()
#include <ctime>                    // For time() to seed rand()
#include <cstring>                  // For C-style string functions like strcmp, strncpy
#include <limits>                   // For numeric_limits (used to clear input buffer)
using namespace std;

// ----------------------- INTERFACE CLASS FOR BOOK -----------------------
class BookInterface {
public:
    virtual void setDetails() = 0;       // Pure virtual function to set book details
    virtual void showDetails() = 0;      // Pure virtual function to show book details
    virtual ~BookInterface() {}          // Virtual destructor
};

// ----------------------- BOOK CLASS -----------------------
class Book : public BookInterface {
protected:
    string id;           // Book ID
    int price, quantity; // Price and quantity
    string author;       // Author name

public:
    // Set details of the book
    void setDetails() override {
        cout << "Enter Book ID: ";
        cin.ignore();
        getline(cin, id);

        // Input price with validation
        cout << "Enter price: ";
        while (!(cin >> price)) {
            cout << "Invalid input. Enter a number: ";
            cin.clear(); // clear error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ignore bad input
        }

        // Input quantity with validation
        cout << "Enter quantity: ";
        while (!(cin >> quantity)) {
            cout << "Invalid input. Enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Input author name
        cout << "Enter Author Name: ";
        cin.ignore();
        getline(cin, author);
    }

    // Display book details
    void showDetails() override {
        cout << "Book ID: " << id << ", Price: " << price
             << ", Quantity: " << quantity << ", Author: " << author << endl;
    }

    // Getters and Setters
    string getId() { return id; }
    int getQuantity() { return quantity; }
    void setQuantity(int q) { quantity = q; }

    // Copy all book properties from another book
    void copyBook(const Book& b) {
        id = b.id;
        price = b.price;
        quantity = b.quantity;
        author = b.author;
    }
};

// ----------------------- INVENTORY CLASS -----------------------
class Inventory {
    Book books[100]; // Array to store up to 100 books
    int count;       // Number of books in inventory

public:
    Inventory() { count = 0; }

    // Add a new book or update quantity if it already exists
    void addBook() {
        Book newBook;
        newBook.setDetails();

        // Check if book ID already exists, update quantity
        for (int i = 0; i < count; i++) {
            if (books[i].getId() == newBook.getId()) {
                books[i].setQuantity(books[i].getQuantity() + newBook.getQuantity());
                cout << "Book already exists. Quantity updated to " << books[i].getQuantity() << ".\n";
                return;
            }
        }

        // Add new book if space is available
        if (count < 100) {
            books[count] = newBook;
            count++;
            cout << "New book added to inventory.\n";
        } else {
            cout << "Inventory full.\n";
        }
    }

    // Display all books in inventory
    void showInventory() {
        if (count == 0) {
            cout << "Inventory is empty.\n";
            return;
        }
        cout << "Inventory:\n";
        for (int i = 0; i < count; i++) {
            books[i].showDetails();
        }
    }

    // Search book by ID and return index
    int searchBook(string bookId) {
        for (int i = 0; i < count; i++) {
            if (books[i].getId() == bookId)
                return i;
        }
        return -1;
    }

    // Transfer book from inventory to store
    void transferBook(string bookId, Book store[], int& storeCount) {
        int pos = searchBook(bookId);
        if (pos != -1 && books[pos].getQuantity() > 0) {
            int qty;
            cout << "Enter quantity to transfer: ";
            cin >> qty;

            // Validate quantity
            if (qty <= 0 || qty > books[pos].getQuantity()) {
                cout << "Invalid quantity.\n";
                return;
            }

            // Check if book already in store
            for (int i = 0; i < storeCount; i++) {
                if (store[i].getId() == bookId) {
                    store[i].setQuantity(store[i].getQuantity() + qty);
                    books[pos].setQuantity(books[pos].getQuantity() - qty);

                    // Remove book from inventory if quantity becomes zero
                    if (books[pos].getQuantity() == 0) {
                        for (int j = pos; j < count - 1; j++) {
                            books[j] = books[j + 1];
                        }
                        count--;
                    }
                    cout << "Book quantity added to store. Updated inventory.\n";
                    return;
                }
            }

            // Add as new book to store
            store[storeCount].copyBook(books[pos]);
            store[storeCount].setQuantity(qty);
            storeCount++;

            books[pos].setQuantity(books[pos].getQuantity() - qty);
            if (books[pos].getQuantity() == 0) {
                for (int i = pos; i < count - 1; i++) {
                    books[i] = books[i + 1];
                }
                count--;
            }

            cout << "Book transferred to store and inventory updated.\n";
        } else {
            cout << "Book not available.\n";
        }
    }
};

// ----------------------- CART CLASS -----------------------
class Cart {
    Book cartBooks[5]; // Limit cart to 5 books
    int count;

public:
    Cart() { count = 0; }

    // Add a book to cart from store
    void addToCart(Book store[], string bookId, int qty, int storeCount) {
        if (count >= 5) {
            cout << "Cart is full.\n";
            return;
        }

        // Check if book already in cart
        for (int i = 0; i < count; i++) {
            if (cartBooks[i].getId() == bookId) {
                cout << "Book already in cart.\n";
                return;
            }
        }

        // Search book in store
        for (int i = 0; i < storeCount; i++) {
            if (store[i].getId() == bookId) {
                if (store[i].getQuantity() == 0) {
                    cout << "Book is not available in store.\n";
                    return;
                } else if (store[i].getQuantity() >= qty) {
                    cartBooks[count].copyBook(store[i]);
                    cartBooks[count].setQuantity(qty);
                    store[i].setQuantity(store[i].getQuantity() - qty);
                    count++;
                    cout << "Book added to cart.\n";
                    return;
                } else {
                    cout << "Only " << store[i].getQuantity() << " available in store.\n";
                    return;
                }
            }
        }

        cout << "Book not found in store.\n";
    }

    // View all books in cart
    void viewCart() {
        if (count == 0) {
            cout << "Cart is empty.\n";
            return;
        }

        cout << "Cart:\n";
        for (int i = 0; i < count; i++) {
            cartBooks[i].showDetails();
        }
    }

    // Complete purchase
    void buy() {
        if (count == 0)
            cout << "Cart is empty. Nothing to buy.\n";
        else {
            cout << "Books purchased successfully.\n";
            count = 0; // clear cart after purchase
        }
    }
};

// ----------------------- USER INTERFACE (ABSTRACT) -----------------------
class UserInterface {
public:
    virtual void login() = 0;
    virtual void mainMenu() = 0;
    virtual ~UserInterface() {}
};

// ----------------------- USER CLASS -----------------------
class User {
protected:
    char username[20];   // C-style username
    char phone[15];      // Phone number
    char password[10];   // Password

public:
    // Generate random 6-letter CAPTCHA
    void generateCaptcha(char cap[]) {
        srand(time(0));
        for (int i = 0; i < 6; i++)
            cap[i] = 'A' + rand() % 26;
        cap[6] = '\0';
    }
};

// ----------------------- BOOKSHOP CLASS (MAIN APP) -----------------------
class Bookshop : public User, public UserInterface {
    Inventory inventory;
    Book store[10]; // Store holds books from inventory (limit 10)
    int storeCount;

public:
    Bookshop() { storeCount = 0; }

    // User login with username, password, phone and CAPTCHA
    void login() override {
        cout << "Enter username: ";
        cin.ignore();
        cin.getline(username, 20);

        cout << "Enter password: ";
        cin >> password;

        const char correctPass[] = "admin123"; // predefined password
        if (strcmp(password, correctPass) != 0) {
            cout << "Incorrect password. Access denied.\n";
            exit(0);
        }

        // Validate phone number
        int valid = 0;
        while (!valid) {
            cout << "Enter phone number (starts with 6/7/8/9 and 10 digits): ";
            cin >> phone;
            int len = strlen(phone);
            if (len == 10 && (phone[0] == '6' || phone[0] == '7' || phone[0] == '8' || phone[0] == '9')) {
                valid = 1;
            } else {
                cout << "Invalid phone number.\n";
            }
        }

        // CAPTCHA verification
        char captcha[7];
        generateCaptcha(captcha);
        cout << "Captcha: " << captcha << endl;
        char entered[7];
        cout << "Enter the above captcha: ";
        cin >> entered;

        if (strncmp(entered, captcha, 6) != 0) {
            cout << "Captcha mismatch. Access denied.\n";
            exit(0);
        }

        cout << "User authenticated.\n";
    }

    // Owner functionalities
    void ownerMenu() {
        int ch;
        do {
            cout << "\nOwner Menu:\n1. Add Book\n2. Show Inventory\n3. Transfer to Store\n4. Back\nEnter choice: ";
            cin >> ch;

            if (ch == 1)
                inventory.addBook();
            else if (ch == 2)
                inventory.showInventory();
            else if (ch == 3) {
                if (storeCount < 10) {
                    string bookId;
                    cout << "Enter Book ID to transfer: ";
                    cin.ignore();
                    getline(cin, bookId);
                    inventory.transferBook(bookId, store, storeCount);
                } else {
                    cout << "Store is full.\n";
                }
            }
        } while (ch != 4);
    }

    // Customer functionalities
    void customerMenu() {
        Cart cart;
        int ch;
        do {
            cout << "\nCustomer Menu:\n1. Search Book\n2. Add to Cart\n3. View Cart\n4. Buy\n5. Back\nEnter choice: ";
            cin >> ch;

            if (ch == 1) {
                if (storeCount == 0) {
                    cout << "The store is empty.\n";
                    continue;
                }

                string id;
                bool found = false;
                cout << "Enter Book ID to search: ";
                cin.ignore();
                getline(cin, id);
                for (int i = 0; i < storeCount; i++) {
                    if (store[i].getId() == id) {
                        store[i].showDetails();
                        found = true;
                        break;
                    }
                }
                if (!found)
                    cout << "Book not found in store.\n";
            } else if (ch == 2) {
                string id;
                int qty;
                cout << "Enter Book ID: ";
                cin.ignore();
                getline(cin, id);
                cout << "Enter Quantity: ";
                cin >> qty;
                cart.addToCart(store, id, qty, storeCount);
            } else if (ch == 3) {
                cart.viewCart();
            } else if (ch == 4) {
                cart.buy();
            }
        } while (ch != 5);
    }

    // Main menu for selecting role
    void mainMenu() override {
        int choice;
        do {
            cout << "\nWelcome to Bookshop\n1. Owner\n2. Customer\n3. Exit\nEnter choice(number): ";
            cin >> choice;

            if (choice == 1)
                ownerMenu();
            else if (choice == 2)
                customerMenu();
        } while (choice != 3);
    }
};

// ----------------------- MAIN FUNCTION -----------------------
int main() {
    UserInterface* user = new Bookshop(); // Create Bookshop instance via interface pointer
    user->login();     // Login process
    user->mainMenu();  // Display main menu
    delete user;       // Clean up
    return 0;
}
