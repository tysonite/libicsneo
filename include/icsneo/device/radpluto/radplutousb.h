#ifndef __RADPLUTOUSB_H_
#define __RADPLUTOUSB_H_

#include "icsneo/device/radpluto/radpluto.h"
#include "icsneo/platform/stm32.h"

namespace icsneo {

class RADPlutoUSB : public RADPluto {
public:
	// Serial numbers start with RP
	static std::vector<std::shared_ptr<Device>> Find() {
		std::vector<std::shared_ptr<Device>> found;

		for(auto neodevice : STM32::FindByProduct(PRODUCT_ID))
			found.emplace_back(new RADPlutoUSB(neodevice));

		return found;
	}

private:
	RADPlutoUSB(neodevice_t neodevice) : RADPluto(neodevice) {
		initialize<STM32>();
	}
};

}

#endif