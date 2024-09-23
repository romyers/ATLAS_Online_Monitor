#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "DAQMonitor/RingBuffer.h"

TEST_CASE("RingBuffer Constructor") {

	SECTION("Default constructor gives correct capacity") {

		RingBuffer<int> buffer;

		REQUIRE(buffer.capacity() == 1000000);

	}

	SECTION("Capacity can be set in the constructor") {

		RingBuffer<int> buffer(100);

		REQUIRE(buffer.capacity() == 100);

	}

	SECTION("RingBuffers are empty on construction") {

		RingBuffer<int> buffer;

		REQUIRE(buffer.empty());

		RingBuffer<int> buffer2(100);

		REQUIRE(buffer2.empty());

	}

	SECTION("RingBuffers have size 0 on construction") {

		RingBuffer<int> buffer;

		REQUIRE(buffer.size() == 0);

		RingBuffer<int> buffer2(100);

		REQUIRE(buffer2.size() == 0);

	}

	SECTION("RingBuffers can handle 0 capacity") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.capacity() == 0);

	}

}

TEST_CASE("RingBuffer::front()") {

	SECTION("front() throws an exception on an empty buffer") {

		RingBuffer<int> buffer;

		REQUIRE_THROWS(buffer.front());

		RingBuffer<int> buffer2(100);

		REQUIRE_THROWS(buffer2.front());

	}

	SECTION("front() returns the first element in the buffer") {

		RingBuffer<int> buffer(10);

		buffer.push(1);

		REQUIRE(buffer.front() == 1);

		buffer.push(2);

		REQUIRE(buffer.front() == 1);

	}

	SECTION("front() allows modification of the first element") {

		RingBuffer<int> buffer(10);

		buffer.push(1);

		buffer.front() = 2;

		REQUIRE(buffer.front() == 2);

	}

	SECTION("front() throws an exception for a zero capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE_THROWS(buffer.front());

	}

}

TEST_CASE("RingBuffer::push()") {

	SECTION("push() does nothing to a zero capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE_FALSE(buffer.push(1));

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());
		REQUIRE(buffer.capacity() == 0);

	}

	SECTION("push() adds an element to the buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.size() == 0);

		REQUIRE(buffer.push(1));

		REQUIRE(buffer.size() == 1);

		REQUIRE(buffer.push(2));

		REQUIRE(buffer.size() == 2);

	}

	SECTION("push() has FIFO semantics") {

		RingBuffer<int> buffer(2);

		REQUIRE(buffer.push(1));
		REQUIRE(buffer.push(2));

		REQUIRE(buffer.front() == 1);

		buffer.pop();

		REQUIRE(buffer.push(3));

		REQUIRE(buffer.front() == 2);

	}

	SECTION("push() does nothing to a full buffer") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		REQUIRE_FALSE(buffer.push(3));

		REQUIRE(buffer.size() == 2);
		REQUIRE(buffer.get(2) == std::vector<int>{1, 2});
		REQUIRE(buffer.full());
		REQUIRE(buffer.capacity() == 2);

	}

	SECTION("Push wraps around correctly") {

		RingBuffer<int> buffer(2);

		REQUIRE(buffer.push(1));
		REQUIRE(buffer.push(2));

		buffer.pop();
		REQUIRE(buffer.push(3));

		REQUIRE(buffer.front() == 2);
		buffer.pop();
		REQUIRE(buffer.front() == 3);

	}

	SECTION("Push works after clear()") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.clear();

		REQUIRE(buffer.push(3));

		REQUIRE(buffer.front() == 3);

	}

}

TEST_CASE("RingBuffer::pop()") {

	SECTION("pop() does nothing on a zero capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.pop() == false);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());
		REQUIRE(buffer.capacity() == 0);

	}

	SECTION("pop() removes an element from the buffer") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		REQUIRE(buffer.size() == 2);

		REQUIRE(buffer.pop());

		REQUIRE(buffer.size() == 1);

		REQUIRE(buffer.pop());

		REQUIRE(buffer.size() == 0);

	}

	SECTION("pop() has FIFO semantics") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		REQUIRE(buffer.pop());

		REQUIRE(buffer.front() == 2);

	}

	SECTION("pop() does nothing on an empty buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.pop() == false);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());
		REQUIRE(buffer.capacity() == 10);

	}

	SECTION("pop() wraps around correctly") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		REQUIRE(buffer.pop());
		buffer.push(3);

		REQUIRE(buffer.front() == 2);

		REQUIRE(buffer.pop());
		buffer.push(4);

		REQUIRE(buffer.front() == 3);

		REQUIRE(buffer.pop());
		buffer.push(5);

		REQUIRE(buffer.front() == 4);

		REQUIRE(buffer.pop());
		REQUIRE(buffer.front() == 5);

	}

	SECTION("Pop works after clear") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.clear();

		buffer.push(3);
		buffer.push(4);

		REQUIRE(buffer.pop());

		REQUIRE(buffer.front() == 4);

	}

}

TEST_CASE("RingBuffer::size()") {

	SECTION("size() is 0 for 0 capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.size() == 0);

	}

	SECTION("size() is correct for an empty buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.size() == 0);

	}

	SECTION("size() returns the number of elements in the buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.size() == 0);

		buffer.push(1);

		REQUIRE(buffer.size() == 1);

		buffer.push(2);

		REQUIRE(buffer.size() == 2);

		buffer.pop();

		REQUIRE(buffer.size() == 1);

	}

	SECTION("size() wraps around correctly") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.pop();

		REQUIRE(buffer.size() == 1);

		buffer.push(3);

		REQUIRE(buffer.size() == 2);

		buffer.pop();
		buffer.pop();

		REQUIRE(buffer.size() == 0);

	}

	SECTION("Size is correct for a full buffer") {

		SECTION("No pops") {

			RingBuffer<int> buffer(2);

			buffer.push(1);
			buffer.push(2);

			REQUIRE(buffer.size() == 2);

		}

		SECTION("With pops") {

			RingBuffer<int> buffer(2);

			buffer.push(1);
			buffer.push(2);

			buffer.pop();
			buffer.push(3);

			REQUIRE(buffer.size() == 2);

		}

	}

	SECTION("Size is correct for a buffer emptied with clear()") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.clear();

		REQUIRE(buffer.size() == 0);

	}

	SECTION("Size is correct with head and tail at end of buffer") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.pop();
		buffer.pop();

		REQUIRE(buffer.size() == 0);

		buffer.push(3);

		REQUIRE(buffer.size() == 1);

	}

}

TEST_CASE("RingBuffer::empty()") {

	SECTION("empty() is true for 0 capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.empty());

	}
	
	SECTION("empty() returns true for an empty buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.empty());

	}

	SECTION("empty() returns false for a non-empty buffer") {

		RingBuffer<int> buffer(10);

		buffer.push(1);

		REQUIRE_FALSE(buffer.empty());

	}

	SECTION("empty() returns false for buffer filled with push") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		REQUIRE_FALSE(buffer.empty());

	}

	SECTION("empty() returns false for buffer filled with insert()") {

		RingBuffer<int> buffer(2);

		buffer.insert(std::vector<int>{1, 2});

		REQUIRE_FALSE(buffer.empty());

	}

	SECTION("empty() returns true for buffer emptied with pops") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.pop();
		buffer.pop();

		REQUIRE(buffer.empty());

	}

	SECTION("empty() returns true for buffer emptied with clear()") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.clear();

		REQUIRE(buffer.empty());

	}

	SECTION("empty() returns true for buffer emptied with erase()") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.erase(2);

		REQUIRE(buffer.empty());

	}

	SECTION("empty() is correct for buffer with wrapping") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.pop();
		buffer.push(3);

		REQUIRE_FALSE(buffer.empty());

		buffer.pop();
		buffer.pop();

		REQUIRE(buffer.empty());

	}

	SECTION("empty() is correct with head and tail at end of buffer") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.pop();
		buffer.pop();

		REQUIRE(buffer.empty());

		buffer.push(3);

		REQUIRE_FALSE(buffer.empty());

	}
	
}
	
TEST_CASE("RingBuffer::full()") {

	SECTION("full() is true for 0 capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.full());

	}
	
	SECTION("full() returns false for an empty buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE_FALSE(buffer.full());

	}

	SECTION("full() returns false for a non-full buffer") {

		RingBuffer<int> buffer(10);

		buffer.push(1);

		REQUIRE_FALSE(buffer.full());

	}

	SECTION("full() returns true for a full buffer") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		REQUIRE(buffer.full());

	}

	SECTION("full() returns false for a filled then emptied buffer") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.pop();
		buffer.pop();

		REQUIRE_FALSE(buffer.full());

	}

	SECTION("full() is correct for buffer with wrapping") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		buffer.pop();

		REQUIRE_FALSE(buffer.full());

		buffer.push(3);

		REQUIRE(buffer.full());

	}
	
}

TEST_CASE("RingBuffer::clear()") {

	SECTION("clear() does not disturb a 0 capacity buffer") {

		RingBuffer<int> buffer(0);

		buffer.clear();

		REQUIRE(buffer.empty());
		REQUIRE(buffer.size() == 0);

	}

	SECTION("clear() does not disturb an empty buffer") {

		RingBuffer<int> buffer(10);

		buffer.clear();

		REQUIRE(buffer.empty());
		REQUIRE(buffer.size() == 0);

	}

	SECTION("clear() empties the buffer") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		buffer.clear();

		REQUIRE(buffer.empty());

	}

	SECTION("clear() resets the size to 0") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		buffer.clear();

		REQUIRE(buffer.size() == 0);

	}

	SECTION("clear() resets empty()") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		buffer.clear();

		REQUIRE(buffer.empty());

	}

}

TEST_CASE("RingBuffer::erase()") {

	SECTION("erase() with 0 elements does nothing") {

		RingBuffer<int> buffer(10);

		buffer.push(1);

		REQUIRE(buffer.erase(0) == 0);

		REQUIRE(buffer.size() == 1);
		REQUIRE(buffer.front() == 1);

	}

	SECTION("erase() with 0 capacity buffer does nothing") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.erase(2) == 0);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());

	}

	SECTION("erase() removes elements from the buffer") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);
		buffer.push(3);

		REQUIRE(buffer.erase(2) == 2);

		REQUIRE(buffer.size() == 1);

	}

	SECTION("erase() has FIFO semantics") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);
		buffer.push(3);

		REQUIRE(buffer.erase(2) == 2);

		REQUIRE(buffer.front() == 3);

	}

	SECTION("erase() does nothing to an empty buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.erase(2) == 0);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());

	}

	SECTION("erase(n) is correct for a buffer with less than n elements") {

		RingBuffer<int> buffer(2);

		buffer.push(1);
		buffer.push(2);

		REQUIRE(buffer.erase(3) == 2);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());
		REQUIRE(buffer.capacity() == 2);

	}

	SECTION("erase() works with head and tail at end of buffer") {

		RingBuffer<int> buffer(3);

		buffer.push(1);
		buffer.push(2);
		buffer.push(3);

		buffer.pop();
		buffer.pop();
		buffer.pop();

		buffer.push(4);
		buffer.push(5);
		buffer.push(6);

		REQUIRE(buffer.erase(2) == 2);

		REQUIRE(buffer.size() == 1);
		REQUIRE(buffer.front() == 6);

	}

	SECTION("erase() wraps around correctly") {

		RingBuffer<int> buffer(3);

		buffer.push(1);
		buffer.push(2);
		buffer.push(3);

		buffer.pop();
		buffer.pop();

		buffer.push(4);
		buffer.push(5);

		REQUIRE(buffer.erase(2) == 2);

		REQUIRE(buffer.size() == 1);
		REQUIRE(buffer.front() == 5);

	}

}

TEST_CASE("RingBuffer::insert()") {

	SECTION("Insert does nothing for 0 capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.insert({1, 2}) == 0);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());

	}

	SECTION("Inserting nothing does not affect size or empty state") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());

		REQUIRE(buffer.insert({}) == 0);

		REQUIRE(buffer.size() == 0);
		REQUIRE(buffer.empty());

	}	

	SECTION("insert() adds elements to the buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.insert({1, 2}) == 2);

		REQUIRE(buffer.size() == 2);

		REQUIRE(buffer.get(2) == std::vector<int>{1, 2});

	}

	SECTION("insert() has FIFO semantics") {

		RingBuffer<int> buffer(3);

		REQUIRE(buffer.insert({1, 2}) == 2);

		REQUIRE(buffer.insert({3}) == 1);

		REQUIRE(buffer.get(3) == std::vector<int>{1, 2, 3});

	}

	SECTION("insert() is correct for a full buffer") {

		RingBuffer<int> buffer(2);

		buffer.insert({1, 2});

		REQUIRE(buffer.insert({3}) == 0);

		REQUIRE(buffer.get(2) == std::vector<int>{1, 2});

	}

	SECTION("insert() is correct when the buffer is too small") {

		RingBuffer<int> buffer(2);

		REQUIRE(buffer.insert({1, 2, 3}) == 2);

		REQUIRE(buffer.get(2) == std::vector<int>{1, 2});

	}

	SECTION("insert() wraps around correctly") {

		RingBuffer<int> buffer(4);

		buffer.insert({1, 2, 3, 4});

		buffer.pop();
		buffer.pop();

		REQUIRE(buffer.insert({5, 6}) == 2);

		REQUIRE(buffer.get(4) == std::vector<int>{3, 4, 5, 6});

	}

	SECTION("insert() works after clear()") {

		RingBuffer<int> buffer(2);

		REQUIRE(buffer.insert({1, 2}) == 2);

		buffer.clear();

		REQUIRE(buffer.insert({3, 4}) == 2);

		REQUIRE(buffer.get(2) == std::vector<int>{3, 4});

	}

	SECTION("insert() works with head and tail at end of buffer") {

		RingBuffer<int> buffer(2);

		buffer.insert({1, 2});

		buffer.pop();
		buffer.pop();

		REQUIRE(buffer.insert({3, 4}) == 2);

		REQUIRE(buffer.get(2) == std::vector<int>{3, 4});

	}

}

TEST_CASE("RingBuffer::get()") {

	SECTION("get() returns nothing for 0 capacity buffer") {

		RingBuffer<int> buffer(0);

		REQUIRE(buffer.get(2).empty());

	}

	SECTION("get() returns nothing for an empty buffer") {

		RingBuffer<int> buffer(10);

		REQUIRE(buffer.get(2).empty());

	}

	SECTION("get() returns a vector of the correct size") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		auto vec = buffer.get(2);

		REQUIRE(vec.size() == 2);

	}

	SECTION("get() returns the correct elements") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		auto vec = buffer.get(2);

		REQUIRE(vec[0] == 1);
		REQUIRE(vec[1] == 2);

	}

	SECTION("get() returns the correct elements with wrapping") {

		RingBuffer<int> buffer(5);

		buffer.push(1);
		buffer.push(2);
		buffer.push(3);
		buffer.push(4);
		buffer.push(5);

		buffer.pop();
		buffer.pop();

		buffer.push(6);
		buffer.push(7);

		auto vec = buffer.get(5);
		
		REQUIRE(vec[0] == 3);
		REQUIRE(vec[1] == 4);
		REQUIRE(vec[2] == 5);
		REQUIRE(vec[3] == 6);
		REQUIRE(vec[4] == 7);

	}

	SECTION("get() is correct when the buffer is too small") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		REQUIRE(buffer.get(3).size() == 2);
		REQUIRE(buffer.get(3) == std::vector<int>{1, 2});

	}

	SECTION("get() returns the correct elements with tail at end of buffer") {

		RingBuffer<int> buffer(5);

		buffer.push(1);
		buffer.push(2);
		buffer.push(3);
		buffer.push(4);
		buffer.push(5);

		buffer.pop();
		buffer.pop();

		buffer.push(6);
		buffer.push(7);

		buffer.pop();
		buffer.pop();
		buffer.pop();

		buffer.push(8);

		auto vec = buffer.get(3);

		REQUIRE(vec[0] == 6);
		REQUIRE(vec[1] == 7);
		REQUIRE(vec[2] == 8);

	}

}

TEST_CASE("RingBuffer stream extraction") {

	SECTION("Nothing happens for 0 capacity buffer") {

		RingBuffer<uint8_t> buffer(0);

		std::stringstream ss("\x01\x02\x03\x04");

		ss >> buffer;

		REQUIRE(buffer.empty());
		REQUIRE(buffer.size() == 0);

		uint8_t test;
		REQUIRE(ss >> test);
		REQUIRE(test == 1);

		REQUIRE(ss >> test);
		REQUIRE(test == 2);

		REQUIRE(ss >> test);
		REQUIRE(test == 3);

		REQUIRE(ss >> test);
		REQUIRE(test == 4);

		REQUIRE_FALSE(ss >> test);

	}

	SECTION("Empty istream clears buffer") {

		RingBuffer<uint8_t> buffer(10);

		buffer.push(1);

		std::stringstream ss;

		ss >> buffer;

		REQUIRE(buffer.empty());
		REQUIRE(buffer.size() == 0);

	}

	SECTION("Extraction is correct when buffer is bigger than stream") {

		RingBuffer<uint8_t> buffer(10);

		std::stringstream ss("\x01\x02\x03\x04\x05");

		ss >> buffer;

		REQUIRE(buffer.get(5) == std::vector<uint8_t>{1, 2, 3, 4, 5});
		REQUIRE(buffer.size() == 5);

		// Make sure we're at the end of the stream
		uint8_t temp;
		REQUIRE_FALSE(ss >> temp);

	}

	SECTION("Extraction is correct when stream fills buffer") {

		RingBuffer<uint8_t> buffer(5);

		std::stringstream ss("\x01\x02\x03\x04\x05");

		ss >> buffer;

		REQUIRE(buffer.get(5) == std::vector<uint8_t>{1, 2, 3, 4, 5});
		REQUIRE(buffer.size() == 5);

		// Make sure we're at the end of the stream
		uint8_t temp;
		REQUIRE_FALSE(ss >> temp);

	}

	SECTION("Extraction is correct when stream overflows buffer") {

		RingBuffer<uint8_t> buffer(5);

		std::stringstream ss("\x01\x02\x03\x04\x05\x06\x07\x08\x09");

		ss >> buffer;

		REQUIRE(buffer.get(5) == std::vector<uint8_t>{1, 2, 3, 4, 5});
		REQUIRE(buffer.size() == 5);

		// Make sure the stringstream cursor ended up in the right place
		uint8_t temp;
		REQUIRE(ss >> temp);
		REQUIRE(temp == 6);

	}

	SECTION("Extraction returns the stream cursor to the end of the last WHOLE element read") {

		std::stringstream ss("\x01\x02\x03");

		RingBuffer<uint16_t> buffer(3);

		ss >> buffer;

		// This will fail erroneously for big endian systems, so if your system
		// is big endian and this fails, don't worry about it. If you're a developer,
		// tweak the test to work on your system.
		REQUIRE(buffer.front() == 0x0201);
		REQUIRE(buffer.size() == 1);

		// Make sure the stringstream cursor ended up in the right place
		uint8_t temp;
		REQUIRE(ss >> temp);
		REQUIRE(temp == 3);


	}

	SECTION("Extraction is correct when uint16_t fills buffer") {

		RingBuffer<uint16_t> buffer(3);

		std::stringstream ss("\x01\x02\x03\x04\x05\x06");

		ss >> buffer;

		// This will fail erroneously for big endian systems, so if your system
		// is big endian and this fails, don't worry about it. If you're a developer,
		// tweak the test to work on your system.
		REQUIRE(buffer.get(3) == std::vector<uint16_t>{0x0201, 0x0403, 0x0605}); 
		REQUIRE(buffer.size() == 3);

		// Make sure we're at the end of the stream
		uint8_t temp;
		REQUIRE_FALSE(ss >> temp);

	}

}

TEST_CASE("RingBuffer Copy Constructor") {

	SECTION("Size is copied") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		RingBuffer<int> buffer2(buffer);

		REQUIRE(buffer2.size() == 2);

	}

	SECTION("Capacity is copied") {

		RingBuffer<int> buffer(10);

		RingBuffer<int> buffer2(buffer);

		REQUIRE(buffer2.capacity() == 10);

	}

	SECTION("Contents are copied") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		RingBuffer<int> buffer2(buffer);

		REQUIRE(buffer2.get(2) == std::vector<int>{1, 2});

	}

	SECTION("Empty status is copied") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		RingBuffer<int> buffer2(buffer);

		REQUIRE_FALSE(buffer2.empty());

		RingBuffer<int> buffer3(10);

		RingBuffer<int> buffer4(buffer3);

		REQUIRE(buffer4.empty());

	}

}

TEST_CASE("RingBuffer Copy Assignment") {

	SECTION("Size is copied") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		RingBuffer<int> buffer2(5);

		buffer2 = buffer;

		REQUIRE(buffer2.size() == 2);

	}

	SECTION("Capacity is copied") {

		RingBuffer<int> buffer(10);

		RingBuffer<int> buffer2(5);

		buffer2 = buffer;

		REQUIRE(buffer2.capacity() == 10);

	}

	SECTION("Contents are copied") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		RingBuffer<int> buffer2(5);

		buffer2 = buffer;

		REQUIRE(buffer2.get(2) == std::vector<int>{1, 2});

	}

	SECTION("Empty status is copied") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		RingBuffer<int> buffer2(5);

		buffer2 = buffer;

		REQUIRE_FALSE(buffer2.empty());

		RingBuffer<int> buffer3(10);

		RingBuffer<int> buffer4(5);

		buffer4 = buffer3;

		REQUIRE(buffer4.empty());

	}

	SECTION("Old contents are overwritten") {

		RingBuffer<int> buffer(10);

		buffer.push(1);
		buffer.push(2);

		RingBuffer<int> buffer2(5);

		buffer2.push(3);
		buffer2.push(4);

		buffer2 = buffer;

		REQUIRE(buffer2.get(2) == std::vector<int>{1, 2});

	}

}