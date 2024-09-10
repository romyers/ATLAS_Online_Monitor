#include <catch2/catch_test_macros.hpp>

#include "SettingsManager.h"

#include <fstream>

using namespace DAQ;

const std::string SETTINGS_FILE = "test_settings.txt";

TEST_CASE("SettingsManager::at() does bounds checking") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SECTION("at() throws when key does not exist") {

		SettingsManager sm(SETTINGS_FILE);

		REQUIRE_THROWS(sm.at("nonexistent_key"));

	}

	SECTION("at() does not throw when key exists") {

		SettingsManager sm(SETTINGS_FILE);

		sm["key"] = "value";

		REQUIRE_NOTHROW(sm.at("key"));

	}

}

TEST_CASE("SettingsManager::at() returns the value") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	sm["key"] = "value";

	REQUIRE(sm.at("key") == "value");

}

TEST_CASE("SettingsManager::at() provides write access") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	sm["key"] = "value";

	sm.at("key") = "new_value";

	REQUIRE(sm.at("key") == "new_value");

}

TEST_CASE("SettingsManager::operator[] inserts a key if it does not exist") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	REQUIRE_FALSE(sm.has("key"));

	REQUIRE(sm["key"] == "");

	REQUIRE(sm.has("key"));

}

TEST_CASE("SettingsManager::operator[] returns the value if it exists") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	sm["key"] = "value";

	REQUIRE(sm["key"] == "value");

}

TEST_CASE("SettingsManager::operator[] provides write access") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	sm["key"] = "value";
	sm["key"] = "new_value";

	REQUIRE(sm["key"] == "new_value");

}

TEST_CASE("Saved changes persist across SettingsManager instances") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm1(SETTINGS_FILE);

	sm1["key"] = "value";
	sm1.save();

	SettingsManager sm2(SETTINGS_FILE);

	REQUIRE(sm2["key"] == "value");

}

TEST_CASE("SettingsManager::SetDefault() behaves correctly") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SECTION("SetDefault() returns true when key does not exist") {

		SettingsManager sm(SETTINGS_FILE);

		REQUIRE(sm.setDefault("key", "value"));

	}

	SECTION("SetDefault() returns false when key exists") {

		SettingsManager sm(SETTINGS_FILE);

		sm["key"] = "value";

		REQUIRE_FALSE(sm.setDefault("key", "value"));

	}

	SECTION("SetDefault() sets the key if it does not exist") {

		SettingsManager sm(SETTINGS_FILE);

		sm.setDefault("key", "value");

		REQUIRE(sm.has("key"));
		REQUIRE(sm["key"] == "value");

	}

	SECTION("SetDefault() does not set the key if it exists") {

		SettingsManager sm(SETTINGS_FILE);

		sm["key"] = "value";

		sm.setDefault("key", "new_value");

		REQUIRE(sm["key"] == "value");

	}


}

TEST_CASE("Saved changes do not propogate to other existing SettingsManager instances") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm1(SETTINGS_FILE);
	SettingsManager sm2(SETTINGS_FILE);

	sm1["key"] = "value";
	sm1.save();

	REQUIRE_FALSE(sm2.has("key"));

}

TEST_CASE("SettingsManagers reload changes on refresh") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm1(SETTINGS_FILE);
	SettingsManager sm2(SETTINGS_FILE);

	sm1["key"] = "value";
	sm1.save();

	REQUIRE(sm2.refresh());

	REQUIRE(sm2["key"] == "value");

}

TEST_CASE("SettingsManager overwrites existing settings on good refresh") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	sm["key"] = "value";
	
	REQUIRE(sm.refresh());

	REQUIRE_FALSE(sm.has("key"));

}

TEST_CASE("SettingsManager does not overwrite existing settings on bad refresh") {

	SettingsManager sm("nonexistent");

	sm["key"] = "value";

	REQUIRE_FALSE(sm.refresh());

	REQUIRE(sm["key"] == "value");

}

TEST_CASE("SettingsManagers with different files do not share data") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();
	std::ofstream("test_settings2.txt", std::ofstream::trunc).close();

	SettingsManager sm1(SETTINGS_FILE);
	SettingsManager sm2("test_settings2.txt");

	sm1["key"] = "value";
	sm1.save();

	REQUIRE(sm2.refresh());

	REQUIRE_FALSE(sm2.has("key"));

}

TEST_CASE("SettingsManager can handle multiple values") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	sm["key1"] = "value1";
	sm["key2"] = "value2";

	REQUIRE(sm["key1"] == "value1");
	REQUIRE(sm["key2"] == "value2");

	sm.save();

	SettingsManager sm2(SETTINGS_FILE);

	REQUIRE(sm2["key1"] == "value1");
	REQUIRE(sm2["key2"] == "value2");

}

TEST_CASE("SettingsManager is stable against malformed settings") {

	std::ofstream(SETTINGS_FILE, std::ofstream::trunc).close();

	SettingsManager sm(SETTINGS_FILE);

	sm["key"] = "value";
	sm.save();

	std::ofstream out(SETTINGS_FILE, std::ofstream::app);
	out << "badline" << std::endl;
	out << "badline2" << std::endl;
	out.close();

	SettingsManager sm2(SETTINGS_FILE);

	REQUIRE(sm2["key"] == "value");
	REQUIRE_FALSE(sm2.has("badline"));
	REQUIRE_FALSE(sm2.has("badline2"));

	REQUIRE(sm2.save());

	std::ifstream in(SETTINGS_FILE);

	std::string line;

	REQUIRE(std::getline(in, line));
	REQUIRE_FALSE(std::getline(in, line));

}