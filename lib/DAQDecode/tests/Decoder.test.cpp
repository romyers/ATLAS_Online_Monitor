#include <catch2/catch_test_macros.hpp>

#include "Decoder.h"

#include <fstream>

using namespace DAQDecode;

// TODO: We've skipped testing for a little while. Come back to it

TEST_CASE("Decoder", "[Decoder]") {

	std::string file = __FILE__;
	file = file.substr(0, file.find_last_of("/\\") + 1);
	file += "conf/test1.conf";

	SECTION("Empty data produces empty result") {

		Decoder decoder(file);
		
		std::vector<uint64_t> data;

		DecodeResult result = decoder.decode(data);

		REQUIRE(result.events.size() == 0);
		REQUIRE(result.log.size() == 0);
		REQUIRE(result.droppedSignals == 0);
		REQUIRE(result.droppedEvents == 0);
		
	}

	// TODO: Test that signals are buffered between calls when no trailer is
	//       found.

	// TODO: Make sure bad signals are actually dropped by checking events

	SECTION("TDC Data with bad TDC ID is dropped") {

		Decoder decoder(file);

		std::vector<uint64_t> data = {
			0b0000101000000000000000000000000000000000
		};

		DecodeResult result = decoder.decode(data);

		REQUIRE(result.events.size() == 0);
		REQUIRE(result.log.size() == 2);
		REQUIRE(result.droppedSignals == 1);
		REQUIRE(result.droppedEvents == 0);

		REQUIRE(result.log[0].level == LogLevel::ERROR);
		REQUIRE(result.log[0].message == "Unexpected data TDCID = 10, Channel = 0");

		REQUIRE(result.log[1].level == LogLevel::INFO);
		REQUIRE(result.log[1].message == "Dropped signal");

	}

	SECTION("TDC Data with bad channel is dropped") {

		Decoder decoder(file);

		std::vector<uint64_t> data = {
			0b0000000001010000000000000000000000000000
		};

		DecodeResult result = decoder.decode(data);

		REQUIRE(result.events.size() == 0);
		REQUIRE(result.log.size() == 2);
		REQUIRE(result.droppedSignals == 1);
		REQUIRE(result.droppedEvents == 0);

		REQUIRE(result.log[0].level == LogLevel::ERROR);
		REQUIRE(result.log[0].message == "Unexpected data TDCID = 0, Channel = 10");

		REQUIRE(result.log[1].level == LogLevel::INFO);
		REQUIRE(result.log[1].message == "Dropped signal");

	}

	SECTION("TDC Data with good TDC and Channel is kept") {

		Decoder decoder(file);

		std::vector<uint64_t> data = {
			0b0000000100110000000000000000000000000000
		};

		DecodeResult result = decoder.decode(data);
		
		std::cout << result.log[0].message << std::endl;

		REQUIRE(result.events.size() == 0);
		REQUIRE(result.log.size() == 0);
		REQUIRE(result.droppedSignals == 0);
		REQUIRE(result.droppedEvents == 0);

	}
	
}

TEST_CASE("FORCE_FAIL", "[Decoder]") {
	
	REQUIRE(false);

}