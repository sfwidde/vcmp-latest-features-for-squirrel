/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-12-04
 */

#include "sqhook.h"
#include <squirrel/SQModule.h>

extern HSQAPI sq;

// -----------------------------------------------------------------------------

// Grab a class from the root table
//
// If successful, the class is pushed in the stack, otherwise nothing is done
static bool GetSquirrelClass(HSQUIRRELVM v, const SQChar* className)
{
	// Find out if identifier exists
	sq->pushroottable(v);
	sq->pushstring(v, className, -1);
	if (SQ_FAILED(sq->get(v, -2)))
	{
		sq->pop(v, 1);
		return false;
	}

	// Identifier is not of class type
	if (sq->gettype(v, -1) != OT_CLASS)
	{
		sq->pop(v, 2);
		return false;
	}

	return true;
}

static bool RegisterSqratClassProperty(HSQUIRRELVM v, SQFUNCTION function,
	const SQChar* className, const SQChar* propertyName, const SQChar* sqratTableName)
{
	if (!GetSquirrelClass(v, className)) { return false; }

	// Grab __setTable or __getTable table from the class
	sq->pushstring(v, sqratTableName, -1);
	if (SQ_FAILED(sq->get(v, -2)))
	{
		sq->pop(v, 2);
		return false;
	}

	// Add a new closure to this table
	sq->pushstring(v, propertyName, -1);
	sq->newclosure(v, function, 0);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 3);
	return true;
}

// -----------------------------------------------------------------------------

// The following function was taken from here:
// https://forum.vc-mp.org/index.php?topic=206.msg3206#msg3206
// It is, however, slightly modified for convenience
void SQLF_RegisterSquirrelFunction(HSQUIRRELVM v, SQFUNCTION function, const SQChar* functionName,
	SQInteger parameterCount, const SQChar* parameterMask)
{
	sq->pushroottable(v);
	sq->pushstring(v, functionName, -1);
	sq->newclosure(v, function, 0);
	sq->setparamscheck(v, parameterCount, parameterMask);
	sq->setnativeclosurename(v, -1, functionName);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
}

bool SQLF_RegisterSquirrelClassMethod(HSQUIRRELVM v, SQFUNCTION function, const SQChar* className,
	const SQChar* methodName, SQInteger parameterCount, const SQChar* parameterMask)
{
	if (!GetSquirrelClass(v, className)) { return false; }

	sq->pushstring(v, methodName, -1);
	sq->newclosure(v, function, 0);
	sq->setparamscheck(v, parameterCount, parameterMask);
	sq->setnativeclosurename(v, -1, methodName);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 2);
	return true;
}

bool SQLF_IsSquirrelObjectInstanceOfClass(HSQUIRRELVM v, SQInteger objectIndex, const SQChar* className)
{
	if (!GetSquirrelClass(v, className)) { return false; }

	bool x;
	sq->push(v, objectIndex);
	x = !!sq->instanceof(v);
	sq->pop(v, 3);
	return x;
}

void SQLF_RegisterSqratClassProperty(HSQUIRRELVM v, SQFUNCTION setterFunction,
	SQFUNCTION getterFunction, const SQChar* className, const SQChar* propertyName)
{
	RegisterSqratClassProperty(v, setterFunction, className, propertyName, _SC("__setTable"));
	RegisterSqratClassProperty(v, getterFunction, className, propertyName, _SC("__getTable"));
}
