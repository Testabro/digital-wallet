#pragma once
#include "ServiceState.h"
#include "Service.h"
#include "Command.hpp"

class ServiceListen: public ServiceState
{
public:
	void process(Service* service) {}
	void toggle(Service* service);
	void onEnter(Service* service);
	std::string getStateName();
	static ServiceState& getInstance();

private:
	ServiceListen() {}
	~ServiceListen() {}
	ServiceListen(const ServiceListen&) = delete;
	ServiceListen& operator=(const ServiceListen&) = delete;
};

class ServiceValidate: public ServiceState
{
public:
	void process(Service* service);
	void toggle(Service* service) {}
	void onEnter(Service* service) {}
	std::string getStateName();
	static ServiceState& getInstance();

private:
	ServiceValidate() {}
	~ServiceValidate() {}
	ServiceValidate(const ServiceValidate&) = delete;
	ServiceValidate& operator=(const ServiceValidate&) = delete;
};

class ServiceApply: public ServiceState
{
public:
	void toggle(Service* service) {}
	void process(Service* service);
	void onEnter(Service* service) {}
	std::string getStateName();
	static ServiceState& getInstance();

private:
	ServiceApply() {}
	~ServiceApply() {}
	ServiceApply(const ServiceApply&) = delete;
	ServiceApply& operator=(const ServiceApply&) = delete;
};
