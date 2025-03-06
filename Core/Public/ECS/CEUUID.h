//
// Created by Chillstep on 25-3-7.
//

#ifndef CENGINE_CORE_PUBLIC_ECS_CEUUID_H_
#define CENGINE_CORE_PUBLIC_ECS_CEUUID_H_

#include "CEngine.h"

namespace CE{
	class CEUUID{
	 public:
		CEUUID();
		CEUUID(uint32_t uuid);
		CEUUID(const CEUUID&) = default;
		operator uint32_t() const {return mUUID;}

		uint64_t mUUID;
	};
}

//template Specialization
template<>
struct std::hash<CE::CEUUID>{
	std::size_t operator()(const CE::CEUUID& uuid) const{
		if(!uuid) return 0;
		return (uint32_t)uuid;
	}
};

#endif //CENGINE_CORE_PUBLIC_ECS_CEUUID_H_
