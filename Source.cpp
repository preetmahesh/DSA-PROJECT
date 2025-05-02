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

// --------- Display Colored Text ---------
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

// --------- Display Start Window ---------
void displayStartWindow() {
    system("cls"); // Clear screen
    displayColoredText("Welcome to the Social Media Recommendation System!\n", 10);
    displayColoredText("Please select an option:\n", 15);
}

// --------- Main Menu ---------
void displayMainMenu() {
    cout << "Main Menu\n"; // Label added here
    int choice;
    cout << "1. Sign Up\n2. Login\n3. Exit\n";
    cin >> choice;
    switch (choice) {
    case 1:
        signUp();
        break;
    case 2:
        login();
        break;
    case 3:
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
    system("cls"); // Clear screen
    cout << "Sign Up\n"; // Label added here
    string username, password, displayName;
    bool isPublic;
    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;
    cout << "Enter your display name: ";
    cin >> displayName;
    cout << "Do you want your profile to be public? (1 for Yes, 0 for No): ";
    cin >> isPublic;

    User newUser;
    newUser.username = username;
    newUser.password = password;
    newUser.displayName = displayName;
    newUser.isPublic = isPublic;
    users[username] = newUser;
    displayColoredText("Sign Up successful! You can now log in.\n", 10);
    saveUsers(); // Save users after sign-up
    displayMainMenu();
}

// --------- Login ---------
void login() {
    system("cls"); // Clear screen
    cout << "Login\n"; // Label added here
    string username, password;
    cout << "Enter your username: ";
    cin >> username;
    cout << "Enter your password: ";
    cin >> password;

    if (users.find(username) != users.end() && users[username].password == password) {
        displayColoredText("Login successful!\n", 10);
        profileMenu(users[username]);
    }
    else {
        displayColoredText("Invalid username or password. Try again.\n", 12);
        displayMainMenu();
    }
}

// --------- Profile Menu ---------
void profileMenu(User& user) {
    system("cls"); // Clear screen
    cout << "Profile Menu\n"; // Label added here
    int choice;
    cout << "Welcome " << user.displayName << "!\n";
    cout << "1. Create Post\n2. View Feed\n3. Explore Posts\n4. View Saved Posts\n5. Send Message\n6. View Messages\n7. Logout\n";
    cin >> choice;
    switch (choice) {
    case 1:
        createPost(user);
        break;
    case 2:
        viewFeed(user);
        break;
    case 3:
        explorePosts();
        break;
    case 4:
        viewSavedPosts(user);
        break;
    case 5:
        sendMessage(user);
        break;
    case 6:
        viewMessages(user);
        break;
    case 7:
        logout(user);
        break;
    default:
        displayColoredText("Invalid choice. Try again.\n", 12);
        profileMenu(user);
        break;
    }
}

// --------- Create Post ---------
void createPost(User& user) {
    system("cls"); // Clear screen
    cout << "Create Post\n"; // Label added here
    string content;
    cout << "Enter the content of your post: ";
    cin.ignore();
    getline(cin, content);

    Post newPost(++lastPostId, user.username, content);
    posts[newPost.postId] = newPost;
    user.posts.push_back(newPost.postId);
    displayColoredText("Post created successfully!\n", 10);
    savePosts(); // Save posts after creating one
    profileMenu(user);
}

// --------- View Feed ---------
void viewFeed(User& user) {
    system("cls"); // Clear screen
    cout << "View Feed\n"; // Label added here
    cout << "Your Feed:\n";
    for (int postId : user.posts) {
        Post& post = posts[postId];
        cout << post.username << ": " << post.content << " | Likes: " << post.likes << "\n";
    }
    profileMenu(user);
}

// --------- Explore Posts ---------
void explorePosts() {
    system("cls"); // Clear screen
    cout << "Explore Posts\n"; // Label added here
    cout << "Explore Posts:\n";
    for (auto& pair : posts) {
        Post& post = pair.second;
        cout << post.username << ": " << post.content << " | Likes: " << post.likes << "\n";
    }
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

// --------- View Saved Posts ---------
void viewSavedPosts(User& user) {
    system("cls"); // Clear screen
    cout << "View Saved Posts\n"; // Label added here
    cout << "Your Saved Posts:\n";
    for (int postId : user.savedPosts) {
        Post& post = posts[postId];
        cout << post.username << ": " << post.content << " | Likes: " << post.likes << "\n";
    }
    profileMenu(user);
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

// --------- Logout ---------
void logout(User& user) {
    system("cls"); // Clear screen
    cout << "Logout\n"; // Label added here
    displayColoredText("You have logged out successfully.\n", 10);
    saveUsers(); // Ensure that users data is saved
    savePosts(); // Ensure that posts data is saved
    displayMainMenu();
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
        string username, password, displayName;
        bool isPublic;
        ss >> username >> password >> displayName >> isPublic;
        User user = { username, password, displayName, isPublic };
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
        userFile << user.username << " " << user.password << " " << user.displayName << " " << user.isPublic << "\n";
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
