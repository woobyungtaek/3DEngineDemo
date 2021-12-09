#include "pch.h"
#include "BTMaterial.h"

BTMaterial::BTMaterial()
{
	mProperties = new MaterialProperties();
}

BTMaterial::~BTMaterial()
{
	delete mProperties;
}
