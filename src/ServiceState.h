#pragma once
#include "Service.h"
#include "Command.hpp"

// TODO clear forward delcaration if possible to replace this workaround for circular dependencies
class Service;

class ServiceState
{
public:
	virtual void process(Service* Service) = 0;
	virtual void toggle(Service* Service) = 0;
	virtual void onEnter(Service* Service) = 0;
	virtual std::string getStateName() = 0;
	virtual ~ServiceState() {}
};