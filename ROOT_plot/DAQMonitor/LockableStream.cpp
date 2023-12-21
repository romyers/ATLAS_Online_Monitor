#include "LockableStream.h"

using namespace std;

LockableStream::LockableStream() : stream(nullptr) {}

void LockableStream::lock  () { m.lock  (); }
void LockableStream::unlock() { m.unlock(); }