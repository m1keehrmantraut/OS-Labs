#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#pragma warning(disable: 4996)

using namespace std;

struct message {
	char text[20];
	bool empty;

	message() : empty(true) {
		memset(text, 0, sizeof(text));
	};

	message(string s) : empty(false) {
		memset(text, 0, sizeof(text));
		if (s.length() >= 20) {
			strncpy(text, s.c_str(), 19);
		} else {
			strcpy(text, s.c_str());
		}
	}

	const char* get_text() const {
		return text;
	}

	bool is_empty() const {
		return empty;
	}

	friend ostream& operator <<(ostream& out, const message& m) {
		out.write((char*)&m, sizeof(m));
		return out;
	}

	friend istream& operator >>(istream& in, message& m) {
		in.read((char*)&m, sizeof(m));
		return in;
	}
};