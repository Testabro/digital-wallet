#pragma once
#include "Service.h"
#include "Command.hpp"

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