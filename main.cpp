// Include necessary header files for input/output operations
#include <iostream>  // For console input/output operations (cin, cout, endl)
#include <fstream>   // For file input/output operations (ifstream, ofstream, file.close())
#include <cstring>   // For C-style string manipulation functions like strcpy, strcmp, strstr, strlen
#include <cctype>    // For character handling functions like tolower for case conversion

/*
  Function: toLower
  Purpose: Converts all characters in a C-style string to lowercase for case-insensitive operations
  Parameters: char* str - pointer to the string to be converted (modified in-place)
  Algorithm: Iterates through each character and applies tolower() function from cctype
  Used for: Case-insensitive string comparisons in search operations
 */
void toLower(char* str) {
    for (int i = 0; str[i]; i++) {  // Loop until null terminator is reached
        str[i] = tolower(str[i]);   // Convert each character to lowercase
    }
}

// Use the standard namespace to avoid prefixing std:: before every standard library element
// This simplifies code readability but should be used carefully in larger projects
using namespace std;

// Define global constants for memory management and data validation
const int MAX_STRING_LENGTH = 100;  // Maximum length for strings (title, author, category) - chosen to handle most book titles
const int MAX_BOOKS = 10000;        // Maximum number of books the library can hold - reasonable limit for memory management

/*
  Structure: Book
  Purpose: Represents a single book record with all essential attributes
  Design Decision: Using struct instead of class for simple data container with public access
  Memory Layout: Fixed-size character arrays for predictable memory usage and file I/O
 */
struct Book {
    int id;                           // Unique identifier for each book (auto-incrementing)
    char title[MAX_STRING_LENGTH];    // Title of the book (fixed-size array for consistent file storage)
    char author[MAX_STRING_LENGTH];   // Author name (supports single author - could be extended for multiple authors)
    char category[MAX_STRING_LENGTH]; // Category/genre of the book (single classification system)
    bool available;                   // Availability status (true = available for checkout, false = currently checked out)
    
    /*
      Method: display
      Purpose: Formats and displays all book information in a user-friendly format
      Design: Uses consistent formatting with separators for visual clarity
      Output: Formatted text display with clear field labels and status interpretation
     */
    void display() {
        cout << "-------------------------\n";    // Top separator line for visual grouping
        cout << "Book ID: " << id << "\n";        // Display unique identifier for reference
        cout << "Title: " << title << "\n";       // Display book title exactly as stored
        cout << "Author: " << author << "\n";     // Display author name
        cout << "Category: " << category << "\n"; // Display book category/genre
        // Convert boolean availability to human-readable text for better user experience
        cout << "Status: " << (available ? "Available" : "Checked Out") << "\n";
        cout << "-------------------------\n";    // Bottom separator line for clear boundaries
    }
};

/*
  Class: Library
  Purpose: Manages the complete collection of books and provides all library operations
  Design Pattern: Encapsulation - private data members with public interface methods
  Memory Management: Uses fixed-size arrays for predictable memory usage
  File Persistence: Automatic saving/loading to maintain data between program runs
 */
class Library {
private:
    // Private data members - encapsulated to prevent direct external access
    Book books[MAX_BOOKS];      // Static array to store all book records - provides fast access and simple memory management
    int bookCount;              // Current number of books in the array - tracks actual vs. maximum capacity
    char filename[MAX_STRING_LENGTH]; // Name of the file where books data is persistently stored
    int nextId;                 // Next available ID for new books - ensures unique identifiers without gaps
    
    /*
      Private Helper Method: toLowerCase
      Purpose: Converts a string to lowercase for case-insensitive string comparisons
      Parameters: char* str - string to convert (modified in-place)
      Algorithm: Iterates through each character and applies tolower() transformation
      Usage: Called before string comparisons to ensure consistent case handling
     */
    void toLowerCase(char* str) {
        for (int i = 0; str[i]; i++) {          // Continue until null terminator
            str[i] = tolower(str[i]);           // Convert each character to lowercase
        }
    }
    
    /*
      Private Helper Method: containsSubstring
      Purpose: Performs case-insensitive substring search within a larger string
      Parameters: 
        - const char* str: The string to search within
        - const char* substr: The substring to search for
      Returns: bool - true if substring found, false otherwise
      Algorithm: Creates temporary copies, converts to lowercase, uses strstr for search
      Memory Safety: Uses strncpy with explicit null termination to prevent buffer overflows
     */
    bool containsSubstring(const char* str, const char* substr) {
        // Create temporary buffers to avoid modifying original strings
        char tempStr[MAX_STRING_LENGTH];
        char tempSubstr[MAX_STRING_LENGTH];
        
        // Copy strings to temporary buffers with bounds checking
        strncpy(tempStr, str, MAX_STRING_LENGTH - 1);
        tempStr[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination for safety
        
        strncpy(tempSubstr, substr, MAX_STRING_LENGTH - 1);
        tempSubstr[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination for safety
        
        // Convert both strings to lowercase for case-insensitive comparison
        toLowerCase(tempStr);
        toLowerCase(tempSubstr);
        
        // Use strstr to check if substring exists in string (returns NULL if not found)
        return (strstr(tempStr, tempSubstr) != NULL);
    }
    
    /*
      Private Method: saveToFile
      Purpose: Persists all book data to a CSV file for data persistence between program runs
      File Format: CSV with fields: ID,Title,Author,Category,Available(1/0)
      Error Handling: Checks file opening success and provides user feedback
      Design Decision: CSV format chosen for human readability and simple parsing
     */
    void saveToFile() {
        // Open the file for writing (overwrites existing content)
        ofstream file(filename);
        
        // Check if file opened successfully - critical for data persistence
        if (!file) {
            cout << "Error: Could not open file for writing.\n";
            return;  // Exit gracefully if file cannot be opened
        }
        
        // Write each book record to the file in CSV format
        for (int i = 0; i < bookCount; i++) {
            // CSV format: ID,Title,Author,Category,Available(1/0)
            // Boolean converted to 1/0 for consistent parsing
            file << books[i].id << "," << books[i].title << "," << books[i].author << ",";
            file << books[i].category << "," << (books[i].available ? "1" : "0") << "\n";
        }
        
        // Close the file to ensure data is flushed and file handle is released
        file.close();
    }
    
    /*
      Private Method: parseCsvLine
      Purpose: Parses a single CSV line into separate fields for data loading
      Parameters:
        - const char* line: The CSV line to parse
        - char fields[][MAX_STRING_LENGTH]: 2D array to store parsed fields
        - int& fieldCount: Reference to store the number of fields found
      Algorithm: Manual CSV parsing with comma delimiter detection
      Design Decision: Custom parsing instead of external library for simplicity and control
      Handles: Field overflow protection and proper null termination
     */
    void parseCsvLine(const char* line, char fields[][MAX_STRING_LENGTH], int& fieldCount) {
        fieldCount = 0;           // Initialize field counter
        int linePos = 0;          // Current position in the input line
        int fieldPos = 0;         // Current position within the current field
        
        // Process each character in the line until end or maximum fields reached
        while (line[linePos] != '\0' && fieldCount < 5) {
            if (line[linePos] == ',') {
                // Comma found - end of current field
                fields[fieldCount][fieldPos] = '\0';  // Null-terminate the current field
                fieldCount++;                         // Move to next field
                fieldPos = 0;                        // Reset position for next field
            } else {
                // Regular character - add to current field
                fields[fieldCount][fieldPos] = line[linePos];
                fieldPos++;
                
                // Prevent buffer overflow by limiting field length
                if (fieldPos >= MAX_STRING_LENGTH - 1) {
                    fieldPos = MAX_STRING_LENGTH - 1;           // Cap at maximum length
                    fields[fieldCount][fieldPos] = '\0';        // Ensure null termination
                } // Note: Excess characters are silently discarded
            }
            linePos++;  // Move to next character in line
        }
        
        // Handle the last field if there's content or if line ended with comma
        if (fieldPos > 0 || line[linePos-1] == ',') {
            fields[fieldCount][fieldPos] = '\0';  // Null-terminate the last field
            fieldCount++;                         // Increment field count
        }
    }
    
    /*
      Private Method: loadFromFile
      Purpose: Loads book data from CSV file into memory during program startup
      File Handling: Gracefully handles missing files by starting with empty library
      Data Validation: Ensures proper field count before processing each record
      ID Management: Tracks highest existing ID to maintain unique ID generation
      Error Recovery: Continues processing even if individual records are malformed
     */
    void loadFromFile() {
        // Attempt to open the file for reading
        ifstream file(filename);
        
        // Handle case when file doesn't exist (first run or missing file)
        if (!file) {
            cout << "No existing book file found. Starting with empty library.\n";
            nextId = 1;     // Start with ID 1 for the first book
            bookCount = 0;  // Initialize book count to 0 for empty library
            return;
        }
        
        // Reset book count for fresh loading
        bookCount = 0;
        
        // Variables for line parsing and field extraction
        char line[MAX_STRING_LENGTH * 4];  // Buffer for reading a line (4x larger to accommodate all fields plus separators)
        char fields[5][MAX_STRING_LENGTH]; // 2D array to hold parsed fields (ID, Title, Author, Category, Available)
        int fieldCount;                    // Number of fields successfully parsed from each line
        
        // Read file line by line until EOF or maximum books reached
        while (file.getline(line, sizeof(line)) && bookCount < MAX_BOOKS) {
            // Parse the CSV line into individual fields
            parseCsvLine(line, fields, fieldCount);
            
            // Validate that we have all 5 expected fields (ID, Title, Author, Category, Availability)
            if (fieldCount == 5) {
                // Convert and assign values to book fields with proper type conversion
                books[bookCount].id = atoi(fields[0]);  // Convert string ID to integer
                
                // Copy string fields with bounds checking and null termination
                strncpy(books[bookCount].title, fields[1], MAX_STRING_LENGTH - 1);
                books[bookCount].title[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination
                
                strncpy(books[bookCount].author, fields[2], MAX_STRING_LENGTH - 1);
                books[bookCount].author[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination
                
                strncpy(books[bookCount].category, fields[3], MAX_STRING_LENGTH - 1);
                books[bookCount].category[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination
                
                // Convert availability string to boolean ("1" = true, anything else = false)
                books[bookCount].available = (strcmp(fields[4], "1") == 0);
                
                // Update nextId to be higher than any existing ID to maintain uniqueness
                if (books[bookCount].id >= nextId) {
                    nextId = books[bookCount].id + 1;
                }
                
                // Increment book count after successfully loading a record
                bookCount++;
            }
            // Note: Malformed lines (wrong field count) are silently skipped
        }
        
        // Close the file to release file handle
        file.close();
    }
    
public:
    /*
      Constructor: Library
      Purpose: Initializes a new Library object with file persistence
      Parameters: const char* file - filename for data storage (defaults to "books.dat")
      Initialization: Sets up filename, counters, and loads existing data
      Design: Automatic file loading ensures data persistence between program runs
     */
    Library(const char* file = "books.dat") {
        // Set the filename for book data storage with bounds checking
        strncpy(filename, file, MAX_STRING_LENGTH - 1);
        filename[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination for safety
        
        nextId = 1;       // Initialize nextId to 1 (first book will have ID 1)
        bookCount = 0;    // Initialize book count to 0 (empty library initially)
        loadFromFile();   // Load existing books from file if available
    }
    
    /*
      Public Method: addBook
      Purpose: Adds a new book to the library with user-provided information
      User Interaction: Prompts for title, author, and category with input validation
      ID Management: Automatically assigns unique ID using nextId counter
      Data Persistence: Automatically saves to file after successful addition
      Error Handling: Checks for library capacity before adding
     */
    void addBook() {
        // Check if library has reached maximum capacity
        if (bookCount >= MAX_BOOKS) {
            cout << "Error: Library is full. Cannot add more books.\n";
            return;  // Exit early if no space available
        }
        
        // Create a new Book object for the new entry
        Book newBook;
        
        // Display section header for user interface clarity
        cout << "\n--- Add New Book ---\n";
        
        // Assign the next available unique ID to the new book
        newBook.id = nextId++;
        
        // Temporary buffer for safe input handling
        char buffer[MAX_STRING_LENGTH];
        
        // Clear input buffer to prevent getline from skipping due to previous cin operations
        cout << "Enter Title: ";
        cin.ignore();  // Remove any leftover newline characters from input buffer
        
        // Get book title with getline to handle spaces in input
        cin.getline(buffer, MAX_STRING_LENGTH);
        strncpy(newBook.title, buffer, MAX_STRING_LENGTH - 1);
        newBook.title[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination for safety
        
        // Get book author with same safe input handling
        cout << "Enter Author: ";
        cin.getline(buffer, MAX_STRING_LENGTH);
        strncpy(newBook.author, buffer, MAX_STRING_LENGTH - 1);
        newBook.author[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination for safety
        
        // Get book category with same safe input handling
        cout << "Enter Category: ";
        cin.getline(buffer, MAX_STRING_LENGTH);
        strncpy(newBook.category, buffer, MAX_STRING_LENGTH - 1);
        newBook.category[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination for safety
        
        // Set initial availability status (new books are always available initially)
        newBook.available = true;
        
        // Add the new book to the array and increment the counter
        books[bookCount++] = newBook;
        
        // Persist the updated collection to file for data permanence
        saveToFile();
        
        // Provide user confirmation with the assigned ID for future reference
        cout << "Book added successfully with ID: " << newBook.id << "\n";
    }
    
    /*
      Public Method: getBook
      Purpose: Retrieves and displays details of a specific book by ID
      Search Algorithm: Linear search through books array by ID
      User Interface: Prompts for ID and displays formatted book information
      Error Handling: Provides feedback when book ID is not found
     */
    void getBook() {
        int id;              // Variable to store the ID entered by the user
        bool found = false;  // Flag to track if the book was found during search
        
        // Display section header and prompt for user input
        cout << "\n--- Get Book Details ---\n";
        cout << "Enter Book ID: ";
        cin >> id;
        
        // Perform linear search through all books to find matching ID
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == id) {
                books[i].display();  // Use the Book's display method for formatted output
                found = true;        // Set flag to indicate successful search
                break;               // Exit loop immediately after finding the book (IDs are unique)
            }
        }
        
        // Provide user feedback if the requested book was not found
        if (!found) {
            cout << "Book with ID " << id << " not found.\n";
        }
    }
    
    /*
      Public Method: printBooks
      Purpose: Displays all books in the library in a formatted list
      Output Format: Shows total count followed by individual book details
      User Interface: Clear section headers and spacing for readability
      Performance: O(n) operation where n is the number of books
     */
    void printBooks() {
        // Display section header for user interface clarity
        cout << "\n--- All Library Books ---\n";
        
        // Handle empty library case with appropriate message
        if (bookCount == 0) {
            cout << "No books in the library.\n";
            return;  // Exit the function early if no books to display
        }
        
        // Display summary information (total count) before listing books
        cout << "Total Books: " << bookCount << "\n\n";
        
        // Iterate through all books and display each one
        for (int i = 0; i < bookCount; i++) {
            books[i].display();  // Use Book's display method for consistent formatting
            cout << "\n";        // Add extra newline for visual separation between books
        }
    }
    
    /*
      Public Method: findBook
      Purpose: Searches for books based on title, author, or category using substring matching
      Search Options: User selects search field (title/author/category)
      Algorithm: Case-insensitive substring search across all books
      User Interface: Menu-driven selection with results display
      Performance: O(n*m) where n is number of books and m is average string length
     */
    void findBook() {
        // Handle empty library case early
        if (bookCount == 0) {
            cout << "No books in the library.\n";
            return;  // Exit if no books available to search
        }
        
        int choice;                      // Variable for user's search type selection
        char searchTerm[MAX_STRING_LENGTH]; // Buffer for the search term input
        bool found = false;              // Flag to track if any books match the search
        
        // Display search options menu for user selection
        cout << "\n--- Find Book ---\n";
        cout << "Search by:\n";
        cout << "1. Title\n";
        cout << "2. Author\n";
        cout << "3. Category\n";
        cout << "Enter your choice (1-3): ";
        cin >> choice;
        
        // Clear input buffer for subsequent getline operation
        cin.ignore();
        
        // Validate user choice to prevent invalid array access
        if (choice < 1 || choice > 3) {
            cout << "Invalid choice. Returning to main menu.\n";
            return;  // Exit gracefully for invalid input
        }
        
        // Get search term from user with safe string input
        cout << "Enter search term: ";
        cin.getline(searchTerm, MAX_STRING_LENGTH);
        
        // Display results header for output organization
        cout << "\n--- Search Results ---\n";
        
        // Search through all books using the selected field
        for (int i = 0; i < bookCount; i++) {
            const char* valueToSearch = NULL;  // Pointer to the field to search in
            
            // Set the search target based on user's choice
            if (choice == 1) {
                valueToSearch = books[i].title;     // Search in book title
            } else if (choice == 2) {
                valueToSearch = books[i].author;    // Search in book author
            } else {
                valueToSearch = books[i].category;  // Search in book category
            }
            
            // Perform case-insensitive substring search
            if (containsSubstring(valueToSearch, searchTerm)) {
                books[i].display();  // Display matching book with full details
                cout << "\n";        // Add spacing between results for readability
                found = true;        // Mark that at least one match was found
            }
        }
        
        // Provide feedback if no matching books were found
        if (!found) {
            cout << "No matching books found.\n";
        }
    }
    
    /*
      Public Method: updateBook
      Purpose: Allows modification of existing book information by ID
      Update Options: Title, Author, Category, or Availability status
      User Interface: Menu-driven field selection with current values display
      Data Persistence: Automatically saves changes to file
      Validation: Confirms book exists before allowing updates
     */
    void updateBook() {
        int id, field;       // Variables for book ID and field selection
        bool found = false;  // Flag to track if book was found
        
        // Display section header and get book ID to update
        cout << "\n--- Update Book ---\n";
        cout << "Enter Book ID to update: ";
        cin >> id;
        
        // Search for the book with the specified ID
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == id) {
                found = true;  // Mark book as found
                
                // Display current book details for user reference
                books[i].display();
                
                // Present update options menu
                cout << "\nWhat would you like to update?\n";
                cout << "1. Title\n";
                cout << "2. Author\n";
                cout << "3. Category\n";
                cout << "4. Availability\n";
                cout << "5. Cancel\n";
                cout << "Enter your choice (1-5): ";
                cin >> field;
                
                // Clear input buffer for potential getline calls
                cin.ignore();
                
                // Buffer for string input operations
                char buffer[MAX_STRING_LENGTH];
                
                // Track if any actual update was made for save optimization
                bool updated = false;
                
                // Process update based on user's field selection
                switch (field) {
                    case 1:  // Update book title
                        cout << "Enter new title: ";
                        cin.getline(buffer, MAX_STRING_LENGTH);
                        strncpy(books[i].title, buffer, MAX_STRING_LENGTH - 1);
                        books[i].title[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination
                        updated = true;
                        break;
                    case 2:  // Update book author
                        cout << "Enter new author: ";
                        cin.getline(buffer, MAX_STRING_LENGTH);
                        strncpy(books[i].author, buffer, MAX_STRING_LENGTH - 1);
                        books[i].author[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination
                        updated = true;
                        break;
                    case 3:  // Update book category
                        cout << "Enter new category: ";
                        cin.getline(buffer, MAX_STRING_LENGTH);
                        strncpy(books[i].category, buffer, MAX_STRING_LENGTH - 1);
                        books[i].category[MAX_STRING_LENGTH - 1] = '\0';  // Ensure null termination
                        updated = true;
                        break;
                    case 4:  // Toggle availability status (simple boolean flip)
                        books[i].available = !books[i].available;
                        cout << "Book status changed to: " 
                              << (books[i].available ? "Available" : "Checked Out") << "\n";
                        updated = true;
                        break;
                    case 5:  // User chose to cancel the update
                        cout << "Update canceled.\n";
                        break;
                    default:  // Invalid choice handling
                        cout << "Invalid choice.\n";
                        break;
                }
                
                // Save changes and provide confirmation if an update was made
                if (updated) {
                    saveToFile();  // Persist changes to file for data permanence
                    cout << "Book updated successfully.\n";
                    books[i].display();  // Show updated book details for verification
                }
                
                break;  // Exit loop after processing the target book
            }
        }
        
        // Provide feedback if the requested book ID was not found
        if (!found) {
            cout << "Book with ID " << id << " not found.\n";
        }
    }
    
    /*
      Public Method: removeBook
      Purpose: Removes a book from the library after user confirmation
      Safety Feature: Requires explicit confirmation before deletion
      Algorithm: Array compaction by shifting elements left to fill gap
      Data Persistence: Automatically saves changes after successful removal
      Memory Management: Properly decrements book count and maintains array integrity
     */
    void removeBook() {
        int id;              // Variable for book ID to remove
        bool found = false;  // Flag to track if book was found
        
        // Display section header and get book ID to remove
        cout << "\n--- Remove Book ---\n";
        cout << "Enter Book ID to remove: ";
        cin >> id;
        
        // Search for the book with the specified ID
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == id) {
                found = true;  // Mark book as found
                
                // Display book details before removal for user verification
                books[i].display();
                
                // Safety confirmation to prevent accidental deletions
                char confirm;
                cout << "Are you sure you want to remove this book? (y/n): ";
                cin >> confirm;
                
                // Process based on user confirmation
                if (confirm == 'y' || confirm == 'Y') {
                    // Remove book by shifting all subsequent books one position back
                    // This maintains array compactness and preserves order
                    for (int j = i; j < bookCount - 1; j++) {
                        books[j] = books[j + 1];  // Copy next book to current position
                    }
                    bookCount--;  // Decrement total book count
                    
                    saveToFile();  // Persist changes to file immediately
                    cout << "Book removed successfully.\n";
                } else {
                    cout << "Removal canceled.\n";  // User chose not to remove
                }
                
                break;  // Exit loop after processing the target book
            }
        }
        
        // Provide feedback if the requested book ID was not found
        if (!found) {
            cout << "Book with ID " << id << " not found.\n";
        }
    }
    
    /*
      Public Method: showMenu
      Purpose: Main user interface - displays menu and handles user interactions
      Design Pattern: Event loop that continues until user chooses to exit
      Input Validation: Handles both invalid numeric and out-of-range inputs
      User Experience: Clear menu options with numbered choices for easy navigation
      Error Recovery: Gracefully handles input errors and continues operation
     */
    void showMenu() {
        int choice;          // Variable for user's menu selection
        bool running = true; // Control flag for the main program loop
        
        // Main program loop - continues until user chooses to exit
        while (running) {
            // Display main menu with clear formatting and all available options
            cout << "\n==== LIBRARY MANAGEMENT SYSTEM ====\n";
            cout << "1. Add New Book\n";
            cout << "2. Get Book Details\n";
            cout << "3. Print All Books\n";
            cout << "4. Find Book\n";
            cout << "5. Update Book\n";
            cout << "6. Remove Book\n";
            cout << "7. Exit\n";
            cout << "Enter your choice (1-7): ";
            
            // Handle invalid input (non-numeric) gracefully
            if (!(cin >> choice)) {
                cin.clear();                    // Clear error flags on cin
                cin.ignore(100, '\n');         // Discard invalid input up to 100 characters or newline
                cout << "Invalid input. Please enter a number.\n";
                continue;  // Return to start of loop for new input
            }
            
            // Process valid numeric input using switch statement for clean code organization
            switch (choice) {
                case 1:  // Add new book functionality
                    addBook();
                    break;
                case 2:  // Get specific book details by ID
                    getBook();
                    break;
                case 3:  // Display all books in library
                    printBooks();
                    break;
                case 4:  // Search for books by various criteria
                    findBook();
                    break;
                case 5:  // Update existing book information
                    updateBook();
                    break;
                case 6:  // Remove book from library
                    removeBook();
                    break;
                case 7:  // Exit program gracefully
                    cout << "Thank you for using the Library Management System. Goodbye!\n";
                    running = false;  // Set flag to exit the main loop
                    break;
                default:  // Handle valid numbers outside the expected range
                    cout << "Invalid choice. Please try again.\n";
                    break;
            }
        }
    }
};

/*
  Function: main
  Purpose: Program entry point - initializes library system and starts user interface
  Design: Simple initialization with default filename, then hands control to menu system
  Return: 0 for successful program completion (standard C++ convention)
  Architecture: Creates Library object which handles all functionality through its methods
 */
int main() {
    // Create a Library object with default filename for data persistence
    // Constructor automatically loads existing data if file exists
    Library lms;
    
    // Start the library management system by displaying the interactive menu
    // This method contains the main program loop and handles all user interactions
    lms.showMenu();
    
    // Return 0 to indicate successful program execution to the operating system
    return 0;
}
