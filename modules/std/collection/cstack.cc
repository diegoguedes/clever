/**
 * Clever programming language
 * Copyright (c) Clever Team
 *
 * This file is distributed under the MIT license. See LICENSE for details.
 */

#include "core/type.h"
#include "modules/std/core/function.h"
#include "modules/std/collection/cstack.h"

namespace clever { namespace modules { namespace std {

// Stack.Stack()
CLEVER_METHOD(CStack::ctor)
{
	if (!clever_check_no_args()) {
		return;
	}

	result->setObj(this, new CStackObject);
}

// Stack.push()
CLEVER_METHOD(CStack::push)
{
	if (!clever_check_args(".")) {
		return;
	}

	CStackObject* sobj = clever_get_this(CStackObject*);

	sobj->stack.push(args[0]->clone());
}

// Stack.pop()
CLEVER_METHOD(CStack::pop)
{
	if (!clever_check_no_args()) {
		return;
	}

	CStackObject* sobj = clever_get_this(CStackObject*);

	if (!sobj->stack.empty()) {
		clever_delref(sobj->stack.top());
		sobj->stack.pop();
	}
}

// Stack.top()
CLEVER_METHOD(CStack::top)
{
	if (!clever_check_no_args()) {
		return;
	}

	const CStackObject* sobj = clever_get_this(CStackObject*);

	if (sobj->stack.empty()) {
		result->setNull();
	} else {
		result->copy(sobj->stack.top());
	}
}

// Stack.size()
CLEVER_METHOD(CStack::size)
{
	if (!clever_check_no_args()) {
		return;
	}

	const CStackObject* sobj = clever_get_this(CStackObject*);

	result->setInt(sobj->stack.size());
}

// Stack.empty()
CLEVER_METHOD(CStack::empty)
{
	if (!clever_check_no_args()) {
		return;
	}

	const CStackObject* sobj = clever_get_this(CStackObject*);

	result->setBool(sobj->stack.empty());
}

// Stack type initialization
CLEVER_TYPE_INIT(CStack::init)
{
	setConstructor((MethodPtr)&CStack::ctor);

	addMethod(new Function("push",  (MethodPtr)&CStack::push));
	addMethod(new Function("pop",   (MethodPtr)&CStack::pop));
	addMethod(new Function("top",   (MethodPtr)&CStack::top));
	addMethod(new Function("size",  (MethodPtr)&CStack::size));
	addMethod(new Function("empty", (MethodPtr)&CStack::empty));
}

}}} // clever::modules::std
