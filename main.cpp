#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>

using namespace std;

string admin_login(); 
void add_book(); 
void display(string roll); 
void display_sorted(); 
void delete_book();
void delete_admin();
void delete_student();
void edit_book(); 
int find_student(string roll); 
int find_book(int isbn); 
string get_roll_from_username(string name);
void issue_item(string roll); 
void show_issued_books_admin();
void load_data(); 
void return_issued_books(string roll);
void save_data(); 
void show_issued_books(string roll);
string student_login(); 
void register_admin(); 
void register_student();
void view_books();

struct Student {
    string roll;
    string name;
    string password;
};

struct Book {
    int isbn;
    string title;
    string author;
    bool available;
};

unordered_map<string, string> admins;
vector<Student> students;
vector<Book> books;

int main() {
    load_data();

    int option;
    string logged_in_admin = "";
    string logged_in_student = ""; 

    while (true) {
        if (logged_in_admin.empty() && logged_in_student.empty()) {
            cout << "Login as: \n1. Register Admin\n2. Admin Login\n3. Register Student \n4. Student Login\n0. Exit\n\n";
            cin >> option;

            if (option == 0) {
                save_data();
                break;
            }

            bool is_admin_login = (option == 1 || option == 2);

            if (is_admin_login) {
                if (option == 1)
                    register_admin();
                else if (option == 2) {
                    logged_in_admin = admin_login();
                    if (logged_in_admin ==2
                     "")
                        continue;
                }
            } else {
                if (option == 3)
                    register_student();
                else if (option == 4) {
                    string roll = student_login();
                    if (roll != "-1") {
                        logged_in_student = roll; 
                    }
                    else
                        continue;
                }
            }
        }

        if (!logged_in_admin.empty()) {
            cout << "Admin options:\n1. Add Book\n2. Edit Book\n3. View Books\n4. View enrolled students\n5. View Student Detail\n6. Delete Book\n7. Delete Account\n8. Show Issused Books\n0. Logout\n\n";
            cin >> option;

            switch (option) {
                case 1: add_book(); break;
                case 2: edit_book(); break;
                case 3: view_books(); break;
                case 4: display_sorted(); break;
                case 5: {
                    string roll;
                    cout << "Enter student roll number: ";
                    cin >> roll;
                    display(roll);
                    break;
                }
                case 6: delete_book(); break;
                case 7: delete_admin(); logged_in_admin = ""; break;
                case 8: show_issued_books_admin(); break;
                case 0: logged_in_admin = ""; cout << "Logged out.\n\n"; break;
                default: cout << "Invalid admin option.\n\n";
            }
        } else if (!logged_in_student.empty()) {
            cout << "Student options:\n1. Profile\n2. Issue Book\n3. Show Issued Books\n4. Delete Account\n5. Return Issused Book\n0. Logout\n\n";
            cin >> option;

            string roll = get_roll_from_username(logged_in_student);

            switch (option) {
                case 1: display(roll); break;
                case 2: issue_item(roll); break;
                case 3: show_issued_books(roll); break;
                case 4: delete_student(); logged_in_student = ""; break;
                case 5: return_issued_books(roll); break;
                case 0: logged_in_student = ""; cout << "Logged out.\n\n"; break; 
                default: cout << "Invalid student option.\n\n";
            }
        }
    }

    return 0;
}

void save_data() {
    ofstream file("data.txt");
    if (file.is_open()) {
        file << "#Admins\n";
        for (const auto& admin : admins) {
            file << admin.first << " " << admin.second << "\n";
        }

        file << "#Students\n";
        for (const auto& student : students) {
            file << student.roll << " " << student.name << " " << student.password << "\n";
        }

        file << "#Books\n";  // Save books
        for (const auto& book : books) {
            file << book.isbn << " " << book.title << " " << book.author << " " << book.available << "\n";
        }

        file.close();
    }
}

void load_data() {
    ifstream file("data.txt");
    if (file.is_open()) {
        string line;
        bool readingAdmins = false, readingStudents = false, readingBooks = false; 

        while (getline(file, line)) {
            if (line == "#Admins") {
                readingAdmins = true;
                readingStudents = false;
                readingBooks = false;
                continue;
            } else if (line == "#Students") {
                readingAdmins = false;
                readingStudents = true;
                readingBooks = false;
                continue;
            } else if (line == "#Books") {
                readingAdmins = false;
                readingStudents = false;
                readingBooks = true;
                continue;
            }

            stringstream ss(line);

            if (readingAdmins) {
                string username, password;
                ss >> username >> password;
                admins[username] = password;
            } else if (readingStudents) {
                Student student;
                ss >> student.roll >> student.name >> student.password;
                students.push_back(student);
            } else if (readingBooks) {
                Book book;
                ss >> book.isbn;
                getline(ss, line);
                stringstream title_ss(line);
                string word;
                bool first_word = true;
                while(title_ss >> word){
                    if(first_word){
                        book.title += word;
                        first_word = false;
                    }
                    else
                        book.title += " " + word;
                }
                
                getline(ss, line);
                stringstream author_ss(line);
                first_word = true;
                while(author_ss >> word){
                    if(first_word){
                        book.author += word;
                        first_word = false;
                    }
                    else
                        book.author += " " + word;
                }
                ss >> book.available;
                books.push_back(book);
            }
        }
        file.close();
    }
}

string admin_login() {
    string username, password;
    cout << "Enter admin username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (admins.find(username) != admins.end() && admins[username] == password) {
        cout << "Admin login successful!\n\n";
        return username;
    } 
    else {
        cout << "Invalid credentials or Account Not found.\n\n";
        return "";
    }
}

void register_admin() {
    string username, password;
    cout << "Enter new admin username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (admins.find(username) != admins.end()) {
        cout << "Admin already exists!\n\n";
        return;
    }

    admins[username] = password;
    save_data();
    cout << "Admin registered successfully!\n\n";
}

void register_student() {
    Student student;

    cout << "Enter student Roll No.: ";
    cin >> student.roll;
    cout << "Enter student name: ";
    cin.ignore();

    getline(cin, student.name);

    cout << "Enter password: ";
    string password;

    cin >> password;
    student.password = password;

    students.push_back(student);
    save_data();
    cout << "Student registered successfully!\n\n";
}

string student_login() {
    string username, password;
    cout << "Enter student user name: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    for (const auto& student : students) {
        if (student.name == username && student.password == password) {
            cout << "Student login successful!\n\n";
            return username;
        }
    }

    cout << "Invalid credentials or Account not found.\n\n";
    return "-1";
}

int find_student(string roll) {
    for (int i = 0; i < students.size(); i++) {
        if (students[i].roll == roll)
            return i;
    }

    return -1;
}

void display(string roll) {
    int index = find_student(roll);

    if (index == -1) {
        cout << "Student not found.\n\n";
        return;
    }

    cout << "Roll No: " << students[index].roll << endl;
    cout << "Name: " << students[index].name << endl;
}

int find_book(int isbn) {
    for (int i = 0; i < books.size(); i++) {
        if (books[i].isbn == isbn)
            return i;
    }

    return -1;
}

void add_book() {
    Book new_book;
    cout << "Enter book title: ";
    cin.ignore(); // Consume the newline left by previous input
    getline(cin, new_book.title);

    cout << "Enter book author: ";
    getline(cin, new_book.author);

    cout << "Enter book ISBN: ";
    cin >> new_book.isbn;

    if (find_book(new_book.isbn) != -1) {
        cout << "A book with this ISBN already exists.\n\n";
        return;
    }

    new_book.available = true;
    books.push_back(new_book);
    cout << "Book added successfully.\n\n";

    save_data();
}


void edit_book() {
    int isbn;
    cout << "Enter book ISBN to edit: ";
    cin >> isbn;

    int index = find_book(isbn);
    if (index == -1) {
        cout << "Book not found.\n\n";
        return;
    }

    cout << "Current book title: " << books[index].title << endl;
    cout << "Enter new book title (or press Enter to keep current): ";
    cin.ignore();

    string new_title;
    getline(cin, new_title);

    if (!new_title.empty())
        books[index].title = new_title;

    cout << "Current book author: " << books[index].author << endl;
    cout << "Enter new book author (or press Enter to keep current): ";

    string new_author;
    getline(cin, new_author);

    if (!new_author.empty())
        books[index].author = new_author;

    cout << "Book details updated.\n\n";

    save_data();
}

void view_books() {
    if (books.empty()) {
        cout << "No books in the library.\n\n";
        return;
    }
    for (const auto& book : books) {
        cout << "Title: " << book.title << endl;
        cout << "Author: " << book.author << endl;
        cout << "ISBN: " << book.isbn << endl;
        cout << "Available: " << (book.available ? "Yes" : "No") << endl << endl;
    }
}

void issue_item(string roll) {
    int student_index = find_student(roll);
    if (student_index == -1) {
        cout << "Student not found.\n\n";
        return;
    }

    cout << "Available books:\n\n";
    if(books.size() == 0)
        cout << "No Books Found. Add books to the Library...\n";
    else {
        for (int i = 0; i < books.size(); i++) {
            if (books[i].available)
                cout << i + 1 << ". " << books[i].title << " by " << books[i].author << " (ISBN: " << books[i].isbn << ")\n";
        }

        int choice;
        cout << "Enter the ISBN of the book you want to issue (0 to cancel): ";
        cin >> choice;

        if (choice == 0) 
            return;

        if (choice > 0 && choice <= books.size()) {
            int book_index = choice - 1;

            if (books[book_index].available) {
                books[book_index].available = false;
                cout << "Book issued successfully.\n\n";
            } 
            else
                cout << "Book is not available.\n\n";
        } 
        else
            cout << "Invalid book choice.\n\n";
    }

}

void display_sorted() {
    vector<Student> sorted_students = students;

    sort(sorted_students.begin(), sorted_students.end(), [](const Student& a, const Student& b) {
        return a.roll < b.roll;
    });

    for (const auto& student : sorted_students) {
        cout << student.roll << " - " << student.name << endl;
    }
}

void delete_book() {
    int isbn;
    cout << "Enter ISBN of book to delete: ";
    cin >> isbn;

    int index = find_book(isbn);
    if (index == -1) {
        cout << "Book not found.\n\n";
        return;
    }

    books.erase(books.begin() + index);
    cout << "Book deleted successfully.\n\n";
    save_data(); 
}


void delete_admin() {
    string username, password;
    cout << "Enter username of admin to delete: ";
    cin >> username;

    if (admins.find(username) == admins.end()) {
        cout << "Admin not found.\n\n";
        return;
    }

    cout << "Enter password for verification: ";
    cin >> password;

    if (admins[username] == password) {
        admins.erase(username);
        cout << "Admin deleted successfully.\n\n";
        save_data();
    } else {
        cout << "Incorrect password.\n\n";
    }

    save_data();
}

void delete_student() {
    string roll, password;
    cout << "Enter Roll No. of student to delete: ";
    cin >> roll;

    int index = find_student(roll);
    if (index == -1) {
        cout << "Student not found.\n\n";
        return;
    }

    cout << "Enter password for verification: ";
    cin >> password;

    if (students[index].password == password) {
        students.erase(students.begin() + index);
        cout << "Student deleted successfully.\n\n";
        save_data();
    } else {
        cout << "Incorrect password.\n\n";
    }

    save_data();
}

void show_issued_books(string roll) {
    int student_index = find_student(roll);
    if (student_index == -1) {
        cout << "Student not found.\n\n";
        return;
    }

    bool books_issued = false;
    cout << "Issued Books:\n";
    for (const auto& book : books) {
        for(auto const& st : students){
            if(!book.available && st.roll == roll){ // Check if the book is issued to the logged in student
                cout << book.title << " by " << book.author << " (ISBN: " << book.isbn << ")\n";
                books_issued = true;
                break;
            }
        }
    }

    if (!books_issued) {
        cout << "No books issued.\n";
    }
    cout << endl;
}

string get_roll_from_username(string name) {
    for (const auto& student : students) {
        if (student.name == name) {
            return student.roll;
        }
    }
    return "";
}

void show_issued_books_admin() {
    if (books.empty()) {
        cout << "No books in the library.\n\n";
        return;
    }

    if (students.empty()) {
        cout << "No students registered.\n\n";
        return;
    }

    bool books_issued = false;
    cout << "Issued Books:\n";

    for (const auto& book : books) {
        if (!book.available) {
            for (const auto& student : students) {
                if (book.author == student.name) { // Replace with your actual check
                    cout << "Book: " << book.title << " by " << book.author << " (ISBN: " << book.isbn << ") - Issued to: " << student.name << " (Roll: " << student.roll << ")\n";
                    books_issued = true;
                    break; 
                }
            }
        }
    }

    if (!books_issued) {
        cout << "No books are currently issued.\n";
    }
    cout << endl;
}

void return_issued_books(string roll) {
    int student_index = find_student(roll);
    if (student_index == -1) {
        cout << "Student not found.\n\n";
        return;
    }

    bool books_issued_to_student = false;
    cout << "Books issued to you:\n";

    for (auto& book : books) { 
        if (!book.available) {
            if (book.author == students[student_index].name) {
                cout << book.title << " by " << book.author << " (ISBN: " << book.isbn << ")\n";
                books_issued_to_student = true;

                char return_choice;
                cout << "Do you want to return this book? (y/n): ";
                cin >> return_choice;

                if (tolower(return_choice) == 'y') {
                    book.available = true;
                    cout << "Book returned successfully.\n";
                    save_data(); 
                }
            }
        }
    }

    if (!books_issued_to_student) {
        cout << "You have no books currently issued.\n";
    }
    cout << endl;
}
