#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <windows.h>
#include <limits>

using namespace std;

// --------- Manual Data Structures ---------
const int MAX_USERS = 100;
const int MAX_POSTS = 100;
const int MAX_SET_SIZE = 50;
const int MAX_STACK_SIZE = 50;
const int MAX_NOTIFICATIONS = 50; // For CustomQueue
const int HASH_TABLE_SIZE = 101; // Prime number for hashing
const int BUCKET_SIZE = 10; // Max entries per hash bucket

// Custom Set (for followers, following, savedPosts)
struct CustomSet {
    string elements[MAX_SET_SIZE];
    int int_elements[MAX_SET_SIZE];
    int size;

    CustomSet() : size(0) {}

    bool insert(const string& value) {
        if (size >= MAX_SET_SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (elements[i] == value) return false;
        }
        int i = size - 1;
        while (i >= 0 && elements[i] > value) {
            elements[i + 1] = elements[i];
            i--;
        }
        elements[i + 1] = value;
        size++;
        return true;
    }

    bool insert(int value) {
        if (size >= MAX_SET_SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (int_elements[i] == value) return false;
        }
        int i = size - 1;
        while (i >= 0 && int_elements[i] > value) {
            int_elements[i + 1] = int_elements[i];
            i--;
        }
        int_elements[i + 1] = value;
        size++;
        return true;
    }

    bool contains(const string& value) const {
        for (int i = 0; i < size; i++) {
            if (elements[i] == value) return true;
        }
        return false;
    }

    bool contains(int value) const {
        for (int i = 0; i < size; i++) {
            if (int_elements[i] == value) return true;
        }
        return false;
    }
};

// Custom Stack (for activityLog)
struct CustomStack {
    string elements[MAX_STACK_SIZE];
    int top;

    CustomStack() : top(-1) {}

    void push(const string& value) {
        if (top < MAX_STACK_SIZE - 1) {
            elements[++top] = value;
        }
    }

    string pop() {
        if (top >= 0) {
            return elements[top--];
        }
        return "";
    }

    bool empty() const {
        return top == -1;
    }
};

// Custom Queue for notifications (replaces CustomMaxHeap)
struct CustomQueue {
    string elements[MAX_NOTIFICATIONS];
    int front;
    int rear;
    int size;

    CustomQueue() : front(0), rear(-1), size(0) {}

    bool enqueue(const string& value) {
        if (size >= MAX_NOTIFICATIONS) return false;
        rear = (rear + 1) % MAX_NOTIFICATIONS;
        elements[rear] = value;
        size++;
        return true;
    }

    string dequeue() {
        if (size == 0) return "";
        string value = elements[front];
        front = (front + 1) % MAX_NOTIFICATIONS;
        size--;
        return value;
    }

    string peek() const {
        if (size == 0) return "";
        return elements[front];
    }

    bool empty() const {
        return size == 0;
    }

    int getSize() const {
        return size;
    }
};

// --------- Structures and Classes ---------
struct Post {
    int postId;
    string username;
    string content;
    int likes;
    string keywords[5];
    vector<string> comments;
    int priority;

    Post() {}
    Post(int id, string user, string cont, string tags[5]) :
        postId(id), username(user), content(cont), likes(0), priority(0) {
        for (int i = 0; i < 5; i++) {
            keywords[i] = tags[i];
        }
    }
};

struct Message {
    string fromUser;
    string toUser;
    string content;
};

struct User {
    int userId;
    string username;
    string password;
    string displayName;
    bool isPublic;
    string bio;
    vector<int> posts;
    string interest[5];
    CustomSet friends;
    //CustomSet following;
    CustomStack activityLog;
    queue<Message> messages;
    CustomQueue notifications; // Replaced CustomMaxHeap
    CustomSet savedPosts;
    
    User(){}
    User(int id) : userId(id) { }
};

// Custom Hash Table for Users (string -> User)
struct UserHashTable {
    struct BucketEntry {
        string key;
        User value;
        bool isUsed;
    };
    struct Bucket {
        BucketEntry entries[BUCKET_SIZE];
        int size;
        Bucket() : size(0) {
            for (int i = 0; i < BUCKET_SIZE; i++) entries[i].isUsed = false;
        }
    };
    Bucket table[HASH_TABLE_SIZE];
    int size;

    UserHashTable() : size(0) {}

    // Polynomial hash function for strings
    int hash(const string& key) const {
        unsigned long hashVal = 0;
        for (int i = 0; i < key.length(); i++) {
            hashVal = 37 * hashVal + key[i];
        }
        hashVal %= HASH_TABLE_SIZE;
        if (hashVal < 0) hashVal += HASH_TABLE_SIZE;
        return hashVal;
    }

    void insert(const string& key, const User& value) {
        if (size >= MAX_USERS) return;
        int index = hash(key);
        Bucket& bucket = table[index];

        // Update if exists
        for (int i = 0; i < bucket.size; i++) {
            if (bucket.entries[i].isUsed && bucket.entries[i].key == key) {
                bucket.entries[i].value = value;
                return;
            }
        }

        // Insert new
        if (bucket.size < BUCKET_SIZE) {
            bucket.entries[bucket.size].key = key;
            bucket.entries[bucket.size].value = value;
            bucket.entries[bucket.size].isUsed = true;
            bucket.size++;
            size++;
        }
    }

    User* find(const string& key) {
        int index = hash(key);
        Bucket& bucket = table[index];
        for (int i = 0; i < bucket.size; i++) {
            if (bucket.entries[i].isUsed && bucket.entries[i].key == key) {
                return &bucket.entries[i].value;
            }
        }
        return nullptr;
    }
};

// Custom Hash Table for Posts (int -> Post)
struct PostHashTable {
    struct BucketEntry {
        int key;
        Post value;
        bool isUsed;
    };
    struct Bucket {
        BucketEntry entries[BUCKET_SIZE];
        int size;
        Bucket() : size(0) {
            for (int i = 0; i < BUCKET_SIZE; i++) entries[i].isUsed = false;
        }
    };
    Bucket table[HASH_TABLE_SIZE];
    int size;

    PostHashTable() : size(0) {}

    // Hash function for integers
    int hash(int key) const {
        return key % HASH_TABLE_SIZE;
    }

    void insert(int key, const Post& value) {
        if (size >= MAX_POSTS) return;
        int index = hash(key);
        Bucket& bucket = table[index];

        for (int i = 0; i < bucket.size; i++) {
            if (bucket.entries[i].isUsed && bucket.entries[i].key == key) {
                bucket.entries[i].value = value;
                return;
            }
        }

        if (bucket.size < BUCKET_SIZE) {
            bucket.entries[bucket.size].key = key;
            bucket.entries[bucket.size].value = value;
            bucket.entries[bucket.size].isUsed = true;
            bucket.size++;
            size++;
        }
    }

    Post* find(int key) {
        int index = hash(key);
        Bucket& bucket = table[index];
        for (int i = 0; i < bucket.size; i++) {
            if (bucket.entries[i].isUsed && bucket.entries[i].key == key) {
                return &bucket.entries[i].value;
            }
        }
        return nullptr;
    }

    // Iterate over all posts
    template <typename Func>
    void forEach(Func func) {
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            Bucket& bucket = table[i];
            for (int j = 0; j < bucket.size; j++) {
                if (bucket.entries[j].isUsed) {
                    func(bucket.entries[j].value);
                }
            }
        }
    }
};

// Global Data
UserHashTable users;
PostHashTable posts;
vector<string> friendShips[100];
int lastPostId = 0;
int lastUserId = 0;

// --------- Function Prototypes ---------
void displayColoredText(const string& text, int colorCode);
void displayStartWindow();
void displayMainMenu();
void signUp();
void login();
void logout(User& user);
void profileMenu(User& user);
void createPost(User& user);
void viewFeed(User& user);
void explorePosts(User& user);
void likePost(User& user, int postId);
void commentOnPost(User& user, int postId);
void savePost(User& user, int postId);
void viewSavedPosts(User& user);
void sendMessage(User& user);
void viewMessages(User& user);
//void addFriends(User& user);
void makeFriends(User& user);// New function
void viewNotifications(User& user); // New function
void searchUsers();
void searchPosts();
void loadUsers();
void saveUsers();
void loadPosts();
void savePosts();

// --------- Helper Functions ---------
void displayColoredText(const string& text, int colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
    cout << text;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void displayStartWindow() {
    system("cls");
    cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\n";
    cout << "\t\t\t\t\t\t\t\t\t\t\t\t\t\n\n\n\n\n\n\n";
    displayColoredText("        \t\t ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", 3);
    displayColoredText("        \t\t 8  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  8\n", 3);
    displayColoredText("        \t\t 8  •                                                             •  8\n", 11);
    displayColoredText("        \t\t 8  •         Welcome to your Personalised Social engine!         •  8\n", 7);
    displayColoredText("        \t\t 8  •                                                             •  8\n", 13);
    displayColoredText("        \t\t 8  •                        Developed by:                        •  8\n", 14);
    displayColoredText("        \t\t 8  •                                                             •  8\n", 14);
    displayColoredText("        \t\t 8  •             Preet Mahesh, Jiya Lal, Abdul Wasay             •  8\n", 12);
    displayColoredText("        \t\t 8  •                                                             •  8\n", 11);
    displayColoredText("        \t\t 8  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  8\n", 3);
    displayColoredText("        \t\t ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", 3);
    cout << "\n\n\n\n";
    cin.get();
}

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
    getline(cin, newUser.bio);

    for (int i = 0; i < 5; i++) {
        cout << "\t\tAdd at least 5 interests: ";
        getline(cin, newUser.interest[i]);
    }



    cout << "\n\t\tDo you want your profile to be public? (1 = Yes, 0 = No): ";
    cin >> isPublic;

    newUser.username = username;
    newUser.password = password;
    newUser.displayName = displayName;
    newUser.isPublic = isPublic;
    newUser.userId = ++lastUserId;

    users.insert(username, newUser);
    saveUsers();

    displayColoredText("\n\t\tSign Up successful! You can now log in.\n", 10);
    cin.ignore();
    cin.get();
    displayMainMenu();
}

// --------- Login ---------
void login() {
    system("cls");
    displayColoredText("\n\n\t\t           - LOGIN PAGE - \n", 12);
    displayColoredText("\t\t----------------------------------- \n\n", 12);
    string username, password;
    cout << "\t\tEnter your username: ";
    cin >> username;
    cout << "\t\tEnter your password: ";
    cin >> password;

    User* userPtr = users.find(username);
    if (userPtr && userPtr->password == password) {
        displayColoredText("Login successful!\n", 10);
        User currentUser = *userPtr;
        profileMenu(currentUser);
        users.insert(username, currentUser);
    }
    else {
        displayColoredText("\n\t\tInvalid username or password. Try again.\n", 12);
        cin.ignore();
        cin.get();
        displayMainMenu();
    }
}

// --------- Profile Display ---------
void profileDisplay(User& user) {
    displayColoredText("\n\n\t\t       - PROFILE DASHBOARD - \n", 14);
    displayColoredText("\t\t----------------------------------- \n\n", 14);

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

    cout << "\t\tPosts: " << user.posts.size()
        << " | Friends: " << user.friends.size;
        //<< " | Following: " << user.following.size << "\n";
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
    cout << "\t\t8. Add Friends\n"; // New option
    cout << "\t\t9. View Notifications\n"; // New option
    cout << "\t\t10. Logout\n\n";
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
        explorePosts(user);
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
        makeFriends(user);
        break;
    case 9:
        viewNotifications(user);
        break;
    case 10:
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

    string tags[5];

    for (int i = 0; i < 5; i++)
    {

        cout << "\t\tAdd atleast 5 tags for better reach:\n\t\t";
        getline(cin, tags[i]);
    }


    Post newPost(++lastPostId, user.username, content, tags);
    posts.insert(newPost.postId, newPost);
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
    char choice = - 1;
    
    posts.forEach([&](Post& post) {
        
        if (user.friends.contains(post.username)) {
            cout << "\t\t" << post.username << ": " << post.content
                << " | Likes: " << post.likes << "\n";
        }
        
    

        });
      
       

    cout << "\n\t\tPress Enter to return...";
    cin.ignore();
    cin.get();
    profileMenu(user);
}

// --------- Explore Posts ---------
void explorePosts(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t          - EXPLORE POSTS - \n", 11);
    displayColoredText("\t\t----------------------------------- \n\n", 11);

    vector<Post> postsWithPriority;

    posts.forEach([&](Post& post) {
        int count = 0; 
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                if (user.interest[i] == post.keywords[j]) {
                    count++;
                }
            }
        }
        post.priority = count;
        postsWithPriority.push_back(post); 
        });

    
    int n = postsWithPriority.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (postsWithPriority[j].priority < postsWithPriority[j + 1].priority) {
                Post temp = postsWithPriority[j];
                postsWithPriority[j] = postsWithPriority[j + 1];
                postsWithPriority[j + 1] = temp;
            }
        }
    }

   
    char choice;
    for (int i = 0; i < n; ++i) {
        Post& p = postsWithPriority[i];
        cout << "\n\t\tAuthor | " << p.username
            << " | Content: " << p.content
            << " | Likes: " << p.likes
            << " | Priority: " << p.priority << "\n";

        //cout << "\t\tLike = 'L', Comment = 'C', Save = 'S', Skip = any other key: ";
        //cin >> choice;
        //switch (choice) {
        //case 'L':
        //case 'l':
        //    likePost(user, p.postId);
        //    break;
        //case 'C':
        //case 'c':
        //    commentOnPost(user, p.postId);
        //    break;
        //case 'S':
        //case 's':
        //    savePost(user, p.postId);
        //    break;
        //default:
        //    break;
        //}
    }

    cout << "\n\t\tPress Enter to return...";
    cin.ignore();
    cin.get();
    profileMenu(user);
}

// --------- Like Post ---------
void likePost(User& user, int postId) {
    system("cls");

    Post* post = posts.find(postId);
    if (post) {
        post->likes++;
        displayColoredText("You liked the post!\n", 10);
    }
    else {
        displayColoredText("Post not found.\n", 12);
    }
    savePosts();

}

// --------- Comment on Post ---------
void commentOnPost(User& user, int postId) {
    system("cls");
    cout << "Comment on Post\n";
    string comment;
    cout << "Enter your comment: ";
    cin.ignore();
    getline(cin, comment);

    Post* post = posts.find(postId);
    if (post) {
        post->comments.push_back(comment);
        displayColoredText("Comment added!\n", 10);
    }
    else {
        displayColoredText("Post not found.\n", 12);
    }
    savePosts();

}

// --------- Save Post ---------
void savePost(User& user, int postId) {
    system("cls");
   
    Post* post = posts.find(postId);
    if (post) {
        user.savedPosts.insert(postId);
        displayColoredText("Post saved!\n", 10);
    }
    else {
        displayColoredText("Post not found.\n", 12);
    }

}

// --------- View Saved Posts ---------
void viewSavedPosts(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t        - SAVED POSTS - \n", 14);
    displayColoredText("\t\t-----------------------------------\n\n", 14);

    if (user.savedPosts.size == 0) {
        displayColoredText("\t\tNo saved posts.\n", 12);
    }
    else {
        for (int i = 0; i < user.savedPosts.size; i++) {
            int postId = user.savedPosts.int_elements[i];
            Post* post = posts.find(postId);
            if (post) {
                cout << "\t\t[" << post->postId << "] " << post->username << ": " << post->content
                    << " | Likes: " << post->likes << "\n";
            }
        }
    }

    cout << "\n\t\tPress Enter to return...";
    cin.ignore();
    cin.get();
    profileMenu(user);
}

// --------- Send Message ---------
void sendMessage(User& user) {
    system("cls");
    cout << "Send Message\n";
    string toUser, content;
    cout << "Enter the username to send a message to: ";
    cin >> toUser;
    cout << "Enter your message: ";
    cin.ignore();
    getline(cin, content);

    User* recipient = users.find(toUser);
    if (recipient) {
        Message msg = { user.username, toUser, content };
        recipient->messages.push(msg);
        displayColoredText("Message sent successfully!\n", 10);
    }
    else {
        displayColoredText("User not found.\n", 12);
    }
    profileMenu(user);
}

// --------- View Messages ---------
void viewMessages(User& user) {
    system("cls");
    cout << "View Messages\n";
    cout << "Your Messages:\n";
    while (!user.messages.empty()) {
        Message msg = user.messages.front();
        user.messages.pop();
        cout << "From: " << msg.fromUser << " | To: " << msg.toUser << "\n";
        cout << "Message: " << msg.content << "\n";
    }
    profileMenu(user);
}

// --------- Add Friends ---------
//void addFriends(User& user) {
//    system("cls");
//    displayColoredText("\n\n\t\t          - ADD FRIENDS - \n", 11);
//    displayColoredText("\t\t----------------------------------- \n\n", 11);
//
//    string targetUsername;
//    cout << "\t\tEnter the username to follow: ";
//    cin >> targetUsername;
//
//    if (targetUsername == user.username) {
//        displayColoredText("\t\tYou cannot follow yourself!\n", 12);
//        cin.ignore();
//        cin.get();
//        profileMenu(user);
//        return;
//    }
//
//    User* targetUser = users.find(targetUsername);
//    if (!targetUser) {
//        displayColoredText("\t\tUser not found.\n", 12);
//        cin.ignore();
//        cin.get();
//        profileMenu(user);
//        return;
//    }
//
//    if (user.following.contains(targetUsername)) {
//        displayColoredText("\t\tYou are already following this user!\n", 12);
//        cin.ignore();
//        cin.get();
//        profileMenu(user);
//        return;
//    }
//
//    if (targetUser->isPublic) {
//        // Directly follow public account
//        user.following.insert(targetUsername);
//        targetUser->followers.insert(user.username);
//        users.insert(user.username, user);
//        users.insert(targetUsername, *targetUser);
//        saveUsers();
//        displayColoredText("\t\tYou are now following " + targetUsername + "!\n", 10);
//    }
//    else {
//        // Send follow request for private account
//        string notification = user.username + " sent you a follow request";
//        targetUser->notifications.enqueue(notification);
//        users.insert(targetUsername, *targetUser);
//        saveUsers();
//        displayColoredText("\t\tFollow request sent to " + targetUsername + "!\n", 10);
//    }
//
//    cin.ignore();
//    cin.get();
//    profileMenu(user);
//}

// --------- Make Friends ---------
void makeFriends(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t          - ADD FRIENDS - \n", 11);
    displayColoredText("\t\t----------------------------------- \n\n", 11);

 
    string targetUsername;
    cout << "Enter username: ";
    cin >> targetUsername;
    if (targetUsername == user.username) {
        displayColoredText("\t\tYou cannot follow yourself!\n", 12);
        cin.ignore();
        cin.get();
        profileMenu(user);
        return;
    }

    User* targetUser = users.find(targetUsername);
    if (!targetUser) {
        displayColoredText("\t\tUser not found.\n", 12);
        cin.ignore();
        cin.get();
        profileMenu(user);
        return;
    }
    else {
        if (!user.friends.contains(targetUsername))
        {
            friendShips[user.userId].push_back(targetUser->username);
            friendShips[targetUser->userId].push_back(user.username);
            user.friends.insert(targetUsername);
        }
    }


        cout << "Friends of : " << user.username << "\n";

        for (int i = 0; i < friendShips[user.userId].size(); i++) {
               cout << "- " << friendShips[user.userId][i] << "\n";
        }
              

    cin.ignore();
    cin.get();
    profileMenu(user);
}


// --------- View Notifications ---------
void viewNotifications(User& user) {
    system("cls");
    displayColoredText("\n\n\t\t        - NOTIFICATIONS - \n", 14);
    displayColoredText("\t\t-----------------------------------\n\n", 14);

    if (user.notifications.empty()) {
        displayColoredText("\t\tNo notifications.\n", 12);
    }
    else {
        while (!user.notifications.empty()) {
            string notification = user.notifications.dequeue();
            cout << "\t\t" << notification << "\n";
        }
        // Update user to clear notifications
        users.insert(user.username, user);
        saveUsers();
    }

    cout << "\n\t\tPress Enter to return...";
    cin.ignore();
    cin.get();
    profileMenu(user);
}

// --------- Logout ---------
void logout(User& user) {
    displayColoredText("\n\n\t\tAre you sure you want to logout? (1 = Yes, 2 = No): ", 14);
    int choice;
    cin >> choice;

    if (choice == 1) {
        cout << "\t\tLogging out...\n";
        Sleep(1000);
        saveUsers();
        savePosts();
        displayMainMenu();
    }
    else if (choice == 2) {
        profileMenu(user);
    }
    else {
        displayColoredText("Invalid choice, please enter 1 or 2.\n", 12);
        logout(user);
    }
}

// --------- Load Users ---------
void loadUsers() {
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "Error loading users file!" << endl;
        return;
    }
    string line;
    while (getline(userFile, line)) {
        stringstream ss(line);
        int userId;
        string username, password, displayName, bio, interest[5];
        bool isPublic;
        int notificationCount;
      
        ss >> userId >> username >> password >> displayName >> isPublic >> notificationCount;
        getline(ss >> ws, bio); // Skip whitespace and read bio

        User user = { userId };
        user.username = username;
        user.password = password;
        user.displayName = displayName;
        user.isPublic = isPublic;
        user.bio = bio;
        for (int i = 0; i < 5; i++)
        {
            ss >> user.interest[i];
        }
        // Load notifications
        for (int i = 0; i < notificationCount; i++) {
            getline(userFile, line);
            user.notifications.enqueue(line);
        }

        users.insert(username, user);
        if (user.userId > lastUserId) {
            lastUserId = user.userId;
        }
    }
    userFile.close();
}

// --------- Save Users ---------
void saveUsers() {
    ofstream userFile("users.txt");
    if (!userFile) {
        cerr << "Error saving users file!" << endl;
        return;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        UserHashTable::Bucket& bucket = users.table[i];
        for (int j = 0; j < bucket.size; j++) {
            if (bucket.entries[j].isUsed) {
                User& user = bucket.entries[j].value;
                userFile << user.userId << " " << user.username << " " << user.password << " " << user.displayName << " "
                    << user.isPublic << " " << user.notifications.getSize() << " " << user.bio << " ";
                for (int i = 0; i < 5; i++)
                {
                    userFile << user.interest[i] << " ";
                }
                userFile << "\n";
                // Save notifications
                CustomQueue tempQueue = user.notifications;
                while (!tempQueue.empty()) {
                    userFile << tempQueue.dequeue() << "\n";
                }
            }
        }
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
        string username, content, tags[5];
        ss >> postId;
        ss.ignore(); // Skip delimiter
        getline(ss, username);
        getline(ss, content);
        for (int i = 0; i < 5; i++)
        {
            getline(ss, tags[i]);

        }
        Post post = { postId, username, content, tags };
        posts.insert(postId, post);
        if (postId > lastPostId) lastPostId = postId;
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
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        PostHashTable::Bucket& bucket = posts.table[i];
        for (int j = 0; j < bucket.size; j++) {
            if (bucket.entries[j].isUsed) {
                Post& post = bucket.entries[j].value;
                postFile << post.postId << " " << post.username << " " << post.content << " ";
                for (int i = 0; i < 5; i++)
                {
                    postFile << post.keywords[i] << " ";
                }
                postFile << "\n";
            }
        }
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