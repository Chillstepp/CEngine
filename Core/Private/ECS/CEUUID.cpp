//
// Created by Chillstep on 25-3-7.
//
#include "ECS/CEUUID.h"
#include "random"


namespace CE{

	static ::std::random_device sRandomDevice;
	static ::std::mt19937_64 sEngine(sRandomDevice());
	static ::std::uniform_int_distribution<uint32_t> sUniformDistribution(1, UINT32_MAX);

	CEUUID::CEUUID() : mUUID(sUniformDistribution(sEngine))
	{

	}

	CEUUID::CEUUID(uint32_t uuid) : mUUID(uuid)
	{

	}
}