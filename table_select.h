#pragma once
#include <vector>
#include<conio.h>
#include <iomanip>
#include <iostream>
using namespace std;
struct uiTable {
	vector<string> selections;
	int index = 0;
	int maxStringSize = 0;
	void configTable(vector<string> inputSelections) {
		for (int i = 0; i < inputSelections.size(); i++) {
			selections.push_back(inputSelections[i]);
			if (inputSelections[i].length() + 1 > maxStringSize) {
				maxStringSize = inputSelections[i].length() + 1;
			}
		}
	}
	void print() {
		string definer = "<------";
		for (int i = 0; i < selections.size(); i++) {
			cout << selections[i];
			cout << setw(maxStringSize - selections[i].length()) << " ";
			if (i == index) {
				cout << definer;
			}
			cout << endl;
		}
	}

	int testInput(char up, char down, bool waitsforinput) {
		if (_kbhit() || waitsforinput) {
			char keyPressed = _getch();
			if (keyPressed == up) {
				if (index > 0) {
					index = index - 1;
				}
			}
			if (keyPressed == down) {
				if (index < selections.size() - 1) {
					index = index + 1;
				}
			}
			return keyPressed;
		}
		else {
			return 0;
		}
	}
};