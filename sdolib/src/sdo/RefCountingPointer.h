/* 
+----------------------------------------------------------------------+
| (c) Copyright IBM Corporation 2005.                                  | 
| All Rights Reserved.                                                 |
+----------------------------------------------------------------------+ 
|                                                                      | 
| Licensed under the Apache License, Version 2.0 (the "License"); you  | 
| may not use this file except in compliance with the License. You may | 
| obtain a copy of the License at                                      | 
|  http://www.apache.org/licenses/LICENSE-2.0                          |
|                                                                      | 
| Unless required by applicable law or agreed to in writing, software  | 
| distributed under the License is distributed on an "AS IS" BASIS,    | 
| WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      | 
| implied. See the License for the specific language governing         | 
| permissions and limitations under the License.                       | 
+----------------------------------------------------------------------+ 
| Author: Ed Slattery                                                  | 
+----------------------------------------------------------------------+ 

*/
/* $Id$ */

#ifndef _REFCOUNTINGPOINTER_H
#define _REFCOUNTINGPOINTER_H

#include "export.h"
#include <iostream>
using namespace std;

namespace commonj{
namespace sdo{

template <class T>
class RefCountingPointer {
	public:
		SDO_API RefCountingPointer(T* realPtr = 0);
		SDO_API RefCountingPointer(const RefCountingPointer& rhs);
		SDO_API ~RefCountingPointer();
		SDO_API RefCountingPointer& operator=(const RefCountingPointer& rhs);
		SDO_API bool operator==(RefCountingPointer& test) const;
		SDO_API T* operator->() const;
		SDO_API T& operator*() const;
		SDO_API bool operator!() const;

#ifdef MFT
		// MFT == member function templates
		// Notes on the items below.
		// In our code, we use subclasses to expose the API, and super
		// classes to implement. E,g DataObject and DataObjectImpl.
		// In some cases, we know that the DataObject given to us is a 
		// DataObjectImpl, and cast it. With RefCountingPointers, however,
		// the cast cannot work, as the RefCountingPointer to the superclass
		// is not related to the RCP to the subclass. Recent changes in the
		// C++ language allow this to work by defining an operator which 
		// causes a pointer of the other type to be returned, as long as pointee
		// is acceptable as a parameter to the cosntructor of the other type 
		// of pointer. This works in C++.NET, but not in C++6:
		operator RefCountingPointer<otherType>()
		{ 
			 return RefCountingPointer<otherType>(pointee);
		}

		// Since we are using C6, a possible workround is to provide a method
		// which returns the dumb pointer, then construct a pointer to the 
		// base class from the pointer returned. This is that the operator T* does.
		// The code in DataObject could be simpler if we used C7,and we should
		// discusss changing.
#else 
		operator T*() {return pointee;}
#endif

		template <class otherType>
		operator RefCountingPointer<otherType>()
		{
			return RefCountingPointer<otherType>(pointee);
		}

	private:
		T *pointee;
		void init();
};



template<class T>
void RefCountingPointer<T>::init()
{
	//cout << "RCP to:" << pointee << " CREATED" << endl;
	if (pointee == 0) return;
	pointee->addRef();
}

template<class T>
SDO_API RefCountingPointer<T>::RefCountingPointer(T* realPtr)
:pointee(realPtr)
{
	init();
}

template<class T>
SDO_API RefCountingPointer<T>::RefCountingPointer(const RefCountingPointer& rhs)
: pointee(rhs.pointee)
{
	//cout << "RCP: ref counting pointer to " << pointee << "copied" << endl;
	init();
}

template<class T>
SDO_API RefCountingPointer<T>::~RefCountingPointer()
{
	//cout << "RCP to:" << pointee << " DELETED" << endl;
	if (pointee)pointee->releaseRef();
}

template<class T>
SDO_API RefCountingPointer<T>& RefCountingPointer<T>::operator=(const RefCountingPointer& rhs)
{
	//cout << "RCP to:" << pointee << " BEING COPIED" << endl;
	if (pointee != rhs.pointee)
	{
		T *oldP = pointee;
		pointee = rhs.pointee;
		init();
		if (oldP) oldP->releaseRef();
	}
	return *this;
}

template<class T>
SDO_API bool RefCountingPointer<T>::operator!() const
{
	return (pointee == 0);
}

template<class T>
SDO_API bool RefCountingPointer<T>::operator==(RefCountingPointer& test) const
{
	return (pointee == test.pointee);
}

template<class T>
SDO_API T* RefCountingPointer<T>::operator->() const 
{
	return pointee;
}

template<class T>
SDO_API T& RefCountingPointer<T>::operator*() const
{
	return *pointee;
}

class DataObject;
typedef RefCountingPointer<DataObject> DataObjectPtr;
class DASDataObject;
typedef RefCountingPointer<DASDataObject> DASDataObjectPtr;
class DASDataFactory;
typedef RefCountingPointer<DASDataFactory> DASDataFactoryPtr;
class DataFactory;
typedef RefCountingPointer<DataFactory> DataFactoryPtr;
class Sequence;
typedef Sequence* SequencePtr;
class ChangeSummary;
typedef ChangeSummary* ChangeSummaryPtr;
class XMLDocument;
typedef RefCountingPointer<XMLDocument> XMLDocumentPtr;
class XSDHelper;
typedef RefCountingPointer<XSDHelper> XSDHelperPtr;
class XMLHelper;
typedef RefCountingPointer<XMLHelper> XMLHelperPtr;

};
};

#endif
