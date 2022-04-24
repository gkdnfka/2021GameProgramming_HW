#include "stu.h"

bool _stu::operator<(_stu a) const{
	return score < a.score;
}

bool _stu::operator>(_stu a) const{
	return score > a.score;
}

bool _stu::operator<=(_stu a) const{
	return score <= a.score;
}

bool _stu::operator>=(_stu a) const {
	return score >= a.score;
}