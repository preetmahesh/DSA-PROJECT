#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <windows.h>
#include <limits>

using namespace std;


void displayColoredText(const string& text, int colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
    cout << text;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

// --------- Structures and Classes ---------
struct Post {
    int postId;
    string username;
    string content;
    int likes;
    vector<string> comments;

    Post() {}
    Post(int id, string user, string cont) : postId(id), username(user), content(cont), likes(0) {}
};

struct Message {
    string fromUser;
    string toUser;
    string content;
};

struct User {
    string username;
    string password;
    string displayName;
    bool isPublic;
    string bio;
    vector<int> posts;
    set<string> followers;
    set<string> following;
    stack<string> activityLog;
    queue<Message> messages;
    priority_queue<string> notifications;
    set<int> savedPosts;
};

unordered_map<string, User> users;
unordered_map<int, Post> posts;
int lastPostId = 0;

// --------- Function Prototypes ---------
void displayStartWindow();
void displayMainMenu();
void signUp();
void login();
void logout(User& user);
void profileMenu(User& user);
void createPost(User& user);
void viewFeed(User& user);
void explorePosts();
void likePost(User& user);
void commentOnPost(User& user);
void savePost(User& user);
void viewSavedPosts(User& user);
void sendMessage(User& user);
void viewMessages(User& user);
void searchUsers();
void searchPosts();
void loadUsers();
void saveUsers();
void loadPosts();
void savePosts();

// --------- Helper Functions ---------
void displayStartWindow() {
    system("cls");
    cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\n";
    cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\n\n\n\n\n\n\n";
    displayColoredText("        \t\t ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", 3); 
    displayColoredText("        \t\t 8  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  8\n", 3); // Yellow 
    displayColoredText("        \t\t 8  •                                                             •  8\n", 11); // Cyan 
    displayColoredText("        \t\t 8  •         Welcome to your Personalised Social engine!         •  8\n", 7); // Magenta 
    displayColoredText("        \t\t 8  •                                                             •  8\n", 13); // Green 
    displayColoredText("        \t\t 8  •                        Developed by:                        •  8\n", 14); // Yellow 
    displayColoredText("        \t\t 8  •                                                             •  8\n", 14); // Yellow 
    displayColoredText("        \t\t 8  •             Preet Mahesh, Jiya Lal, Abdul Wasay             •  8\n", 12); // Cyan
    displayColoredText("        \t\t 8  •                                                             •  8\n", 11); // Cyan 
    displayColoredText("        \t\t 8  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  8\n", 3); 
    displayColoredText("        \t\t ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", 3); 
    cout << "\n\n\n\n";
    cin.get(); // Wait for user input 
}
//18 is for green

void displayMainMenu() {
    int choice;
        system("cls");
        displayColoredText("\n\n\t\t\t+-----------------------------------------+\n", 4);
        displayColoredText("\t\t\t|       Welcome to our Social Media       |\n", 6);
        displayColoredText("\t\t\t+-----------------------------------------+\n\n", 4);
        cout << "\n\n\t\t Main Menu:\n";
        cout << "\t\t............\n\n";
        cout << "\t\t1. Sign Up" << endl;
        cout << "\t\t2. Login" << endl;
        cout << "\t\t3. Exit\n" << endl;
        cout << "\t\tEnter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            signUp();
            break;
        case 2:
            login();
            break;
        case 3:
            cout << endl;
            exit(0);
            break;
        default:
            displayColoredText("Invalid choice, try again!\n", 12);
            displayMainMenu();
            break;
        }

}

// --------- Sign Up ---------
void signUp() {
    system("cls");
    displayColoredText("\n\n\t\t           - SIGN UP PAGE - \n", 11);
    displayColoredText("\t\t----------------------------------- \n\n", 11);

    string username, password, displayName;
    bool isPublic;
    User newUser;

    cout << "\t\tEnter your username: ";
    cin >> username;

    cout << "\t\tEnter your password: ";
    cin >> password;

    cout << "\t\tEnter your display name: ";
    cin.ignore();
    getline(cin, displayName);

    cout << "\t\tEnter your bio (optional): ";
    cin.ignore();
    getline(cin, newUser.bio);


    cout << "\n\t\tDo you want your profile to be public? (1 = Yes, 0 = No): ";
    cin >> isPublic;

    newUser.username = username;
    newUser.password = password;
    newUser.displayName = displayName;
    newUser.isPublic = isPublic;

    users[username] = newUser;
    saveUsers();

    displayColoredText("\n\t\tSign Up successful! You can now log in.\n", 10);
    cin.ignore();
    cin.get(); // Pause before returning
    displayMainMenu();
}
//loginnnn
void login() {
    system("cls");
    displayColoredText("\n\n\t\t           - LOGIN PAGE - \n", 12);
    displayColoredText("\t\t----------------------------------- \n\n", 12);
    string username, password;
    cout << "\t\tEnter your username: ";
    cin >> username;
    cout << "\t\tEnter your password: ";
    cin >> password;

    auto it = users.find(username);
    if (it != users.end() && it->second.password == password) {
        displayColoredText("Login successful!\n", 10);

        // Create a local copy of the user object for the session
        User currentUser = it->second;

        // user goes aagay
        profileMenu(currentUser);

        // any changes (post, share wtv gets saved to map again)
        users[username] = currentUser;
    }
    else {
        displayColoredText("\n\t\tInvalid username or password. Try again.\n", 12);
        cin.ignore();
        cin.get();
        displayMainMenu();
    }
}

void profileDisplay(User& user) {
    displayColoredText("\n\n\t\t       - PROFILE DASHBOARD - \n", 14);
    displayColoredText("\t\t----------------------------------- \n\n", 14);

    // Profile Summary
    cout << "\t\tUsername: " << user.username << "\n";
    cout << "\t\tDisplay Name: " << user.displayName << "\n";
    cout << "\t\tBio: ";
    if (user.bio.empty()) {
        cout << "N/A";
    }
    else {
        cout << user.bio;
    }
    cout << "\n\t\tAccount Type: ";
    if (user.isPublic)
        cout << "Public\n\n";
    else
        cout << "Private\n\n";

    // Follower & Post Stats
    cout << "\t\tPosts: " << user.posts.size()
        << " | Followers: " << user.followers.size()
        << " | Following: " << user.following.size() << "\n";
    cin.ignore();
    cin.get();
    profileMenu(user);
    
    
}

// --------- Profile Menu ---------
void profileMenu(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t     - PROFILE MENU - \n", 14);
    displayColoredText("\t\t----------------------------------- \n\n", 14);

    cout << "\t\tWelcome, " << user.displayName << "!\n\n";
    cout << "\t\t1. Profile Display\n";
    cout << "\t\t2. Create Post\n";
    cout << "\t\t3. View Feed\n";
    cout << "\t\t4. Explore Posts\n";
    cout << "\t\t5. View Saved Posts\n";
    cout << "\t\t6. Send Message\n";
    cout << "\t\t7. View Messages\n";
    cout << "\t\t8. Logout\n\n";
    cout << "\t\tEnter your choice: ";

    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        profileDisplay(user);
        break;
    case 2:
        createPost(user); 
        break;
    case 3:
        viewFeed(user); 
        break;
    case 4: 
        explorePosts(); 
        break;
    case 5:
        viewSavedPosts(user);
        break;
    case 6:
        sendMessage(user); 
        break;
    case 7:
        viewMessages(user);
        break;
    case 8:
        logout(user);
        break;
    default:
        displayColoredText("\n\t\tInvalid choice. Try again.\n", 12);
        cin.ignore(); 
        cin.get();
        profileMenu(user);
    }
}
// --------- Create Post ---------
void createPost(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t          - CREATE POST - \n", 11);
    displayColoredText("\t\t----------------------------------- \n\n", 11);

    string content;
    cout << "\t\tEnter the content of your post:\n\t\t";
    cin.ignore();
    getline(cin, content);

    Post newPost(++lastPostId, user.username, content);
    posts[newPost.postId] = newPost;
    user.posts.push_back(newPost.postId);
    savePosts();

    displayColoredText("\n\t\tPost created successfully!\n", 10);
    cin.get();
    profileMenu(user);
}

// --------- View Feed ---------
void viewFeed(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t          - YOUR FEED - \n", 11);
    displayColoredText("\t\t----------------------------------- \n\n", 11);

    for (int postId : user.posts) {
        Post& post = posts[postId];
        cout << "\t\t" << post.username << ": " << post.content
            << " | Likes: " << post.likes << "\n";
    }

    cout << "\n\t\tPress Enter to return...";
    cin.ignore();
    cin.get();
    profileMenu(user);
}


// --------- Explore Posts ---------
void explorePosts() {
    system("cls");
    displayColoredText("\n\n\t\t          - EXPLORE POSTS - \n", 11);
    displayColoredText("\t\t----------------------------------- \n\n", 11);

    for (auto& pair : posts) {
        Post& post = pair.second;
        cout << "\t\t" << post.username << ": " << post.content
            << " | Likes: " << post.likes << "\n";
    }

    cout << "\n\t\tPress Enter to return...";
    cin.ignore();
    cin.get();
    displayMainMenu();
}


// --------- Like Post ---------
void likePost(User& user) {
    system("cls"); // Clear screen
    cout << "Like Post\n"; // Label added here
    int postId;
    cout << "Enter the post ID to like: ";
    cin >> postId;

    if (posts.find(postId) != posts.end()) {
        posts[postId].likes++;
        displayColoredText("You liked the post!\n", 10);
    }
    else {
        displayColoredText("Post not found.\n", 12);
    }
    savePosts(); // Save after liking post
    profileMenu(user);
}

// --------- Comment on Post ---------
void commentOnPost(User& user) {
    system("cls"); // Clear screen
    cout << "Comment on Post\n"; // Label added here
    int postId;
    string comment;
    cout << "Enter the post ID to comment on: ";
    cin >> postId;
    cout << "Enter your comment: ";
    cin.ignore();
    getline(cin, comment);

    if (posts.find(postId) != posts.end()) {
        posts[postId].comments.push_back(comment);
        displayColoredText("Comment added!\n", 10);
    }
    else {
        displayColoredText("Post not found.\n", 12);
    }
    savePosts(); // Save after commenting
    profileMenu(user);
}

// --------- Save Post ---------
void savePost(User& user) {
    system("cls"); // Clear screen
    cout << "Save Post\n"; // Label added here
    int postId;
    cout << "Enter the post ID to save: ";
    cin >> postId;

    if (posts.find(postId) != posts.end()) {
        user.savedPosts.insert(postId);
        displayColoredText("Post saved!\n", 10);
    }
    else {
        displayColoredText("Post not found.\n", 12);
    }
    profileMenu(user);
}
void viewSavedPosts(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t        - SAVED POSTS - \n", 14);
    displayColoredText("\t\t-----------------------------------\n\n", 14);

    if (user.savedPosts.empty()) {
        displayColoredText("\t\tNo saved posts.\n", 12);
    }
    else {
        for (int postId : user.savedPosts) {
            Post& post = posts[postId];
            cout << "\t\t[" << post.postId << "] " << post.username << ": " << post.content
                << " | Likes: " << post.likes << "\n";
        }
    }

    cout << "\n\t\tPress Enter to return...";
    cin.ignore();
    cin.get();
}


// --------- Send Message ---------
void sendMessage(User& user) {
    system("cls"); // Clear screen
    cout << "Send Message\n"; // Label added here
    string toUser, content;
    cout << "Enter the username to send a message to: ";
    cin >> toUser;
    cout << "Enter your message: ";
    cin.ignore();
    getline(cin, content);

    if (users.find(toUser) != users.end()) {
        Message msg = { user.username, toUser, content };
        users[toUser].messages.push(msg);
        displayColoredText("Message sent successfully!\n", 10);
    }
    else {
        displayColoredText("User not found.\n", 12);
    }
    profileMenu(user);
}

// --------- View Messages ---------
void viewMessages(User& user) {
    system("cls"); // Clear screen
    cout << "View Messages\n"; // Label added here
    cout << "Your Messages:\n";
    while (!user.messages.empty()) {
        Message msg = user.messages.front();
        user.messages.pop();
        cout << "From: " << msg.fromUser << " | To: " << msg.toUser << "\n";
        cout << "Message: " << msg.content << "\n";
    }
    profileMenu(user);
}

void logout(User& user) {
    
    displayColoredText("\n\n\t\tAre you sure you want to logout? (1 = Yes, 2 = No): ", 14);
    int choice;
    cin >> choice;

    if (choice == 1) {
        cout << "\t\tLogging out...\n";
        Sleep(1000);
        saveUsers(); // Ensure that users data is saved
        savePosts(); // Ensure that posts data is saved
        displayMainMenu(); // Return to main menu (Sign up / Login page)
    }
    else if (choice == 2) {
        displayMainMenu(); // Return to user's profile menu without logging out
    }
    else {
        displayColoredText("Invalid choice, please enter 1 or 2.\n", 12);
        logout(user); // Retry if the input is invalid
    }
}


// --------- Load Data ---------
void loadUsers() {
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "Error loading users file!" << endl;
        return;
    }
    string line;
    while (getline(userFile, line)) {
        stringstream ss(line);
        string username, password, displayName, bio;
        bool isPublic;
        ss >> username >> password >> displayName >> isPublic;
        getline(ss, bio); 
        User user = { username, password, displayName, isPublic };
        user.bio = bio;
        users[username] = user;

    }
    userFile.close();
}

// --------- Save Data ---------
void saveUsers() {
    ofstream userFile("users.txt");
    if (!userFile) {
        cerr << "Error saving users file!" << endl;
        return;
    }
    for (auto& pair : users) {
        User& user = pair.second;
        userFile << user.username << " " << user.password << " " << user.displayName << " " << user.isPublic << " " << user.bio << "\n";
    }
    userFile.close();
}

// --------- Load Posts ---------
void loadPosts() {
    ifstream postFile("posts.txt");
    if (!postFile) {
        cerr << "Error loading posts file!" << endl;
        return;
    }
    string line;
    while (getline(postFile, line)) {
        stringstream ss(line);
        int postId;
        string username, content;
        ss >> postId;
        cin.ignore();
        getline(ss, username, '\n');
        getline(ss, content, '\n');
        Post post = { postId, username, content };
        posts[postId] = post;
    }
    postFile.close();
}

// --------- Save Posts ---------
void savePosts() {
    ofstream postFile("posts.txt");
    if (!postFile) {
        cerr << "Error saving posts file!" << endl;
        return;
    }
    for (auto& pair : posts) {
        Post& post = pair.second;
        postFile << post.postId << " " << post.username << " " << post.content << "\n";
    }
    postFile.close();
}

int main() {
    loadUsers();
    loadPosts();
    displayStartWindow();
    displayMainMenu();
    return 0;
}
