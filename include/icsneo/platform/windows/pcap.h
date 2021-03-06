#ifndef __PCAP_WINDOWS_H_
#define __PCAP_WINDOWS_H_

#include "icsneo/platform/windows/internal/pcapdll.h"
#include "icsneo/device/neodevice.h"
#include "icsneo/communication/icommunication.h"
#include "icsneo/api/errormanager.h"
#include <string>

namespace icsneo {

class PCAP : public ICommunication {
public:
	class PCAPFoundDevice {
	public:
		neodevice_t device;
		std::vector<std::vector<uint8_t>> discoveryPackets;
	};

	static std::vector<PCAPFoundDevice> FindAll();
	static std::string GetEthDevSerialFromMacAddress(uint8_t product, uint16_t macSerial);
	static bool IsHandleValid(neodevice_handle_t handle);

	PCAP(const device_errorhandler_t& err, neodevice_t& forDevice);
	bool open();
	bool isOpen();
	bool close();
private:
	PCAPDLL pcap;
	device_errorhandler_t err;
	char errbuf[PCAP_ERRBUF_SIZE] = { 0 };
	neodevice_t& device;
	uint8_t deviceMAC[6];
	bool openable = true;
	void readTask();
	void writeTask();

	class NetworkInterface {
	public:
		uint8_t uuid;
		uint8_t macAddress[8];
		std::string nameFromWinPCAP;
		std::string nameFromWin32API;
		std::string descriptionFromWinPCAP;
		std::string descriptionFromWin32API;
		std::string friendlyNameFromWin32API;
		std::string fullName;
		pcap_t* fp = nullptr;
		pcap_stat stats;
	};
	static std::vector<NetworkInterface> knownInterfaces;
	NetworkInterface interface;

	class EthernetPacket {
	public: // Don't worry about endian when setting fields, this is all taken care of in getBytestream
		EthernetPacket() {};
		EthernetPacket(const std::vector<uint8_t>& bytestream);
		EthernetPacket(const uint8_t* data, size_t size);
		int loadBytestream(const std::vector<uint8_t>& bytestream);
		std::vector<uint8_t> getBytestream() const;
		uint8_t errorWhileDecodingFromBytestream = 0; // Not part of final bytestream, only for checking the result of the constructor
		uint8_t destMAC[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		uint8_t srcMAC[6] = { 0x00, 0xFC, 0x70, 0xFF, 0xFF, 0xFF };
		uint16_t etherType = 0xCAB1; // Big endian, Should be 0xCAB1 or 0xCAB2
		uint32_t icsEthernetHeader = 0xAAAA5555; // Big endian, Should be 0xAAAA5555
		// At this point in the packet, there is a 16-bit payload size, little endian
		// This is calculated from payload size in getBytestream
		uint16_t packetNumber = 0;
		bool firstPiece = true; // These booleans make up a 16-bit bitfield, packetInfo
		bool lastPiece = true;
		bool bufferHalfFull = false;
		std::vector<uint8_t> payload;
	};
};

}

#endif