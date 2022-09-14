#pragma once
#include "ServiceState.h"
#include "Service.h"
#include "Command.hpp"

class ServiceListen: public ServiceState
{
public:
	void process(Service* Service) {}
	void toggle(Service* Service);
	static ServiceState& getInstance();

private:
	ServiceListen() {}
	~ServiceListen() {}
	ServiceListen(const ServiceListen& other);
	ServiceListen& operator=(const ServiceListen& other);
};

class ServiceValidate: public ServiceState
{
public:
	void process(Service* Service);
	void toggle(Service* Service) {}
	static ServiceState& getInstance();

private:
	ServiceValidate() {}
	~ServiceValidate() {}
	ServiceValidate(const ServiceValidate& other);
	ServiceValidate& operator=(const ServiceValidate& other);
};

class ServiceApply: public ServiceState
{
public:
	void toggle(Service* Service) {}
	void process(Service* Service);
	static ServiceState& getInstance();

private:
	ServiceApply() {}
	~ServiceApply() {}
	ServiceApply(const ServiceListen& other);
	ServiceApply& operator=(const ServiceApply& other);
};