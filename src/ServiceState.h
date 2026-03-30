#pragma once
#include "Service.h"
#include "Command.hpp"

class Service;

class ServiceState
{
public:
	virtual void process(Service* service) = 0;
	virtual void toggle(Service* service) = 0;
	virtual void onEnter(Service* service) = 0;
	virtual std::string getStateName() = 0;
	virtual ~ServiceState() {}
};
