/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-12-04
 */

#include "sqhook.hpp"
#include <squirrel/SQModule.h>

extern HSQAPI      sq;
extern HSQUIRRELVM v;

// -----------------------------------------------------------------------------
// SQClassHook CLASS
// -----------------------------------------------------------------------------

SQClassHook::SQClassHook(const SQChar* className)
{
	// Find out if identifier exists within the root table
	sq->pushroottable(v);
	sq->pushstring(v, className, -1);
	if (SQ_FAILED(sq->get(v, -2)))
	{
		sq->pop(v, 1);
	failure:
		m_valid = false; // This object is pretty much useless now
		return;
	}

	// Identifier is not of class type
	if (sq->gettype(v, -1) != OT_CLASS)
	{
		sq->pop(v, 2);
		goto failure;
	}

	// Class with such name exists
	// and is ready to be operated on
	m_valid = true;
}

SQClassHook::~SQClassHook()
{
	// Just pop what we pushed earlier
	// as we are done with this class
	if (m_valid) { sq->pop(v, 2); }
}

void SQClassHook::AddMethod(SQFUNCTION function, const SQChar* methodName, const SQChar* parameterMask, SQInteger parameterCount) const
{
	if (!m_valid) { return; }

	sq->pushstring(v, methodName, -1);
	sq->newclosure(v, function, 0);
	sq->setparamscheck(v, parameterCount, parameterMask);
	sq->setnativeclosurename(v, -1, methodName);
	sq->newslot(v, -3, SQFalse);
}

void SQClassHook::AddSqratProperty(SQFUNCTION function, const SQChar* propertyName, const SQChar* sqratTableName) const
{
	// Attempt to grab __setTable or __getTable table from the class
	sq->pushstring(v, sqratTableName, -1);
	if (SQ_FAILED(sq->get(v, -2))) { return; }

	// Add a new closure (function) to this table
	sq->pushstring(v, propertyName, -1);
	sq->newclosure(v, function, 0);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
}

void SQClassHook::AddSqratProperty(SQFUNCTION setterFunction, SQFUNCTION getterFunction, const SQChar* propertyName) const
{
	if (!m_valid) { return; }

	AddSqratProperty(setterFunction, propertyName, _SC("__setTable"));
	AddSqratProperty(getterFunction, propertyName, _SC("__getTable"));
}

bool SQClassHook::IsObjectInstanceOfThis(SQInteger objectIndex) const
{
	if (!m_valid) { return false; }

	bool x;
	sq->push(v, objectIndex);
	x = !!sq->instanceof(v);
	sq->pop(v, 1);
	return x;
}

// -----------------------------------------------------------------------------

// The following function was taken from here:
// https://forum.vc-mp.org/index.php?topic=206.msg3206#msg3206
// it is, however, slightly modified for convenience
void RegisterSquirrelFunction(SQFUNCTION function, const SQChar* functionName,
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
