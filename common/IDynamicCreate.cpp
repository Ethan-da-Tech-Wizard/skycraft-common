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

/*
 * Registry of dynamic class descriptors. Entire contents are compiled out unless
 * ENABLE_IDYNAMICCREATE is 1 (see IDynamicCreate.h); with the default 0 this file produces no code.
 */

#if ENABLE_IDYNAMICCREATE

/// The one global registry instance (note: the class's own storage `theClassRegistry` is a static member).
IClassRegistry	_gClassRegistry;

/// Nothing to do: all state is the static map.
IClassRegistry::IClassRegistry()
{
	//
}

/// Nothing to free: descriptors are static objects owned by their classes.
IClassRegistry::~IClassRegistry()
{
	//
}

/**
 * Associate a numeric id with a class descriptor (replaces any previous entry for that id).
 *
 * @param id       Application-chosen class id.
 * @param typeInfo Descriptor (not owned).
 */
void IClassRegistry::RegisterClassInfo(UInt32 id, IDynamicType * typeInfo)
{
	theClassRegistry[id] = typeInfo;
}

/**
 * @param id Class id.
 * @return Descriptor registered under that id, or NULL.
 */
IDynamicType * IClassRegistry::LookupClassInfo(UInt32 id)
{
	ClassRegistryType::iterator iter = theClassRegistry.find(id);

	return (iter == theClassRegistry.end()) ? NULL : (*iter).second;
}

/**
 * @param name Class name, compared case-sensitively.
 * @return First registered descriptor with that name, or NULL. Linear search.
 */
IDynamicType * IClassRegistry::LookupClassInfo(char * name)
{
	for(ClassRegistryType::iterator iter = theClassRegistry.begin(); iter != theClassRegistry.end(); iter++)
		if(!strcmp((*iter).second->GetName(), name))
			return (*iter).second;

	return NULL;
}

#endif
