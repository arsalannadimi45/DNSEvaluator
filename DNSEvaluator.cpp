#include "DNSEvaluator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

using namespace std;

// Function to save DNS info
void AddDNS(const string& name, const string& preferredDNS, const string& alternateDNS)
{
	// Open the file in append mode
	ofstream file("saved_dns.txt", ios::app);

	if (file.is_open()) {
		// Combine name, preferred, and alternate DNS in a formatted string
		file << "Name: " << name << "*preferred DNS: " << preferredDNS << "**alternate DNS: " << alternateDNS << endl;
		cout << "DNS saved for " << name << "!" << endl;
	}
	else {
		cerr << "Error opening file!" << endl;
	}
}

// Function to clear the DNS settings and set to "Obtain DNS server address automatically"
void ResetSystemDNS() {
	cout << "Clearing DNS settings and setting to Obtain DNS server address automatically...\n";

	// Execute system command to set DNS to automatic
	int result = system("netsh interface ip set dns name=\"Ethernet\" source=dhcp");

	// Check if the system command was successful
	if (result == 0) {
		cout << "DNS settings cleared successfully. The system will now obtain DNS server address automatically.\n";
	}
	else {
		cerr << "Failed to clear DNS settings. Please ensure you have the necessary permissions.\n";
	}
}

// Function to set DNS
void ApplyDNSToSystem(const string& preferredDNS, const string& alternateDNS)
{
	// Clear DNS from System First
	ResetSystemDNS();

	// Construct the system command to set DNS
	string commandpreferred = "netsh interface ip set dns name=\"Ethernet\" static " + preferredDNS;
	string commandalternate = "netsh interface ip add dns name=\"Ethernet\" " + alternateDNS + " index=2";

	// Execute the system commands
	if (system(commandpreferred.c_str()) == 0) {
		cout << "Preferred DNS set to: " << preferredDNS << endl;
	}
	else {
		cout << "Failed to set Preferred DNS." << endl;
	}

	if (system(commandalternate.c_str()) == 0) {
		cout << "Alternate DNS set to: " << alternateDNS << endl;
	}
	else {
		cout << "Failed to set Alternate DNS." << endl;
	}

}

// Function to display all saved DNS entries and allow the user to set DNS by index
void ShowSavedDNSEntries()
{
	ifstream file("saved_dns.txt");
	string line;
	vector<string> savedDNS;  // To store DNS entries

	if (file.is_open()) {
		cout << "Saved DNS Entries:\n";

		// Read all saved DNS entries and store them in a vector
		int index = 0;
		while (getline(file, line)) {
			savedDNS.push_back(line);
			cout << index++ << ". " << line << endl;  // Display the index and the DNS entry
		}

		file.close();

		if (savedDNS.empty()) {
			cout << "No saved DNS entries found.\n";
			return;
		}

		// Prompt user to select an index to set DNS
		int selectedIndex;
		cout << "Enter the index of the DNS you want to set: ";
		cin >> selectedIndex;

		// Check if the entered index is valid
		if (selectedIndex >= 0 && selectedIndex < savedDNS.size()) {

			// FILE FORMAT:
			//  Name: Electro *preferred DNS: preferredDNS **alternate DNS: alternateDNS;

			// Split the selected entry to get the DNS information
			string selectedDNS = savedDNS[selectedIndex];
			size_t preferredPos = selectedDNS.find("preferred DNS: ") + 15;
			size_t alternatePos = selectedDNS.find("alternate DNS: ") + 15;
			size_t preferredEndPos = selectedDNS.find("**");

			// Extract preferred and alternate DNS from the selected string
			string preferredDNS = selectedDNS.substr(preferredPos, preferredEndPos - preferredPos);
			string alternateDNS = selectedDNS.substr(alternatePos);

			// Call the SetDNS function with the extracted DNS values
			ApplyDNSToSystem(preferredDNS, alternateDNS);
		}
		else {
			cout << "Invalid index, please try again.\n";
		}
	}
	else {
		cerr << "Error opening file!" << endl;
	}
}

// Function to delete all of the saved dns entries.
void DeleteAllSavedDNSEntries() {
	ofstream file("saved_dns.txt", ios::trunc); // Clear the contents
	if (file.is_open()) {
		file.close();
		cout << "DNS list cleared!" << endl;
	}
	else {
		cerr << "Error opening file!" << endl;
	}
}

int main()
{
	string name, preferredDNS, alternateDNS;
	int choice;

	while (true) {
		cout << "--------------------------------------------------------\n";
		cout << "1. Add DNS\n";
		cout << "2. Show Saved DNS Entries\n";
		cout << "3. Reset System DNS\n";
		cout << "4. Delete All Saved DNS Entries\n";
		cout << "5. Exit\n";
		cout << "Choose an option: ";
		cin >> choice;
		cin.ignore(); // To ignore the newline character left in the input buffer

		if (choice == 1) {
			// Prompt the user for name and DNS addresses
			cout << "Enter DNS Entry Name: ";
			getline(cin, name);  // Use getline to allow spaces in the name
			cout << "Enter Preferred DNS: ";
			getline(cin, preferredDNS);
			cout << "Enter Alternate DNS: ";
			getline(cin, alternateDNS);

			// Save the DNS info to the file
			AddDNS(name, preferredDNS, alternateDNS);
		}
		else if (choice == 2) {
			// Display all saved DNS entries
			ShowSavedDNSEntries();
		}
		else if (choice == 3) {
			// Display all saved DNS entries
			ResetSystemDNS();
		}
		else if (choice == 4) {
			cout << "Enter CONFIRM to Proceed: " << endl;
			string confirmation;
			cin >> confirmation;
			if (confirmation == "CONFIRM") {
				DeleteAllSavedDNSEntries();
			}
			else {
				cout << "Operation Canceled." << endl;
			}
		}
		else if (choice == 5) {
			cout << "Exiting program...\n";
			break;
		}
		else {
			cout << "Invalid option, please try again.\n";
		}
	}

	return 0;
}

