#pragma once

#include <stdlib.h>

namespace std {

	struct RefCountIsland {
		RefCountIsland() : mRefCount(1) {}
		
		void retain() { ++mRefCount; }
		void release() { --mRefCount; }
		
		size_t mRefCount;
	};

	template<class T>
	class shared_ptr
	{
	public:		
		shared_ptr(T* ptr, std::RefCountIsland *ref = NULL) : mPtr(ptr), mRef(ref) {
			if (mRef) {
				mRef->retain();
			} else {
				mRef = new RefCountIsland;
			}
		}
		shared_ptr() : mPtr(NULL), mRef(NULL) {}
		shared_ptr(const shared_ptr<T> &original) {
			mPtr = original.mPtr;
			mRef = original.mRef;
			if (mRef) {
				mRef->retain();
			}
		}
		~shared_ptr() {
			if (mRef) {
				mRef->release();
				if (mRef->mRefCount == 0) {
					delete mRef;
					mRef = NULL;
					delete mPtr;
				}
			}
		}
		
		T* operator * () const { return mPtr; }
		T* operator -> () const { return mPtr; }
		T* operator * () { return mPtr; }
		T* operator -> () { return mPtr; }
		
		shared_ptr<T>& operator = (const shared_ptr<T> &original) {
			if (mRef == original.mRef) { return *this; }
			if (mRef) {
				mRef->release();
				if (mRef->mRefCount == 0) {
					delete mRef;
					mRef = NULL;
					delete mPtr;
				}
			}
			mRef = original.mRef;
			mPtr = original.mPtr;
			if (mRef) {
				mRef->retain();
			}
			return *this;
		}
		
		T* get() const { return mPtr; }
		
		RefCountIsland* ref() const { return mRef; }
		
	protected:
		RefCountIsland* mRef;
		T* mPtr;
	};
	
	template<class T, class U>
	shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U> &original) {
		T * rawPtr = dynamic_cast<T*>(original.get());
		if (!rawPtr) {
			return shared_ptr<T>();
		}
		return shared_ptr<T>(rawPtr, original.ref());
	}
	
	template<class T, class U>
	shared_ptr<T> static_pointer_cast(const shared_ptr<U> &original) {
		T * rawPtr = static_cast<T*>(original.get());
		if (!rawPtr) {
			return shared_ptr<T>();
		}
		return shared_ptr<T>(rawPtr, original.ref());
	}

}