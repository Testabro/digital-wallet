#pragma once
#include "ServiceState.h"
#include "Service.h"

class ServiceListen: public ServiceState
{
public:
	void enter(Service* Service) {}
	void toggle(Service* Service);
	void exit(Service* Service) {}
	static ServiceState& getInstance();

private:
	ServiceListen() {}
	ServiceListen(const ServiceListen& other);
	ServiceListen& operator=(const ServiceListen& other);
};

class ServiceValidate: public ServiceState
{
public:
	void enter(Service* Service) {}
	void toggle(Service* Service);
	void exit(Service* Service) {}
	static ServiceState& getInstance();

private:
	ServiceValidate() {}
	ServiceValidate(const ServiceValidate& other);
	ServiceValidate& operator=(const ServiceValidate& other);
};

class ServiceApply: public ServiceState
{
public:
	void enter(Service* Service) {}
	void toggle(Service* Service);
	void exit(Service* Service) {}
	static ServiceState& getInstance();

private:
	ServiceApply() {}
	ServiceApply(const ServiceListen& other);
	ServiceApply& operator=(const ServiceApply& other);
};