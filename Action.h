#pragma once

class ActionBase {
public:
	virtual ~ActionBase() {}

	virtual void operator () (CommandHandler* sender) = 0;
};

template<class T>
class Action : public ActionBase {
public:
	Action(T* target, void (T::*action)(CommandHandler*))
	: mTarget(target), mAction(action) {}
	
	virtual void operator () (CommandHandler* sender) { (*mTarget.*(mAction))(sender); }

protected:
	void (T::*mAction)(CommandHandler*);
	T* mTarget;
};
