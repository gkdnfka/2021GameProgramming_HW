#pragma once
#include "stdafx.h"

typedef struct _stu {
	std::wstring name;
	int score;

	bool operator<(_stu a) const;
	bool operator>(_stu a) const;
	bool operator<=(_stu a) const;
	bool operator>=(_stu a) const;

	_stu(std::wstring pname, int pscore):name(pname), score(pscore) {}
}stu;