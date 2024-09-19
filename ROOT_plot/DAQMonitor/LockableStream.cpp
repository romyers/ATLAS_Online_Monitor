#include "LockableStream.h"

using namespace std;

LockableData::LockableData() {}

void LockableData::lock  () { m.lock  (); }
void LockableData::unlock() { m.unlock(); }