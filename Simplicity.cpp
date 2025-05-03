#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
using namespace std;



struct Users
{
	string username;
	string password;
	unordered_set<string> friends;
	vector<string> interest;
};

struct  Feed
{
	string author;
	string text;
	int score;
};

struct Posts
{
	string author;
	string text;
	unordered_set<string> keywords;

	Posts* next;
};
//USER'S DB
unordered_map<string, Users> usersMap;

//POST'S DB
Posts* head = NULL;

bool findUser(string username) {

	if (usersMap.find(username) == usersMap.end())
	{
		return false;
	}
	return true;
}

void addFriends(Users& user, string friendName) {
	user.friends.insert(friendName);
	cout << "You and " << friendName << " are friends" << endl;
}




void createPost(string author, string text, unordered_set<string> tags) {
	Posts* post = new Posts();
	post->author = author;
	post->text = text;
	post->keywords = tags;

	if (head == NULL)
	{
		post->next = NULL;
		head = post;
	}
	else
	{

		post->next = head;
		head = post;
	}

};
void userFeed(Users& user) {
	Posts* posts = head;
	while (posts != NULL) {
		if (user.friends.count(posts->author) == 1)
		{
			cout << "Author: " << posts->author << endl;
			cout << "Text: " << posts->text << endl;
			cout << "Do you want to like (l) or comment (c) on this post? \n";
			cout << endl;

		}
		posts = posts->next;
	}
}


vector<Feed> recommendedFeed(Users& user) {

	Posts* ptr = head;
	vector<Feed> feed;
	while (ptr != NULL) {
		for (int i = 0; i < user.interest.size(); i++)
		{
			if (ptr->keywords.count(user.interest[i]))
			{
				feed.push_back({ ptr->author,  ptr->text, 0 });
				break;
			}
		}
		ptr = ptr->next;
	}
	return feed;
}

int main() {

	cout << "SIGNUP / LOGIN FIRST!\n";
	string username;
	string password;
	cout << "Enter username: ";
	cin.ignore();
	getline(cin, username);
	cout << "Enter password: ";
	getline(cin, password);

	Users temp = { "", "", {}, {} };
	Users& user = temp;

	if (!findUser(username)) {

		user.username = username;
		user.password = password;
		for (int i = 0; i < 5; i++)
		{
			string interest = "";
			user.interest.resize(5);
			cout << "Enter your interest/hobby/preference: ";
			cin >> interest;
			cout << endl;
			user.interest[i] = interest;

		}
		usersMap[username] = user;
		cout << "User registered!" << endl;


	}



	createPost("Jenna Michaels",
		"Just wrapped up a 10 - day digital detox and wow—my brain finally feels like it belongs to me again. Highly recommend taking a step back to reconnect with the real world.",
		{ "mindfulness", "health", "self - care" });
	createPost("Carlos Rivera",
		"Launched our new app today! After months of development, it’s surreal to finally see people using it. Thanks to everyone who helped bring this to life.",
		{ "app launch", "startup", "product development", "tech" });
	createPost("Aisha Nwosu",
		"Watched the sunrise from the top of Table Mountain this morning. Nature really does know how to show off.",
		{ "travel", "nature", "hiking", "photography" });
	createPost("Derek Kim",
		"Just finished reading *Atomic Habits*—small changes really *do* add up over time. I’ve already started tweaking my morning routine.",
		{ "productivity", "habits", "self - improvement", "reading" });
	createPost("Priya Mehta",
		"Baked my first sourdough loaf today and it actually turned out edible! The smell alone was worth the 12 - hour rise.",
		{ "baking", "sourdough", "home cooking", "quarantine hobbies" });

	int choice;


	cout << "1. Explore Page\n";
	cout << "2. Regular Feed\n";
	cout << "3. Request a Friend\n";
	cout << "4. Exit\n";
	cout << "Choose an option (1-6): ";
	cin >> choice;

	if (choice == 1)
	{
		vector<Feed> feed = recommendedFeed(user);

		cout << "Explore new ideas and thoughts!" << endl;
		for (int i = 0; i < feed.size(); i++)
		{
			cout << "Author: " << feed[i].author << endl;
			cout << "Text: " << feed[i].text << endl;
			cout << "Do you want to like (l) or comment (c) on this post? \n";
			cout << endl;
		}

	}
	else if (choice == 2)
	{
		cout << "See what your's friends share!" << endl;
		userFeed(user);
	}
	else if (choice == 3)
	{
		string username;
		cout << "Let's create your community! " << endl;
		cout << "Enter user whom you want to send request\n";
		cin >> username;
		if (usersMap.find(username) != usersMap.end())
		{
			addFriends(user, "");

		}
		else
		{
			cout << "User does not exist\n";
		}
	}

	else {
		cout << "Invalid Code!\n";
	}


	return 0;
}