#pragma once
#include "SDFat.h"
#include "components/component.h"
class SD : Component
{
  private:
	SdFs sd;
	FsFile file;
	int opened = 0;

  public:
	SD();
	~SD();
	esp_err_t init() override;

	/**
	 * @brief Opens the file in the mode specified as parameter
	 * further operations will work on the opened file
	 *
	 * @param filename
	 * @param mode
	 */
	esp_err_t open_file(char *filename, int mode);
	esp_err_t close_file();
	/**
	 * @brief Writes the given (formatted) char array to the file and adds \n
	 *
	 * @param buff line to be written in the file, s
	 * @return int count of characters written or -1 for failure
	 */
	int write_line(const char *buff);

	/**
	 * @brief Reads characters until \n
	 *
	 * @return String
	 */
	String read_line();

	void select();
	void deselect();
};