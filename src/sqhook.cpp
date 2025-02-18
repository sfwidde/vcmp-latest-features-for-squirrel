/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-12-04
 */

#include "sqhook.hpp"
#include <squirrel/SQModule.h>

extern HSQAPI sq;

// -----------------------------------------------------------------------------

void SQClassHook::AddSqratProperty(SQFUNCTION function, const SQChar* propertyName, const SQChar* sqratTableName) const
{
	if (!v) { return; }

	// Attempt to grab __setTable or __getTable table from the class
	sq->pushstring(v, sqratTableName, -1);
	if (SQ_FAILED(sq->get(v, -2))) { return; }

	// Add a new closure to this table
	sq->pushstring(v, propertyName, -1);
	sq->newclosure(v, function, 0);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
}

SQClassHook::SQClassHook(HSQUIRRELVM v, const SQChar* className)
{
	// Find out if identifier exists
	sq->pushroottable(v);
	sq->pushstring(v, className, -1);
	if (SQ_FAILED(sq->get(v, -2)))
	{
		sq->pop(v, 1);
	failure:
		this->v = nullptr; // This object is pretty much useless now
		return;
	}

	// Identifier is not of class type
	if (sq->gettype(v, -1) != OT_CLASS)
	{
		sq->pop(v, 2);
		goto failure;
	}

	// Success
	this->v = v;
}

SQClassHook::~SQClassHook()
{
	if (v) { sq->pop(v, 2); }
}

void SQClassHook::AddMethod(SQFUNCTION function, const SQChar* methodName, const SQChar* parameterMask, SQInteger parameterCount) const
{
	if (!v) { return; }

	sq->pushstring(v, methodName, -1);
	sq->newclosure(v, function, 0);
	sq->setparamscheck(v, parameterCount, parameterMask);
	sq->setnativeclosurename(v, -1, methodName);
	sq->newslot(v, -3, SQFalse);
}

void SQClassHook::AddSqratProperty(SQFUNCTION setterFunction, SQFUNCTION getterFunction, const SQChar* propertyName) const
{
	AddSqratProperty(setterFunction, propertyName, _SC("__setTable"));
	AddSqratProperty(getterFunction, propertyName, _SC("__getTable"));
}

// The following function was taken from here:
// https://forum.vc-mp.org/index.php?topic=206.msg3206#msg3206
// it is, however, slightly modified for convenience
void RegisterSquirrelFunction(HSQUIRRELVM v, SQFUNCTION function, const SQChar* functionName,
	const SQChar* parameterMask, SQInteger parameterCount)
{
	sq->pushroottable(v);
	sq->pushstring(v, functionName, -1);
	sq->newclosure(v, function, 0);
	sq->setparamscheck(v, parameterCount, parameterMask);
	sq->setnativeclosurename(v, -1, functionName);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
}

bool IsSquirrelObjectInstanceOfClass(SQInteger objectIndex, const SQClassHook& sqClass)
{
	if (!sqClass.v) { return false; }

	bool x;
	sq->push(sqClass.v, objectIndex);
	x = !!sq->instanceof(sqClass.v);
	sq->pop(sqClass.v, 1);
	return x;
}

// -----------------------------------------------------------------------------
