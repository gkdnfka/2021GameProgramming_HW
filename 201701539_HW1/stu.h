#pragma once
#include <string>
#include <vector>

typedef struct _stu {
	std::string name;
	int score;

	bool operator<(_stu a) const;
	bool operator>(_stu a) const;
	bool operator<=(_stu a) const;
	bool operator>=(_stu a) const;

	_stu(std::string pname, int pscore):name(pname), score(pscore) {}
}stu;