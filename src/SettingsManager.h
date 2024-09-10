/**
 * @file SettingsManager.h
 * 
 * @brief Provides a persistent string-to-string map for managing
 * settings that should persist between runs of the DAQMonitor.
 * 
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <string>
#include <map>

namespace DAQ {

	/**
	 * @brief Manages persistent settings for the DAQMonitor.
	 * 
	 * Many settings, like the config file, the network device, and the
	 * window size, should be saved between runs of the DAQMonitor for user
	 * convenience. This class provides a way to manage those settings with
	 * a std::map-like interface.
	 * 
	 * This class acts as a persistent std::map<std::string, std::string>, 
	 * minus unneeded functionality. Changes can be committed to storage
	 * with the save() method.
	 * 
	 * Note that unsaved changes will not persist, and SettingsManager
	 * instances will not share changes made after construction unless the
	 * changes are saved and refreshed.
	 */
	class SettingsManager {

	public:

		/**
		 * @brief Construct a new SettingsManager object and associates it with
		 * a file. If the file could not be read, the 
		 * SettingsManager will be empty, and settings will be set according
		 * to client defaults.
		 * 
		 * @param path The path to the settings file.
		 */
		SettingsManager(const std::string &path);

		/**
		 * @brief Returns true if a setting with the given key exists,
		 * and false otherwise.
		 * 
		 * @param key The key to check for. Must not contain newline
		 * characters.
		 * 
		 * @return True if the setting exists, false otherwise.
		 */
		bool has(const std::string &key);

		/**
		 * @brief Accesses a specified element with bounds checking.
		 * 
		 * Note that if the settings file could not be read, keys
		 * that have been previously set may not still exist. It's best to
		 * make sure it exists by checking with has() or setting a default
		 * with setDefault() before calling this method to avoid an exception.
		 * 
		 * @param key The key of the element to access. Must not
		 * contain newline characters.
		 * 
		 * @return A reference to the value associated with the key.
		 * 
		 * @throws std::out_of_range If the key does not exist.
		 */
		std::string &at(const std::string &key);

		/**
		 * @brief Accesses or inserts the specified element.
		 * 
		 * If the key does not exist, it is inserted with a value of "".
		 * 
		 * @param key The key of the element to access. Must not contain
		 * newline characters.
		 * 
		 * @return A reference to the value associated with the key, or ""
		 * if the key did not exist.
		 */
		std::string &operator[](const std::string &key);

		/**
		 * @brief Sets the default value of a value associated with a key.
		 * 
		 * If the key does not exist, it is inserted with the given value.
		 * If it does exist, the existing value is unaltered.
		 * 
		 * @param key The key of the element to set the default value of.
		 * Must not contain newline characters.
		 * @param value The default value to set.
		 * 
		 * @return True if the default value was set (i.e. the key did not
		 * exist), and false otherwise.
		 */
		bool setDefault(const std::string &key, const std::string &value);

		/**
		 * @brief Saves the current settings to the settings file, and returns
		 * true if the operation was successful.
		 * 
		 * If the settings file could not be opened, the settings will not
		 * be saved and this function will return false.
		 * 
		 * @return True if the settings were saved, and false otherwise.
		 */
		bool save();

		/**
		 * @brief Refreshes the settings from the settings file. The existing
		 * settings will be discarded and replaced with the settings from the
		 * file, provided that the file could be opened.
		 * 
		 * If the settings file could not be opened, the existing settings will
		 * be preserved and the function will return false.
		 * 
		 * @return True if the new settings file could be opened, and
		 * false otherwise.
		 */
		bool refresh();

	private:

		// The path to the settings file
		std::string path;

		// The settings data
		std::map<std::string, std::string> data;

	};

}