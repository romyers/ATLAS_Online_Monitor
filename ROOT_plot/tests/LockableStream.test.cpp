#include <catch2/catch_test_macros.hpp>

#include "DAQMonitor/LockableStream.h"

#include <unistd.h>
#include <fstream>

using namespace std;

// TODO: Test empty lockable stream

TEST_CASE("LockableStream can open an existing file") {

	ofstream out("empty.txt");
	out.close();

	LockableStream ls;

	ls.open("empty.txt");
	REQUIRE(ls.is_open());

	ls.close();

	unlink("empty.txt");

}

TEST_CASE("LockableStream can create a new file") {

	LockableStream ls;

	ls.open("empty.txt");
	REQUIRE(ls.is_open());

	ls.close();

	unlink("empty.txt");

}

TEST_CASE("LockableStream::fileSize()") {

	SECTION("Empty file") {

		LockableStream ls;

		ls.open("empty.txt");

		REQUIRE(ls.fileSize() == 0);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Nonempty file") {

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		LockableStream ls;

		ls.open("empty.txt");

		REQUIRE(ls.fileSize() == 10);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Nonempty file we've read from") {

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		LockableStream ls(3);

		ls.open("empty.txt");

		char buffer[5];
		ls.read(buffer, 5);

		REQUIRE(ls.fileSize() == 10);

		ls.close();

		unlink("empty.txt");

	}

}

TEST_CASE("LockableStream::unreadBytes()") {

	SECTION("Empty file") {

		LockableStream ls;

		ls.open("empty.txt");

		REQUIRE(ls.unreadBytes() == 0);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Nonempty file") {

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		LockableStream ls;

		ls.open("empty.txt");

		REQUIRE(ls.unreadBytes() == 10);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Nonempty file we've read from") {

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		LockableStream ls(2);

		ls.open("empty.txt");

		char buffer[3];
		ls.read(buffer, 3);

		REQUIRE(ls.unreadBytes() == 7);

		ls.close();

		unlink("empty.txt");

	}

}

TEST_CASE("LockableStream cache is managed correctly") {

	SECTION("Cache is populated by write()") {

		LockableStream ls(5);

		ls.open("empty.txt");

		REQUIRE(ls.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10));

		REQUIRE(ls.getCacheUsage() == 5);

		ls.close();

		unlink("empty.txt");

	}
	
	SECTION("Cache is refreshed when exactly exhausted (full cache)") {

		LockableStream ls(5);

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A", 11);
		out.close();

		ls.open("empty.txt");

		char buffer[5];
		ls.read(buffer, 1); // populates the cache
		ls.read(buffer, 5); // exhausts the cache

		REQUIRE(ls.getCacheUsage() == 5);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Cache is refreshed when exactly exhausted (underfull cache)") {

		LockableStream ls(10);

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04", 5);
		out.close();

		ls.open("empty.txt");

		char buffer[5];
		ls.read(buffer, 1);

		ofstream out2("empty.txt", ios::binary | ios::app);
		out2.write("\x05\x06\x07\x08\x09", 5);
		out2.close();

		ls.read(buffer, 4);

		REQUIRE(ls.getCacheUsage() == 5);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Cache is not refreshed if not exhausted (full cache)") {

		LockableStream ls(5);

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		ls.open("empty.txt");

		char buffer[4];
		ls.read(buffer, 1); // populates the cache
		ls.read(buffer, 4); // does not exhaust the cache (barely)

		REQUIRE(ls.getCacheUsage() == 1);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Cache is not refreshed if not exhausted (underfull cache)") {

		LockableStream ls(15);

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		ls.open("empty.txt");

		char buffer[8];
		ls.read(buffer, 1);

		ofstream out2("empty.txt", ios::binary | ios::app);
		out2.write("\x0A\x0B\x0C\x0D\x0E\x0F\x10", 7);
		out2.close();

		ls.read(buffer, 8);

		REQUIRE(ls.getCacheUsage() == 1);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Cache is refreshed starting at next value when more than exhausted (full cache)") {

		LockableStream ls(5);

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		ls.open("empty.txt");

		char buffer[6];
		ls.read(buffer, 1);
		ls.read(buffer, 6);

		REQUIRE(ls.getCacheUsage() == 3);

		ls.close();

		unlink("empty.txt");

	}

	SECTION("Cache is refreshed starting at next value when more than exhausted (underfull cache)") {

		LockableStream ls(10);

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04", 5);
		out.close();

		ls.open("empty.txt");

		char buffer[5];
		ls.read(buffer, 1);

		ofstream out2("empty.txt", ios::binary | ios::app);
		out2.write("\x05\x06\x07\x08\x09", 5);
		out2.close();

		ls.read(buffer, 6);

		REQUIRE(ls.getCacheUsage() == 3);

		ls.close();

		unlink("empty.txt");

	}
	
}

TEST_CASE("LockableStream::read()") {

	SECTION("File reads start in the right place") {

		SECTION("Read starts in the right place after cache populated from file") {

			LockableStream ls(5);

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			ls.open("empty.txt");

			char buffer[5];
			ls.read(buffer, 1); // populates the cache
			ls.read(buffer, 4); // mostly exhausts the cache

			ls.read(buffer, 5);

			REQUIRE(buffer[0] == 5);
			REQUIRE(buffer[1] == 6);
			REQUIRE(buffer[2] == 7);
			REQUIRE(buffer[3] == 8);
			REQUIRE(buffer[4] == 9);

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Read starts in the right place after cache populated with write()") {

			LockableStream ls(5);

			ls.open("empty.txt");

			REQUIRE(ls.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10));

			// Cache should already be populated

			char buffer[5];
			ls.read(buffer, 4); // mostly exhaust cache
			ls.read(buffer, 5); // Overflow cache and make sure some is being read from file.

			REQUIRE(buffer[0] == 4);
			REQUIRE(buffer[1] == 5);
			REQUIRE(buffer[2] == 6);
			REQUIRE(buffer[3] == 7);
			REQUIRE(buffer[4] == 8);

			ls.close();

			unlink("empty.txt");

		}

	}

	SECTION("Nothing is read from an empty stream") {

		SECTION("Zero Capacity Cache") {

			ofstream out("empty.txt");
			out.close();

			LockableStream ls;

			ls.open("empty.txt");

			char buffer[10];
			REQUIRE(ls.read(buffer, 10) == 0);

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Nonzero Capacity Cache") {

			ofstream out("empty.txt");
			out.close();

			LockableStream ls(10);

			ls.open("empty.txt");

			char buffer[10];
			REQUIRE(ls.read(buffer, 10) == 0);

			ls.close();

			unlink("empty.txt");

		}

	}

	SECTION("Reading from a nonempty stream") {

		SECTION("Zero Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls;

			ls.open("empty.txt");

			char buffer[10];
			REQUIRE(ls.read(buffer, 10) == 10);

			REQUIRE(buffer[0] == 0);
			REQUIRE(buffer[1] == 1);
			REQUIRE(buffer[2] == 2);
			REQUIRE(buffer[3] == 3);
			REQUIRE(buffer[4] == 4);
			REQUIRE(buffer[5] == 5);
			REQUIRE(buffer[6] == 6);
			REQUIRE(buffer[7] == 7);
			REQUIRE(buffer[8] == 8);
			REQUIRE(buffer[9] == 9);

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Small Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls(5);

			ls.open("empty.txt");

			char buffer[10];
			REQUIRE(ls.read(buffer, 10) == 10);

			REQUIRE(buffer[0] == 0);
			REQUIRE(buffer[1] == 1);
			REQUIRE(buffer[2] == 2);
			REQUIRE(buffer[3] == 3);
			REQUIRE(buffer[4] == 4);
			REQUIRE(buffer[5] == 5);
			REQUIRE(buffer[6] == 6);
			REQUIRE(buffer[7] == 7);
			REQUIRE(buffer[8] == 8);
			REQUIRE(buffer[9] == 9);

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Big Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls(20);

			ls.open("empty.txt");

			char buffer[10];
			REQUIRE(ls.read(buffer, 10) == 10);

			REQUIRE(buffer[0] == 0);
			REQUIRE(buffer[1] == 1);
			REQUIRE(buffer[2] == 2);
			REQUIRE(buffer[3] == 3);
			REQUIRE(buffer[4] == 4);
			REQUIRE(buffer[5] == 5);
			REQUIRE(buffer[6] == 6);
			REQUIRE(buffer[7] == 7);
			REQUIRE(buffer[8] == 8);
			REQUIRE(buffer[9] == 9);

			ls.close();

			unlink("empty.txt");

		}

	}

}

TEST_CASE("LockableStream::write()") {

	SECTION("Writing to an empty stream") {

		SECTION("Zero Capacity Cache") {

			LockableStream ls;

			ls.open("empty.txt");

			REQUIRE(ls.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10));

			ls.close();

			ifstream in("empty.txt", ios::binary);
			char buffer[10];
			in.read(buffer, 10);

			REQUIRE(buffer[0] == 0);
			REQUIRE(buffer[1] == 1);
			REQUIRE(buffer[2] == 2);
			REQUIRE(buffer[3] == 3);
			REQUIRE(buffer[4] == 4);
			REQUIRE(buffer[5] == 5);
			REQUIRE(buffer[6] == 6);
			REQUIRE(buffer[7] == 7);
			REQUIRE(buffer[8] == 8);
			REQUIRE(buffer[9] == 9);

			in.close();

			unlink("empty.txt");

		}

		SECTION("Nonzero Capacity Cache") {

			LockableStream ls(10);

			ls.open("empty.txt");

			REQUIRE(ls.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10));

			ls.close();

			ifstream in("empty.txt", ios::binary);
			char buffer[10];
			in.read(buffer, 10);

			REQUIRE(buffer[0] == 0);
			REQUIRE(buffer[1] == 1);
			REQUIRE(buffer[2] == 2);
			REQUIRE(buffer[3] == 3);
			REQUIRE(buffer[4] == 4);
			REQUIRE(buffer[5] == 5);
			REQUIRE(buffer[6] == 6);
			REQUIRE(buffer[7] == 7);
			REQUIRE(buffer[8] == 8);
			REQUIRE(buffer[9] == 9);

			in.close();

			unlink("empty.txt");

		}

	}

	SECTION("Writing to a nonempty stream") {

		SECTION("Zero Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls;

			ls.open("empty.txt");

			REQUIRE(ls.write("\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13", 10));

			ls.close();

			ifstream in("empty.txt", ios::binary);
			char buffer[20];
			in.read(buffer, 20);

			REQUIRE(buffer[0] == 0);
			REQUIRE(buffer[1] == 1);
			REQUIRE(buffer[2] == 2);
			REQUIRE(buffer[3] == 3);
			REQUIRE(buffer[4] == 4);
			REQUIRE(buffer[5] == 5);
			REQUIRE(buffer[6] == 6);
			REQUIRE(buffer[7] == 7);
			REQUIRE(buffer[8] == 8);
			REQUIRE(buffer[9] == 9);
			REQUIRE(buffer[10] == 10);
			REQUIRE(buffer[11] == 11);
			REQUIRE(buffer[12] == 12);
			REQUIRE(buffer[13] == 13);
			REQUIRE(buffer[14] == 14);
			REQUIRE(buffer[15] == 15);
			REQUIRE(buffer[16] == 16);
			REQUIRE(buffer[17] == 17);
			REQUIRE(buffer[18] == 18);
			REQUIRE(buffer[19] == 19);

			in.close();

			unlink("empty.txt");

		}

		SECTION("Nonzero Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls(10);

			ls.open("empty.txt");

			REQUIRE(ls.write("\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13", 10));

			ls.close();

			ifstream in("empty.txt", ios::binary);
			char buffer[20];
			in.read(buffer, 20);

			REQUIRE(buffer[0] == 0);
			REQUIRE(buffer[1] == 1);
			REQUIRE(buffer[2] == 2);
			REQUIRE(buffer[3] == 3);
			REQUIRE(buffer[4] == 4);
			REQUIRE(buffer[5] == 5);
			REQUIRE(buffer[6] == 6);
			REQUIRE(buffer[7] == 7);
			REQUIRE(buffer[8] == 8);
			REQUIRE(buffer[9] == 9);
			REQUIRE(buffer[10] == 10);
			REQUIRE(buffer[11] == 11);
			REQUIRE(buffer[12] == 12);
			REQUIRE(buffer[13] == 13);
			REQUIRE(buffer[14] == 14);
			REQUIRE(buffer[15] == 15);
			REQUIRE(buffer[16] == 16);
			REQUIRE(buffer[17] == 17);
			REQUIRE(buffer[18] == 18);
			REQUIRE(buffer[19] == 19);

			in.close();

			unlink("empty.txt");

		}

	}


}

TEST_CASE("LockableStream mixed reads and writes") {

	LockableStream ls(0);

	ls.open("empty.txt");

	REQUIRE(ls.write("\x00\x01\x02\x03", 4));

	char buffer[5];
	REQUIRE(ls.read(buffer, 5) == 4);

	REQUIRE(buffer[0] == 0);
	REQUIRE(buffer[1] == 1);
	REQUIRE(buffer[2] == 2);
	REQUIRE(buffer[3] == 3);

	REQUIRE(ls.write("\x04\x05\x06\x07", 4));

	REQUIRE(ls.read(buffer, 1) == 1);

	REQUIRE(buffer[0] == 4);

	REQUIRE(ls.read(buffer, 2) == 2);

	REQUIRE(buffer[0] == 5);
	REQUIRE(buffer[1] == 6);

	REQUIRE(ls.write("\x08", 1));

	REQUIRE(ls.read(buffer, 3) == 2);

	REQUIRE(buffer[0] == 7);
	REQUIRE(buffer[1] == 8);

	unlink("empty.txt");

}

TEST_CASE("Cache is used appropriately") {

	SECTION("Cache is populated on write") {

		LockableStream ls(5);

		ls.open("empty.txt");

		REQUIRE(ls.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10));

		REQUIRE(ls.getCacheUsage() == 5);

		unlink("empty.txt");

	}

	SECTION("Cache is populated on read") {

		LockableStream ls(5);

		ofstream out("empty.txt", ios::binary);
		out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
		out.close();

		ls.open("empty.txt");

		char buffer[1];
		REQUIRE(ls.read(buffer, 1) == 1);

		REQUIRE(ls.getCacheUsage() == 5);

		unlink("empty.txt");

	}

}

TEST_CASE("LockableStream::get()") {

	SECTION("Getting from an empty stream") {

		SECTION("Zero Capacity Cache") {

			LockableStream ls;

			ls.open("empty.txt");

			vector<unsigned char> buffer = ls.get(10);

			REQUIRE(buffer.size() == 0);

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Nonzero Capacity Cache") {

			LockableStream ls(10);

			ls.open("empty.txt");

			vector<unsigned char> buffer = ls.get(10);

			REQUIRE(buffer.size() == 0);

			ls.close();

			unlink("empty.txt");

		}

	}

	SECTION("Getting from a nonempty stream") {

		SECTION("Zero Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls;

			ls.open("empty.txt");

			vector<unsigned char> buffer = ls.get(10);

			REQUIRE(buffer == vector<unsigned char>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Small Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls(5);

			ls.open("empty.txt");

			vector<unsigned char> buffer = ls.get(10);

			REQUIRE(buffer == vector<unsigned char>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

			ls.close();

			unlink("empty.txt");

		}

	}

	SECTION("Requesting more bytes than exist") {

		SECTION("Zero Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls;

			ls.open("empty.txt");

			vector<unsigned char> buffer = ls.get(20);

			REQUIRE(buffer == vector<unsigned char>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Small Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls(5);

			ls.open("empty.txt");

			vector<unsigned char> buffer = ls.get(20);

			REQUIRE(buffer == vector<unsigned char>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

			ls.close();

			unlink("empty.txt");

		}

		SECTION("Large Capacity Cache") {

			ofstream out("empty.txt", ios::binary);
			out.write("\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09", 10);
			out.close();

			LockableStream ls(20);

			ls.open("empty.txt");

			vector<unsigned char> buffer = ls.get(20);

			REQUIRE(buffer == vector<unsigned char>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));

			ls.close();

			unlink("empty.txt");

		}

	}

}
//