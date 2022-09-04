#pragma once
#include "Service.h"

// TODO clear forward delcaration if possible to replace this workaround for circular dependencies
class Service;

class ServiceState
{
public:
	virtual void enter(Service* Service) = 0;
	virtual void toggle(Service* Service) = 0;
	virtual void exit(Service* Service) = 0;
	virtual ~ServiceState() {}
};