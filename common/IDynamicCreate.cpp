// ==============================================================================
// Program: IDynamicCreate.cpp
// Programmer: Ethan-da-Tech-Wizard
// Last Modified: November 18, 2026
// Purpose: Implementation of dynamic object factory dispatch and class lookup maps.
//          Inputs: String identifiers for registered runtime types.
//          Outputs: Instantiated object pointers or null on unknown identifier.
//          Algorithm: Hash map lookup against registered instantiation function pointers.
// ==============================================================================

#include "IDynamicCreate.h"

#if ENABLE_IDYNAMICCREATE

IClassRegistry	_gClassRegistry;

IClassRegistry::IClassRegistry()
{
	//
}

IClassRegistry::~IClassRegistry()
{
	//
}

void IClassRegistry::RegisterClassInfo(UInt32 id, IDynamicType * typeInfo)
{
	theClassRegistry[id] = typeInfo;
}

IDynamicType * IClassRegistry::LookupClassInfo(UInt32 id)
{
	ClassRegistryType::iterator iter = theClassRegistry.find(id);

	return (iter == theClassRegistry.end()) ? NULL : (*iter).second;
}

IDynamicType * IClassRegistry::LookupClassInfo(char * name)
{
	for(ClassRegistryType::iterator iter = theClassRegistry.begin(); iter != theClassRegistry.end(); iter++)
		if(!strcmp((*iter).second->GetName(), name))
			return (*iter).second;

	return NULL;
}

#endif
