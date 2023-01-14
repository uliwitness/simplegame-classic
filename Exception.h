#pragma once

#include <exception>
#include <stdio.h>
#include <string.h>


class Exception : public std::exception {
public:
	Exception(OSStatus err) : mErr(err) {}
	
	virtual const char* what() const {
		static char buf[1024] = {0};
		CharsHandle errMsg = (CharsHandle) GetResource('ERRS', mErr);
		if (!errMsg) {
			snprintf(buf, sizeof(buf) -1, "Error: %ld", mErr);
		} else {
			strncpy(buf, (*errMsg), sizeof(buf) -1);
		}
		return buf;
	}
	
protected:
	OSStatus mErr;
};